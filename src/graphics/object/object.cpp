#include "object.h"
#include <glm/gtx/quaternion.hpp>
#include "../mesh/meshManager.h"
#include "../shader/shaderManager.h"
#include "../texture/textureManager.h"

Object::Object() {
  m_position = glm::vec3(0.0);
  m_rotation = glm::angleAxis(0.0f, glm::vec3(0.0, 1.0, 0.0));
  m_scale = 70e3/1e3;
  m_meshFilePath = "../assets/models/sphere.obj";
  m_vertexShaderPath = "../shaders/phong.vs";
  m_fragShaderPath = "../shaders/phong.fs";
  m_diffuseMapFilePath = "";
  m_normalMapFilePath = "";
  m_specularMapFilePath = "";
  m_emissiveMapFilePath = "";
  m_isParticle = false;
}

void Object::setName(std::string name) {
  m_name = name;
}

std::string Object::getName() {
  return m_name;
}

glm::vec3 Object::getPosition() {
  return m_position;
}

void Object::setPosition(float x, float y, float z) {
  m_position = glm::vec3(x, y, z);
}

void Object::setPosition(glm::vec3 position) {
  m_position = position;
}

void Object::setRotation(float angle, glm::vec3 axis) {
  m_rotation = glm::angleAxis(angle, glm::normalize(axis));
}

void Object::rotate(glm::quat rotation) {
  m_rotation = rotation * m_rotation;
}

glm::mat4 Object::getRotationMat() {
  return glm::toMat4(m_rotation);
}

float Object::getScale() {
  return m_scale;
}

void Object::setScale(float scale) {
  m_scale = scale;
}

float Object::getAxis()
{
	return m_axis;
}

void Object::setAxis(float axis)
{
	m_axis = axis;
}

void Object::setMesh(std::string meshFilePath) {
  m_meshFilePath = meshFilePath;
}

std::string Object::getMesh() {
  return m_meshFilePath;
}

std::pair<std::string, std::string> Object::getShaders() {
  return std::make_pair(m_vertexShaderPath, m_fragShaderPath);
}

void Object::setShaders(std::string vertexShaderPath, std::string fragShaderPath) {
  m_vertexShaderPath = vertexShaderPath;
  m_fragShaderPath = fragShaderPath;
}

void Object::setDiffuseMap(std::string diffuseMapFilePath) {
  m_diffuseMapFilePath = diffuseMapFilePath;
}

void Object::setNormalMap(std::string normalMapFilePath) {
  m_normalMapFilePath = normalMapFilePath;
}

void Object::setSpecularMap(std::string specularMapFilePath) {
  m_specularMapFilePath = specularMapFilePath;
}

void Object::setEmissiveMap(std::string emissiveMapFilePath) {
  m_emissiveMapFilePath = emissiveMapFilePath;
}

std::vector<std::string> Object::getTextures() {
  return
  {
    m_diffuseMapFilePath,
    m_normalMapFilePath,
    m_specularMapFilePath,
    m_emissiveMapFilePath
  };
}

void Object::bind() {

  // First bind the shader
  ShaderManager* shaderManager = ShaderManager::getInstance();
  shaderManager->bindShader(m_vertexShaderPath, m_fragShaderPath);
  
  // Now bind geometry to buffer
  MeshManager* meshManager = MeshManager::getInstance();
  meshManager->bindMesh(m_meshFilePath);

  // Now bind textures
  TextureManager* textureManager = TextureManager::getInstance();
  std::vector<std::string > textures = getTextures();
  textureManager->bindTextures(textures);

}

glm::mat4 Object::getModelMatrix() {
  glm::mat4 model(1.0f);
  return glm::translate(glm::toMat4(m_rotation) * glm::scale(model, glm::vec3(m_scale)), m_position);
}

bool Object::isParticle() {
  return m_isParticle;
}

void Object::setIsParticle(bool isParticle) {
  m_isParticle = isParticle;
}
