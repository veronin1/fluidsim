#include "WindowManager.hpp"

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include "OpenGLVersion.hpp"
#include "WindowConfig.hpp"
#include <stdexcept>

WindowManager::WindowManager(const WindowConfig& config)
    : width(config.width), height(config.height), title(config.title) {
  if (glfwInit() == 0) {
    throw std::runtime_error("Failed to initialize GLFW");
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,
                 static_cast<int>(OpenGLVersion::VERSION_MAJOR));
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,
                 static_cast<int>(OpenGLVersion::VERSION_MINOR));
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
  if (window == nullptr) {
    glfwTerminate();
    throw std::runtime_error("Failed to create GLFW window");
  }

  glfwMakeContextCurrent(window);
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  if (gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)) ==
      0) {
    glfwTerminate();
    throw std::runtime_error("Failed to initialise GLAD");
  }
  glViewport(0, 0, config.width, config.height);

  glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);
}

WindowManager::~WindowManager() {
  if (window != nullptr) {
    glfwDestroyWindow(window);
  }
  glfwTerminate();
}

bool WindowManager::shouldClose() const {
  return glfwWindowShouldClose(window) != 0;
}

void WindowManager::swapBuffers() { glfwSwapBuffers(window); }

void WindowManager::pollEvents() { glfwPollEvents(); }

GLFWwindow* WindowManager::getGLFWwindow() { return glfwGetCurrentContext(); }

void WindowManager::frameBufferSizeCallback(GLFWwindow* window, int width,
                                            int height) {
  (void)window;
  glViewport(0, 0, width, height);
}
