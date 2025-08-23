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

int navier() {
  Grid3D grid(gridSizeX, gridSizeY, gridSizeZ);
  Liquid water(gridSizeX, gridSizeY, gridSizeZ, VISCOSITY_WATER_M2_PER_S,
               WATER_DIFFUSION_RATE);

  std::vector<float> divergence(grid.size());
  std::vector<float> pressure(grid.size());

  // initialise water density
  std::fill(water.density.begin(), water.density.end(),
            DENSITY_WATER_KG_PER_M3);

  // Put a blob of high density in the center
  for (size_t z = 20; z < 30; ++z) {
    for (size_t y = 40; y < 60; ++y) {
      for (size_t x = 40; x < 60; ++x) {
        water.density[grid.idx(x, y, z)] = DENSITY_WATER_KG_PER_M3 * 2.0F;
      }
    }
  }

  float dt = 0.01F;
  size_t numSteps = 100;

  for (size_t step = 0; step < numSteps; ++step) {
    simulateStep(grid, water, divergence, pressure, dt);
    printDensitySlice(grid, water.density, gridSizeZ / 2);
  }

  return 0;
}

void printDensitySlice(Grid3D& grid, const std::vector<float>& density,
                       size_t zSlice) {
  for (size_t y = 0; y < grid.ny; ++y) {
    for (size_t x = 0; x < grid.nx; ++x) {
      float value = density[grid.idx(x, y, zSlice)];

      float normalized = value / DENSITY_WATER_KG_PER_M3;
      if (normalized > 1.0F)
        std::cout << "@";
      else if (normalized > 0.8F)
        std::cout << "#";
      else if (normalized > 0.6F)
        std::cout << "O";
      else if (normalized > 0.4F)
        std::cout << "o";
      else if (normalized > 0.2F)
        std::cout << ".";
      else
        std::cout << " ";
    }
    std::cout << "\n";
  }
  std::cout << "\n";
}

void simulateStep(Grid3D& grid, Liquid& fluid, std::vector<float>& divergence,
                  std::vector<float>& pressure, float timeStep) {
  // 1. Apply external forces
  Vec3 gravity{0, 0, GRAVITY_FORCE_EARTH_M_PER_S2};
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
        divergence[grid.idx(x, y, z)] = (velocity[grid.idx(x + 1, y, z)].x -
                                         velocity[grid.idx(x - 1, y, z)].x +
                                         velocity[grid.idx(x, y + 1, z)].y -
                                         velocity[grid.idx(x, y - 1, z)].y +
                                         velocity[grid.idx(x, y, z + 1)].z -
                                         velocity[grid.idx(x, y, z - 1)].z) /
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
          size_t index = grid.idx(x, y, z);

          pressureTemp[index] =
              (pressure[grid.idx(x + 1, y, z)] +
               pressure[grid.idx(x - 1, y, z)] +
               pressure[grid.idx(x, y + 1, z)] +
               pressure[grid.idx(x, y - 1, z)] +
               pressure[grid.idx(x, y, z + 1)] +
               pressure[grid.idx(x, y, z - 1)] - divergence[index]) /
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
        size_t index = grid.idx(x, y, z);
        float gradX = ((pressure[grid.idx(x + 1, y, z)] -
                        pressure[grid.idx(x - 1, y, z)])) /
                      (2 * GRID_SPACING);
        float gradY = ((pressure[grid.idx(x, y + 1, z)] -
                        pressure[grid.idx(x, y - 1, z)])) /
                      (2 * GRID_SPACING);
        float gradZ = ((pressure[grid.idx(x, y, z + 1)] -
                        pressure[grid.idx(x, y, z - 1)])) /
                      (2 * GRID_SPACING);

        Vec3 grad{gradX, gradY, gradZ};
        velocity[index] -= grad;
      }
    }
  }
}
