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
  for (size_t z = 0; z < grid.nz; ++z) {
    for (size_t y = 0; y < grid.ny; ++y) {
      for (size_t x = 0; x < grid.nx; ++x) {
        size_t index = grid.idx(x, y, z);
        divergence[index] = computeDivergence(grid, fluid.velocity, divergence);
      }
    }
  }
}

void computeDivergence(Grid3D& grid, const std::vector<Vec3>& velocity,
                       std::vector<float>& divergence) {
  for (size_t z = 0; z < grid.nz; ++z) {
    for (size_t y = 0; y < grid.ny; ++y) {
      for (size_t x = 0; x < grid.nx; ++x) {
        divergence[grid.idx(x, y, z)] = (velocity[grid.idx(x + 1, y, z)].x -
                                         velocity[grid.idx(x - 1, y, z)].x +
                                         velocity[grid.idx(x, y + 1, z)].y -
                                         velocity[grid.idx(x, y - 1, z)].y +
                                         velocity[grid.idx(x, y, z + 1)].z -
                                         velocity[grid.idx(x, y, z - 1)].z) /
                                        2.0F;
      }
    }
  }
}
