#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <cstdint>

enum class Version : uint8_t {
  OPEN_GL_VERSION_MAJOR = 4,
  OPEN_GL_VERSION_MINOR = 8
};

int main() {
  if (glfwInit() == 0) {
    return -1;
  };
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,
                 static_cast<int>(Version::OPEN_GL_VERSION_MAJOR));
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,
                 static_cast<int>(Version::OPEN_GL_VERSION_MINOR));
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}
