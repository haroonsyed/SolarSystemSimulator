#include "inputController.h"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../config.h"

InputController::InputController(GLFWwindow* window) {
  // glfwSetCursorPos(m_window, SCR_WIDTH/2, SCR_HEIGHT/2);
  m_window = window;
}
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void InputController::processInput()
{

  // Get config data
  Config* config = Config::getInstance();
  unsigned int SCR_WIDTH = config->getScreenWidth();
  unsigned int SCR_HEIGHT = config->getScreenHeight();
  unsigned int INPUT_POLL_RATE = config->getInputPollRate();

  // CLOSE APPLICATION
  if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(m_window, true);
  }
  

  // Reset time since last input
  m_timeAtLastInput = glfwGetTime();

  m_pressedKeys.clear();

  // Get the relevant keys that are pressed
  if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS) {
      m_pressedKeys.insert(GLFW_KEY_W);
  }
  if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS) {
      m_pressedKeys.insert(GLFW_KEY_A);
  }
  if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS) {
      m_pressedKeys.insert(GLFW_KEY_S);
  }
  if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS) {
      m_pressedKeys.insert(GLFW_KEY_D);
  }
  if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
      m_pressedKeys.insert(GLFW_MOUSE_BUTTON_MIDDLE);
  }

  if(glfwGetKey(m_window, GLFW_KEY_G) == GLFW_PRESS) {
      guiButtonPressed = true;
  }

  if (glfwGetKey(m_window, GLFW_KEY_G) == GLFW_RELEASE && guiButtonPressed)
  {
      m_pressedKeys.insert(GLFW_KEY_G);
      guiButtonPressed = false;
  }

  // Get the amount the mouse has moved
  // Get the amount the mouse has moved
  glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
  glfwGetCursorPos(m_window, &deltaX, &deltaY);
  deltaX -= SCR_WIDTH / 2;
  deltaY -= SCR_HEIGHT / 2;
  glfwSetCursorPos(m_window, SCR_WIDTH/2, SCR_HEIGHT/2);

}

std::unordered_set<unsigned int>* InputController::getPressedKeys()
{
    return &m_pressedKeys;
}

float InputController::getMouseDeltaX()
{
    return deltaX;
}

float InputController::getMouseDeltaY()
{
    return deltaY;
}

