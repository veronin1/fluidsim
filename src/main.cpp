#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <cstdint>

enum class Version : uint8_t {
  OPEN_GL_VERSION_MAJOR = 4,
  OPEN_GL_VERSION_MINOR = 8
};

class WindowConfig {
 public:
  static constexpr int DEFAULT_WIDTH = 800;
  static constexpr int DEFAULT_HEIGHT = 600;
  static constexpr const char *DEFAULT_TITLE = "fluidsim";

  int width;
  int height;
  const char *title;

  WindowConfig()
      : width(DEFAULT_WIDTH), height(DEFAULT_HEIGHT), title(DEFAULT_TITLE) {}

  WindowConfig(int winWidth, int winHeight, const char *winTitle)
      : width(winWidth), height(winHeight), title(winTitle) {}
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

  const WindowConfig config;

  GLFWwindow *window = glfwCreateWindow(config.width, config.height,
                                        config.title, nullptr, nullptr);
  if (window == nullptr) {
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
}
