#include <GL/glew.h>
#include "scene.h"
#include <iostream>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include "nlohmann/json.hpp"
#include "../graphics/shader/shaderManager.h"
#include "../graphics/mesh/meshManager.h"
#include "../graphics/texture/textureManager.h"
#include "../config.h"

Scene::Scene(GLFWwindow* window) {
  m_universeScaleFactor = 1.0f;

  glGenBuffers(1, &m_modelBuffer);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_modelBuffer);
  const unsigned int numDynamicDataPoints = 16 * 3; // (scale, rot, translation)
  glBufferData(GL_SHADER_STORAGE_BUFFER, numDynamicDataPoints * 100000 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

  m_modelBufferSize = 0;

}

System* Scene::getPhysicsSystem() {
  return &m_physicsSystem;
}

Camera* Scene::getCamera() {
  return &m_camera;
}

float Scene::getUniverseScaleFactor() {
  return m_universeScaleFactor;
}

void Scene::loadScene(std::string sceneFilePath) {

  using namespace nlohmann; // json lib namespace

  // Load scene from json file
  std::ifstream file(sceneFilePath);
  std::string scene;
  std::getline(file, scene, '\0');
  json jScene = json::parse(scene);

  // Get units
  m_universeScaleFactor = jScene["UniverseScaleFactor"].get<float>();
  float physicsDistanceFactor = jScene["PhysicsDistanceFactor"].get<float>();
  float physicsMassFactor = jScene["PhysicsMassFactor"].get<float>();

  // Setup camera
  m_camera.setCameraPosition(
    glm::vec3(
      jScene["CameraPosition"]["x"].get<float>() / physicsDistanceFactor / m_universeScaleFactor,
      jScene["CameraPosition"]["y"].get<float>() / physicsDistanceFactor / m_universeScaleFactor,
      jScene["CameraPosition"]["z"].get<float>() / physicsDistanceFactor / m_universeScaleFactor
    )
  );

  // Setup physics
  m_physicsSystem.setPhysicsDistanceFactor(physicsDistanceFactor);
  m_physicsSystem.setPhysicsMassFactor(physicsMassFactor);

  // Construct scene. In units specified in SI units of json
  for (auto gravBodyJSON : jScene["GravBodies"]) {
    GravBody* body = new GravBody(physicsDistanceFactor, physicsMassFactor, gravBodyJSON);
    m_physicsSystem.addBody(body); // Add gravBody to physics system

    // Tell scene to register this object
    m_newAndUpdatedObjects.push_back(body);

  }

  // Construct lights
  for (auto lightJSON : jScene["Lights"]) {
    Light light;

    light.setPosition(
      lightJSON["position"]["x"].get<float>() / physicsDistanceFactor / m_universeScaleFactor,
      lightJSON["position"]["y"].get<float>() / physicsDistanceFactor / m_universeScaleFactor,
      lightJSON["position"]["z"].get<float>() / physicsDistanceFactor / m_universeScaleFactor
    );

    light.setColor(
      lightJSON["color"]["red"].get<float>(),
      lightJSON["color"]["green"].get<float>(),
      lightJSON["color"]["blue"].get<float>()
    );

    light.setIntensity(lightJSON["intensity"].get<float>());

    m_lights.push_back(light);

  }

}

