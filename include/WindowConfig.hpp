#pragma once

#include <string>
#include <utility>

class WindowConfig {
 public:
  static constexpr int DEFAULT_WIDTH = 800;
  static constexpr int DEFAULT_HEIGHT = 600;
  static constexpr const char* DEFAULT_TITLE = "fluidsim";

  int width;
  int height;
  std::string title;

  WindowConfig()
      : width(DEFAULT_WIDTH), height(DEFAULT_HEIGHT), title(DEFAULT_TITLE) {}

  WindowConfig(int winWidth, int winHeight, std::string winTitle)
      : width(winWidth), height(winHeight), title(std::move(winTitle)) {}
};
