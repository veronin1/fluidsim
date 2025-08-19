#pragma once

#include <GLFW/glfw3.h>

#include <cstdint>

enum class OpenGLVersion : uint8_t { VERSION_MAJOR = 4, VERSION_MINOR = 6 };

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

class WindowManager {
 public:
  explicit WindowManager(const WindowConfig &config);
  ~WindowManager();

  [[nodiscard]] bool shouldClose() const;
  void swapBuffers();
  static void pollEvents();
  static GLFWwindow *getGLFWwindow();

 private:
  int width, height;
  const char *title = nullptr;
  GLFWwindow *window = nullptr;

  static void frameBufferSizeCallback(GLFWwindow *window, int width,
                                      int height);
};
