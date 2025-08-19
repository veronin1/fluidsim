#pragma once

#include <GLFW/glfw3.h>

class WindowConfig;

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
