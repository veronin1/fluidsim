#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <cstdint>
#include <iostream>

enum class Version : uint8_t {
  OPEN_GL_VERSION_MAJOR = 4,
  OPEN_GL_VERSION_MINOR = 6
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

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  (void)window;
  glViewport(0, 0, width, height);
}

int main() {
  if (glfwInit() == 0) {
    std::cerr << "Failed to initialise GLFW" << '\n';
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
    std::cerr << "Failed to create GLFW window" << '\n';
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  if (gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)) ==
      0) {
    std::cerr << "Failed to initialise GLAD" << '\n';
    glfwTerminate();
    return -1;
  }

  glViewport(0, 0, config.width, config.height);

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  while (glfwWindowShouldClose(window) == 0) {
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
