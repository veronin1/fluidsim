#pragma once

#include <string>

struct GLFWwindow;

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
  std::string title;
  GLFWwindow *window = nullptr;

  static void frameBufferSizeCallback(GLFWwindow *window, int width,
                                      int height);
};
