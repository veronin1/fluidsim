#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include "RenderPipeline.hpp"
#include "WindowConfig.hpp"
#include "WindowManager.hpp"
#include "colour.hpp"
#include <array>
#include <fstream>
#include <sstream>
#include <string>

std::string loadShaderSource(const std::string &filePath) {
  const std::ifstream file(filePath);
  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

const std::string vertexShaderSource = loadShaderSource("shaders/quad.vert");
const std::string fragmentShaderSource = loadShaderSource("shaders/quad.frag");

void processInput(GLFWwindow *window);

int main() {
  const WindowConfig config{1024, 768, "My Fluid Simulator"};
  WindowManager window(config);

  // triangle vertices
  const std::array<float, 9> vertices = {-0.5F, -0.5F, 0.0F, 0.5F, -0.5F,
                                         0.0F,  0.0F,  0.5F, 0.0F};

  const RenderPipeline render(vertices, vertexShaderSource,
                              fragmentShaderSource);

  const Colour windowColour{0.2F, 0.3F, 0.3F, 1.0F};

  // render loop
  while (!window.shouldClose()) {
    processInput(WindowManager::getGLFWwindow());

    glClearColor(windowColour.r, windowColour.g, windowColour.b,
                 windowColour.a);
    glClear(GL_COLOR_BUFFER_BIT);

    render.draw();

    window.swapBuffers();
    WindowManager::pollEvents();
  }

  glfwTerminate();
  return 0;
}

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, 1);
  }
}