void Scene::render() {

  // Get shaderProgram
  ShaderManager* shaderManager = ShaderManager::getInstance();
  MeshManager* meshManager = MeshManager::getInstance();
  TextureManager* textureManager = TextureManager::getInstance();
  unsigned int shaderProgram = shaderManager->getBoundShader();

  // Get view projection for the entire draw call
  glm::mat4 view = m_camera.getViewTransform();

  // Setup projection matrix for entire draw call
  Config* config = Config::getInstance();
  unsigned int SCR_WIDTH = config->getScreenWidth();
  unsigned int SCR_HEIGHT = config->getScreenHeight();
  glm::mat4 projection = glm::perspective(glm::radians(m_camera.getFov() / 2.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1e20f);

  // Setup light data
  // x,y,z,type(point/spotlight),r,g,b,strength
  std::vector<float> lightData;
  for (Light& light : m_lights) {
    glm::vec3 lightPos = light.getPosition() / m_universeScaleFactor;
    lightPos = view * glm::vec4(lightPos, 1.0);
    lightData.push_back(lightPos.x);
    lightData.push_back(lightPos.y);
    lightData.push_back(lightPos.z);
    lightData.push_back(0.0f);
    auto lightColor = light.getColor();
    lightData.insert(lightData.end(), lightColor.begin(), lightColor.end());
    lightData.push_back(light.getIntensity());
  }

  // Register new and updated objects to the scene and ssbo
  const unsigned int numDynamicDataPoints = 16 * 3; // (scale, rot, translation)
  for (Object* obj : m_newAndUpdatedObjects) {
    auto shaders = obj->getShaders();
    auto material = obj->getTextures();
    std::string materialName = "";
    for (auto str : material) {
      materialName += str;
    }

    // Setup model matrix data for this obj
    glm::mat4 scale = glm::mat4(1.0);
    scale = glm::scale(scale, glm::vec3(obj->getScale()));
    glm::mat4 rotation = obj->getRotationMat();
    glm::mat4 translation = glm::mat4(1.0f);
    translation = glm::translate(translation, obj->getPosition() / m_universeScaleFactor);

    std::vector<glm::mat4> modelData = { scale, rotation, translation };

    // Decide where to place in buffer
    auto& sameInstances = m_objects_map[shaders.first + shaders.second][obj->getMesh()][materialName];

    if (sameInstances.count(obj) == 0) {
      m_modelBufferSize += numDynamicDataPoints;
      sameInstances[obj] = m_modelBufferSize;
      glBufferSubData(GL_SHADER_STORAGE_BUFFER, sizeof(float) * m_modelBufferSize, sizeof(float) * numDynamicDataPoints, &modelData[0]);
    }
    else {
      // Update this object in the buffer
      unsigned int offset = sameInstances[obj];
      glBufferSubData(GL_SHADER_STORAGE_BUFFER, sizeof(float) * offset, sizeof(float) * numDynamicDataPoints, &modelData[0]);
    }

  }
  m_newAndUpdatedObjects.clear();

  // Compute model for all objects
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_modelBuffer);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_modelBuffer);
  shaderManager->bindComputeShader("../assets/shaders/compute/calculateModel.comp");
  glDispatchCompute(m_modelBufferSize / numDynamicDataPoints, 1, 1);
  glMemoryBarrier(GL_ALL_BARRIER_BITS);


  // Loop through the shaderPrograms, then loop through similar materials, then batch together all the similar objs for drawing
  for (auto const& it : m_objects_map) {

    auto const& shader = it.first;
    auto const& groupedMeshes = it.second;

    for (auto const& it : groupedMeshes) {

      auto const& meshFilePath = it.first;
      auto const& groupedMaterials = it.second;

      for (auto const& it : groupedMaterials) {

        auto const& materialName = it.first;
        auto const& objs = it.second;

        if (objs.size() == 0) {
          continue;
        }

        // Bind this instance type
        auto const& itr = objs.begin();
        
        Object* instance = itr->first;
        instance->bind();

        // Bind the uniform data for this instance
        unsigned int shaderProgram = shaderManager->getBoundShader();

        unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        unsigned int lightCountLoc = glGetUniformLocation(shaderProgram, "lightCount");
        glUniform1i(lightCountLoc, m_lights.size());

        unsigned int lightLoc = glGetUniformLocation(shaderProgram, "lights");
        glUniform1fv(lightLoc, lightData.size(), &(lightData[0]));

        // Set Dynamic attributes for each instance
        glBindBuffer(GL_ARRAY_BUFFER, m_modelBuffer);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(float) * numDynamicDataPoints, (void*)(0 * 4 * sizeof(float)));
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(float) * numDynamicDataPoints, (void*)(1 * 4 * sizeof(float)));
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(float) * numDynamicDataPoints, (void*)(2 * 4 * sizeof(float)));
        glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(float) * numDynamicDataPoints, (void*)(3 * 4 * sizeof(float)));
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);
        glVertexAttribDivisor(7, 1);

        glEnableVertexAttribArray(4);
        glEnableVertexAttribArray(5);
        glEnableVertexAttribArray(6);
        glEnableVertexAttribArray(7);
        
        // Render
        std::vector<unsigned int> bufferInfo = meshManager->getBufferInfo();
        const unsigned int numVertices = bufferInfo[2];
        glDrawArraysInstanced(GL_TRIANGLES, 0, numVertices, objs.size());
      
      }
    
    }

  }

}

