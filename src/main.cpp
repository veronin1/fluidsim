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

constexpr float DEFAULT_DELTA_TIME = 0.02F;

void processInput(GLFWwindow* window);
Vec3 getRandomXYZ();
void stirFluid(Liquid& fluid, Grid3D& grid);

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
  const float deltaTime = DEFAULT_DELTA_TIME;

  // render loop
  while (!window.shouldClose()) {
    processInput(WindowManager::getGLFWwindow());

    stirFluid(water, grid);
    
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

void stirFluid(Liquid& fluid, Grid3D& grid) {
  for (size_t z = 0; z < grid.nx; ++z) {
    for (size_t y = 0; y < grid.ny; ++y) {
      for (size_t x = 0; x < grid.nx; ++x) {
        const int ix = static_cast<int>(x);
        const int iy = static_cast<int>(y);
        const int iz = static_cast<int>(z);

        fluid.velocity[grid.idx(ix, iy, iz)] +=
          getRandomXYZ();
      }
    }
  }
}

void processInput(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, 1);
  }
}

Vec3 getRandomXYZ() {
  static std::random_device rd;
  static std::mt19937 gen(rd());


  std::uniform_real_distribution<float> distrib(-0.1F, 0.1F);
  return {distrib(gen), distrib(gen), distrib(gen)};
}
