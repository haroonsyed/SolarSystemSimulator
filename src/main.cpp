// template based on material from learnopengl.com
#include <GL/glew.h>
#define GLFW_INCLUDE_NONE

#include <iostream>
#include <chrono>
#include <thread>

// My Imports
#include "config.h"
#include "./scene/scene.h"

// Prototypes
GLFWwindow* createWindow();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);


int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // Get the config
    Config* config = Config::getInstance();
    unsigned int TARGET_FRAMERATE = config->getTargetFramerate();


    // Make window 
    GLFWwindow* window = createWindow();
    if (!window) {
      std::cout << "Failed to create GLFW window" << std::endl;
      glfwTerminate();
      return -1;
    }

    // // glew: load all OpenGL function pointers
    glewInit();

    // Load scene
    Scene scene(window);
    scene.loadScene("../assets/scenes/testing.json");

    // render loop
    // -----------
    glEnable(GL_DEPTH_TEST);
    unsigned long frameCounter = 0;
    while (!glfwWindowShouldClose(window))
    {
      // Clear previous frame
      glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      double startTime = glfwGetTime();

      scene.update();
      scene.render();

      // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
      // -------------------------------------------------------------------------------
      glfwSwapBuffers(window);
      glfwPollEvents();

      // Measure performance
      double endTime = glfwGetTime();
      double frameTimeMS = 1000.0 * (endTime - startTime);
      double frameTime = endTime - startTime;
      double targetFrameTime = 1.0 / TARGET_FRAMERATE;

      // Spinlock cpu to stabalize fps to target
      while (frameTime < targetFrameTime) {
        frameTime = glfwGetTime() - startTime;
      }
      frameCounter++;


      if (endTime - startTime > targetFrameTime) {
        std::cout << "Framerate struggling to keep up!" << std::endl;
      }

      // Print FPS every n seconds
      if (std::fmod(glfwGetTime(), 5) < (1.0 / config->getTargetFramerate())) {
        std::cout << "\nOverall Average framerate: " << frameCounter / glfwGetTime() << " fps.\n" << std::endl;
      }
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

GLFWwindow* createWindow() {

  // Get the config
  Config* config = Config::getInstance();
  unsigned int SCR_WIDTH = config->getScreenWidth();
  unsigned int SCR_HEIGHT = config->getScreenHeight();
  unsigned int TARGET_FRAMERATE = config->getTargetFramerate();

  // glfw window creation
  // --------------------
  GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "viewGL", NULL, NULL);
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  return window;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
  // make sure the viewport matches the new window dimensions; note that width and 
  // height will be significantly larger than specified on retina displays.
  Config* config = Config::getInstance();
  config->setScreenWidth(width);
  config->setScreenHeight(height);
  glViewport(0, 0, width, height);
}
