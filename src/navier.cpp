#include "navier.hpp"

#include "liquid.hpp"
#include "vec3.hpp"
#include "vector_math.hpp"
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <vector>

constexpr size_t gridSizeX = 100;
constexpr size_t gridSizeY = 100;
constexpr size_t gridSizeZ = 50;

constexpr float DENSE_THRESHOLD = 0.9F;
constexpr float HIGH_THRESHOLD = 0.7F;
constexpr float MEDIUM_THRESHOLD = 0.5F;
constexpr float LOW_THRESHOLD = 0.3F;
constexpr float VERY_LOW_THRESHOLD = 0.1F;

int navier() {
  Grid3D grid(gridSizeX, gridSizeY, gridSizeZ);
  Liquid water(gridSizeX, gridSizeY, gridSizeZ, VISCOSITY_WATER_M2_PER_S,
               WATER_DIFFUSION_RATE);

  std::vector<float> divergence(grid.size());
  std::vector<float> pressure(grid.size());

  // initialise water density
  std::fill(water.density.begin(), water.density.end(),
            DENSITY_WATER_KG_PER_M3);

  const float deltaTime = 0.02F;
  const size_t numSteps = 100;

  for (size_t step = 0; step < numSteps; ++step) {
    simulateStep(grid, water, divergence, pressure, deltaTime);
    printDensitySlice(grid, water.density, gridSizeZ / 2);
  }

  return 0;
}

void printDensitySlice(Grid3D& grid, const std::vector<float>& density,
                       size_t zSlice) {
  float maxDensity = 0.0F;
  for (size_t y = 0; y < grid.ny; ++y) {
    for (size_t x = 0; x < grid.nx; ++x) {
      const int ix = static_cast<int>(x);
      const int iy = static_cast<int>(y);
      const float value = density[grid.idx(ix, iy, static_cast<int>(zSlice))];
      maxDensity = std::max(value, maxDensity);
    }
  }

  for (size_t y = 0; y < grid.ny; ++y) {
    for (size_t x = 0; x < grid.nx; ++x) {
      const int iy = static_cast<int>(y);
      const int ix = static_cast<int>(x);
      const float value = density[grid.idx(ix, iy, static_cast<int>(zSlice))];

      const float normalized = value / maxDensity;
      if (normalized > DENSE_THRESHOLD) {
        std::cout << "@";
      } else if (normalized > HIGH_THRESHOLD) {
        std::cout << "#";
      } else if (normalized > MEDIUM_THRESHOLD) {
        std::cout << "O";
      } else if (normalized > LOW_THRESHOLD) {
        std::cout << "o";
      } else if (normalized > VERY_LOW_THRESHOLD) {
        std::cout << ".";
      } else {
        std::cout << " ";
      }
    }
    std::cout << "\n";
  }
  std::cout << "\n";
}

void simulateStep(Grid3D& grid, Liquid& fluid, std::vector<float>& divergence,
                  std::vector<float>& pressure, float timeStep) {
  // 1. Apply external forces
  const Vec3 gravity{0, 0, -GRAVITY_FORCE_EARTH_M_PER_S2};
  applyForces(timeStep, gravity, fluid);

  // 2. Diffuse velocity
  std::vector<Vec3> tempVelocity(grid.size());
  diffuse(grid, fluid.velocity, tempVelocity, fluid.viscosity, timeStep);

  // 3. Project velocity
  project(grid, fluid, divergence, pressure);

  // 4. Advect velocity
  advect(grid, fluid.velocity, fluid.velocity, timeStep);

  // 5. Project again
  project(grid, fluid, divergence, pressure);

  // 6. Diffuse density
  std::vector<float> tempDensity(grid.size());
  diffuse(grid, fluid.density, tempDensity, fluid.diffusionRate, timeStep);

  // 7. Advect density
  advect(grid, fluid.velocity, fluid.density, timeStep);
}

// apply gravity (testing)
void applyForces(float timeStep, const Vec3& force, Liquid& fluid) {
  for (auto& vel : fluid.velocity) {
    vel += force * timeStep;
  }
}

