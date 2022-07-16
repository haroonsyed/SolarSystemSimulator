#include "gravBody.h"
#include <iostream>
#include <glm/gtc/constants.hpp>
using namespace nlohmann;

GravBody::GravBody() {
  m_velocity = glm::vec3(1.0);
	m_axis = glm::vec3(0.0f, 1.0f, 0.0f);
  m_mass = 1.0f;
  m_rotationSpeed = 1.0f;
}
GravBody::GravBody(float SIUnitScaleFactor, json jsonData) {
  setParamsFromJSON(SIUnitScaleFactor, jsonData);

  // Gravbody specific data
  std::string name = jsonData["name"].get<std::string>();
  addPlanetInfo(name);
  setMass(jsonData["mass"].get<float>() / SIUnitScaleFactor);
  setVelocity(
    jsonData["velocity"]["x"].get<float>() / SIUnitScaleFactor,
    jsonData["velocity"]["y"].get<float>() / SIUnitScaleFactor,
    jsonData["velocity"]["z"].get<float>() / SIUnitScaleFactor
  );
  setTilt(jsonData["tilt"].get<float>());
  setRotationSpeedFromPeriod(jsonData["rotationPeriod"].get<float>()); // Defined in hours!
  if (jsonData.contains("Type")) {
      addPlanetInfo("Type: " + jsonData["Type"].get<std::string>());
  }
  if (jsonData.contains("Radius")) {
      addPlanetInfo("Radius: " + jsonData["Radius"].get<std::string>());
  }
  if (jsonData.contains("Orbital Period")) {
      addPlanetInfo("Orbital Period: " + jsonData["Orbital Period"].get<std::string>());
  }
  if (jsonData.contains("Length of Day")) {
      addPlanetInfo("Length of a Day: " + jsonData["Length of Day"].get<std::string>());
  }
  if (jsonData.contains("Temperature")) {
      addPlanetInfo("Temperature: " + jsonData["Temperature"].get<std::string>());
  }
}

glm::vec3 GravBody::getVelocity() {
  return m_velocity;
}
void GravBody::setVelocity(float x, float y, float z) {
  m_velocity = glm::vec3(x, y, z);
}
void GravBody::setVelocity(glm::vec3 velocity) {
  m_velocity = velocity;
}
glm::vec3 GravBody::getAxis() {
	return m_axis;
}
void GravBody::setAxis(float x, float y, float z) {
	m_axis = glm::vec3(x, y, z);
}
void GravBody::setTilt(float degrees) {
  // Assuming degrees are from normal of earth's orbital plane around sun (defined as 0)
  double tiltRadians = glm::radians(degrees);
  glm::vec3 axis(
    -glm::sin(tiltRadians),
    0.0,
    glm::cos(tiltRadians)
  );
  m_axis = glm::normalize(axis);

}
float GravBody::getRotationSpeed() {
  return m_rotationSpeed;
}
void GravBody::setRotationSpeedFromPeriod(float hours) {
  m_rotationSpeed = (3.14159265f * 2.0f) / (hours * 60 * 60);
}
float GravBody::getMass() {
  return m_mass;
}
void GravBody::setMass(float mass) {
  m_mass = mass;
}
void GravBody::print() {
	const float vx = m_velocity.x;
	const float vy = m_velocity.y;
	const float vz = m_velocity.z;
	const float px = getPosition().x;
	const float py = getPosition().y;
	const float pz = getPosition().z;
}

void GravBody::addPlanetInfo(std::string info)
{
  m_planetInfo.push_back(info);
}

std::vector<std::string> GravBody::getPlanetInfo()
{
  return m_planetInfo;
}

