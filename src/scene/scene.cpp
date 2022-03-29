#include "scene.h"
#include <iostream>
#include <fstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include "nlohmann/json.hpp"
#include "../graphics/shader/shaderManager.h"
#include "../graphics/mesh/meshManager.h"
#include "../config.h"

System* Scene::getPhysicsSystem() {
  return &m_physicsSystem;
}

void Scene::loadScene(std::string sceneFilePath) {

  using namespace nlohmann; // Testing lib namespace

  // Load scene from json file
  std::ifstream file(sceneFilePath);
  std::string scene;
  std::getline(file, scene, '\0');
  json jScene = json::parse(scene);

  // Construct scene. In units of MegaMeter and GigaGram
  for (auto gravBodyJSON : jScene["GravBodies"]) {
    GravBody* body = new GravBody();

    body->setName(gravBodyJSON["name"].get<std::string>());
    body->setMass(gravBodyJSON["mass"].get<float>() / 1e6);
    body->setPosition(
      gravBodyJSON["position"]["x"].get<float>()/1e6, 
      gravBodyJSON["position"]["y"].get<float>()/1e6,
      gravBodyJSON["position"]["z"].get<float>()/1e6
    );
    body->setVelocity(
      gravBodyJSON["velocity"]["x"].get<float>()/1e6,
      gravBodyJSON["velocity"]["y"].get<float>()/1e6,
      gravBodyJSON["velocity"]["z"].get<float>()/1e6
    );
    body->setMesh(gravBodyJSON["meshFilePath"].get<std::string>());
    body->setShaders(
      gravBodyJSON["vertexShaderPath"].get<std::string>(),
      gravBodyJSON["fragmentShaderPath"].get<std::string>()
    );
    body->setImageTexture(gravBodyJSON["textureFilePath"].get<std::string>());

    m_physicsSystem.addBody(body);

  }

  for (auto lightJSON : jScene["Lights"]) {
    Light light;

    light.setPosition(
      lightJSON["position"]["x"].get<float>()/1e6,
      lightJSON["position"]["y"].get<float>()/1e6,
      lightJSON["position"]["z"].get<float>()/1e6
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

void Scene::render(glm::mat4& view) {

  // Get shaderProgram
  ShaderManager* shaderManager = ShaderManager::getInstance();
  MeshManager* meshManager = MeshManager::getInstance();
  unsigned int shaderProgram = shaderManager->getBoundShader();

  // Setup projection matrix for entire draw call
  Config* config = Config::getInstance();
  unsigned int SCR_WIDTH = config->getScreenWidth();
  unsigned int SCR_HEIGHT = config->getScreenHeight();
  glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
  view = glm::translate(view, glm::vec3(0, 0, -2));

  // Setup light data
  std::vector<glm::vec3> lightPositions;
  for (Light &light : m_lights) {
    lightPositions.push_back(light.getPosition());
  }

  //unsigned int lightLocs = glGetUniformLocation(shaderProgram, "lightPositions");
  //glUniform3fv(lightLocs, lightPositions.size(), glm::value_ptr(&lightPositions[0]));
  glm::vec3 lightPos = glm::vec3(-1.0f, 0.0f, 0.0f);
  lightPos *= 10000;

  std::vector<Object*> objects;
  for (Object* bodyPtr : m_physicsSystem.getBodies()) {
    objects.push_back((Object*)bodyPtr);
  }

  for (Object* obj : objects) {

    // Setup transform matrix for this obj
    glm::mat4 scale = glm::mat4(1.0);
    scale = glm::scale(scale, glm::vec3(obj->getScale()));
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, (obj->getPosition() / 400e3f));

    obj->bind();

    //Pass to gpu
    unsigned int shaderProgram = shaderManager->getBoundShader();
    unsigned int scaleLoc = glGetUniformLocation(shaderProgram, "scale");
    glUniformMatrix4fv(scaleLoc, 1, GL_FALSE, glm::value_ptr(scale));
    unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    unsigned int lightLoc = glGetUniformLocation(shaderProgram, "lightPos");
    glUniform3fv(lightLoc, 1, glm::value_ptr(lightPos));

    std::vector<unsigned int> bufferInfo = meshManager->getBufferInfo();
    const unsigned int numVertices = bufferInfo[2];
    glDrawArrays(GL_TRIANGLES, 0, numVertices);

  }

}

