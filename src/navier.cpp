#include "navier.hpp"

#include "liquid.hpp"
#include "vec3.hpp"
#include "vector_math.hpp"
#include <algorithm>
#include <cstddef>
#include <vector>

constexpr size_t gridSizeX = 100;
constexpr size_t gridSizeY = 100;
constexpr size_t gridSizeZ = 50;

constexpr float DENSITY_WATER_KG_PER_M3 = 997.0F;
constexpr float GRAVITY_FORCE_EARTH_M_PER_S2 = 9.807F;

int navier() {
  Grid3D grid(gridSizeX, gridSizeY, gridSizeZ);

  std::vector<Vec3> velocity(gridSizeX * gridSizeY * gridSizeZ);
  std::vector<float> density(gridSizeX * gridSizeY * gridSizeZ);
  std::vector<float> pressure(gridSizeX * gridSizeY * gridSizeZ);

  // set density to water density (non-changing)
  std::fill(density.begin(), density.end(), DENSITY_WATER_KG_PER_M3);

  return 0;
}

// apply gravity (testing)
void applyForces(float timeStep, Liquid& fluid) {
  for (auto& vel : fluid.velocity) {
    vel.z += GRAVITY_FORCE_EARTH_M_PER_S2 * timeStep;
  }
}

// calculate projection
void project(float timeStep, Grid3D& grid, Liquid& fluid,
             std::vector<float>& divergence) {
  computeDivergence(grid, fluid.velocity, divergence);
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

          pressureTemp[index] = 0;

          // x neighbours
          pressureTemp[index] += pressure[grid.idx(x + 1, y, z)];
          pressureTemp[index] += pressure[grid.idx(x - 1, y, z)];

          // y neighbours
          pressureTemp[index] += pressure[grid.idx(x, y + 1, z)];
          pressureTemp[index] += pressure[grid.idx(x, y - 1, z)];

          // z neighbours
          pressureTemp[index] += pressure[grid.idx(x, y, z + 1)];
          pressureTemp[index] += pressure[grid.idx(x, y, z - 1)];

          float laplacian =
              pressureTemp[index] - (NUM_OF_NEIGHBOURS * pressure[index]);

          pressure[index] = (laplacian - divergence[index]) / NUM_OF_NEIGHBOURS;
        }
      }
    }
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
