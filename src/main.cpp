#include <glad/glad.h>

#include <GLFW/glfw3.h>

#define OPEN_GL_VERSION_MAJOR 4
#define OPEN_GL_VERSION_MINOR 6

int main() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPEN_GL_VERSION_MAJOR);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPEN_GL_VERSION_MINOR);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}
