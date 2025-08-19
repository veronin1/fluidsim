#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include "RenderPipeline.hpp"
#include "WindowConfig.hpp"
#include "WindowManager.hpp"
#include "colour.hpp"
#include <array>

const char *const vertexShaderSource =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

const char *const fragmentShaderSource =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    FragColor = vec4(0.47f, 0.27f, 0.035f, 1.0f);\n"
    "}\n";

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
