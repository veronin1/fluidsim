#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include "FluidRenderer.hpp"
#include "WindowConfig.hpp"
#include "WindowManager.hpp"
#include "colour.hpp"
#include "navier.hpp"
#include "vec3.hpp"
#include <liquid.hpp>
#include <random>
#include <string>

constexpr size_t gridSizeX = 100;
constexpr size_t gridSizeY = 100;
constexpr size_t gridSizeZ = 50;

void processInput(GLFWwindow* window);

int main() {
  const WindowConfig config{1024, 768, "My Fluid Simulator"};
  WindowManager window(config);
  const Colour windowColour{0.2F, 0.3F, 0.3F, 1.0F};

  Grid3D grid(gridSizeX, gridSizeY, gridSizeZ);
  Liquid water(gridSizeX, gridSizeY, gridSizeZ, VISCOSITY_WATER_M2_PER_S,
               WATER_DIFFUSION_RATE);

  std::vector<float> divergence(grid.size());
  std::vector<float> pressure(grid.size());

  std::fill(water.density.begin(), water.density.end(),
            DENSITY_WATER_KG_PER_M3);

  FluidRenderer renderer(grid);
  float deltaTime = 0.02F;

  // render loop
  while (!window.shouldClose()) {
    processInput(WindowManager::getGLFWwindow());

    simulateStep(grid, water, divergence, pressure, deltaTime);

    renderer.updateSlice(water, grid);

    glClearColor(windowColour.r, windowColour.g, windowColour.b,
                 windowColour.a);
    glClear(GL_COLOR_BUFFER_BIT);

    renderer.draw();

    window.swapBuffers();
    WindowManager::pollEvents();
  }

  glfwTerminate();
  return 0;
}

void stirFluid(Liquid& fluid, Grid3D& grid) {}

void processInput(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, 1);
  }
}

Vec3 getRandomXYZ(size_t max_x, size_t max_y, size_t max_z) {
  std::random_device rd;
  std::mt19937 gen(rd());

  std::uniform_int_distribution<int> distrib_x(0, static_cast<int>(max_x));
  std::uniform_int_distribution<int> distrib_y(0, static_cast<int>(max_y));
  std::uniform_int_distribution<int> distrib_z(0, static_cast<int>(max_z));

  Vec3 random = {static_cast<float>(distrib_x(gen)),
                 static_cast<float>(distrib_y(gen)),
                 static_cast<float>(distrib_z(gen))};
  return random;
}