// calculate projection
void project(Grid3D& grid, Liquid& fluid, std::vector<float>& divergence,
             std::vector<float>& pressure) {
  computeDivergence(grid, fluid.velocity, divergence);
  solvePressure(grid, divergence, pressure);
  subtractPressureGradient(grid, pressure, fluid.velocity);
}

void computeDivergence(Grid3D& grid, const std::vector<Vec3>& velocity,
                       std::vector<float>& divergence) {
  constexpr float DIV_FACTOR = 2.0F;
  for (size_t z = 0; z < grid.nz; ++z) {
    for (size_t y = 0; y < grid.ny; ++y) {
      for (size_t x = 0; x < grid.nx; ++x) {
        int ix = static_cast<int>(x);
        int iy = static_cast<int>(y);
        int iz = static_cast<int>(z);

        divergence[grid.idx(ix, iy, iz)] =
            (velocity[grid.idx(ix + 1, iy, iz)].x -
             velocity[grid.idx(ix - 1, iy, iz)].x +
             velocity[grid.idx(ix, iy + 1, iz)].y -
             velocity[grid.idx(ix, iy - 1, iz)].y +
             velocity[grid.idx(ix, iy, iz + 1)].z -
             velocity[grid.idx(ix, iy, iz - 1)].z) /
            DIV_FACTOR;
      }
    }
  }
}

void solvePressure(Grid3D& grid, std::vector<float>& divergence,
                   std::vector<float>& pressure) {
  constexpr float NUM_OF_NEIGHBOURS = 6.0F;
  constexpr size_t MAX_ITERATIONS = 20;

  std::vector<float> pressureTemp = pressure;

  for (size_t i = 0; i < MAX_ITERATIONS; ++i) {
    for (size_t z = 0; z < grid.nz; ++z) {
      for (size_t y = 0; y < grid.ny; ++y) {
        for (size_t x = 0; x < grid.nx; ++x) {
          int ix = static_cast<int>(x);
          int iy = static_cast<int>(y);
          int iz = static_cast<int>(z);

          size_t index = grid.idx(ix, iy, iz);

          pressureTemp[index] =
              (pressure[grid.idx(ix + 1, iy, iz)] +
               pressure[grid.idx(ix - 1, iy, iz)] +
               pressure[grid.idx(ix, iy + 1, iz)] +
               pressure[grid.idx(ix, iy - 1, iz)] +
               pressure[grid.idx(ix, iy, iz + 1)] +
               pressure[grid.idx(ix, iy, iz - 1)] - divergence[index]) /
              NUM_OF_NEIGHBOURS;
        }
      }
    }
    std::swap(pressure, pressureTemp);
  }
}

void subtractPressureGradient(Grid3D& grid, std::vector<float>& pressure,
                              std::vector<Vec3>& velocity) {
  constexpr float GRID_SPACING = 0.5F;

  for (size_t z = 0; z < grid.nz; ++z) {
    for (size_t y = 0; y < grid.ny; ++y) {
      for (size_t x = 0; x < grid.nx; ++x) {
        int ix = static_cast<int>(x);
        int iy = static_cast<int>(y);
        int iz = static_cast<int>(z);

        size_t index = grid.idx(ix, iy, iz);

        float gradX = ((pressure[grid.idx(ix + 1, iy, iz)] -
                        pressure[grid.idx(ix - 1, iy, iz)])) /
                      (2 * GRID_SPACING);
        float gradY = ((pressure[grid.idx(ix, iy + 1, iz)] -
                        pressure[grid.idx(ix, iy - 1, iz)])) /
                      (2 * GRID_SPACING);
        float gradZ = ((pressure[grid.idx(ix, iy, iz + 1)] -
                        pressure[grid.idx(ix, iy, iz - 1)])) /
                      (2 * GRID_SPACING);

        Vec3 grad{gradX, gradY, gradZ};
        velocity[index] -= grad;
      }
    }
  }
}
