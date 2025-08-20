#include "navier.hpp"

#include <math.h>

#include <algorithm>
#include <cmath>
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

void advectVelocity(Grid3D& grid, Liquid& fluid, float timeStep) {
  const Liquid& fluidPrev = fluid;
  constexpr float CELL_CENTER_OFFSET = 0.5F;

  for (size_t z = 0; z < grid.nz; ++z) {
    for (size_t y = 0; y < grid.ny; ++y) {
      for (size_t x = 0; x < grid.nx; ++x) {
        size_t index = grid.idx(x, y, z);
        Vec3 currentCell = {static_cast<float>(x) + CELL_CENTER_OFFSET,
                            static_cast<float>(y) + CELL_CENTER_OFFSET,
                            static_cast<float>(z) + CELL_CENTER_OFFSET};

        Vec3 prevPos = {
            currentCell.x - (fluidPrev.velocity[index].x * timeStep),
            currentCell.y - (fluidPrev.velocity[index].y * timeStep),
            currentCell.z - (fluidPrev.velocity[index].z * timeStep)};

        fluid.velocity[index] =
            trilinearInterpolate(grid, fluidPrev.velocity, prevPos);
      }
    }
  }
}

void advectDensity(Grid3D& grid, Liquid& fluid, float timeStep) {
  const Liquid& fluidPrev = fluid;
  constexpr float CELL_CENTER_OFFSET = 0.5F;

  for (size_t z = 0; z < grid.nz; ++z) {
    for (size_t y = 0; y < grid.ny; ++y) {
      for (size_t x = 0; x < grid.nx; ++x) {
        size_t index = grid.idx(x, y, z);
        Vec3 currentCell = {static_cast<float>(x) + CELL_CENTER_OFFSET,
                            static_cast<float>(y) + CELL_CENTER_OFFSET,
                            static_cast<float>(z) + CELL_CENTER_OFFSET};

        Vec3 prevPos = {
            currentCell.x - (fluidPrev.velocity[index].x * timeStep),
            currentCell.y - (fluidPrev.velocity[index].y * timeStep),
            currentCell.z - (fluidPrev.velocity[index].z * timeStep)};

        fluid.density[index] =
            trilinearInterpolate(grid, fluidPrev.density, prevPos);
      }
    }
  }
}

template <typename T>
T trilinearInterpolate(const Grid3D& grid, const std::vector<T>& field,
                       const Vec3& pos) {
  int x0 = -1;
  int y0 = -1;
  int z0 = -1;
  int x1 = -1;
  int y1 = -1;
  int z1 = -1;

  x0 = static_cast<int>(std::floor(pos.x)), x1 = x0 + 1;
  y0 = static_cast<int>(std::floor(pos.y)), y1 = y0 + 1;
  z0 = static_cast<int>(std::floor(pos.z)), z1 = z0 + 1;

  float u = pos.x - x0;
  float v = pos.y - y0;
  float w = pos.z - z0;

  T f000 = field[grid.idx(x0, y0, z0)];
  T f100 = field[grid.idx(x1, y0, z0)];
  T f010 = field[grid.idx(x0, y1, z0)];
  T f110 = field[grid.idx(x1, y1, z0)];
  T f001 = field[grid.idx(x0, y0, z1)];
  T f101 = field[grid.idx(x1, y0, z1)];
  T f011 = field[grid.idx(x0, y1, z1)];
  T f111 = field[grid.idx(x1, y1, z1)];

  // interpolate along X
  T f00 = linearInterpolate(f000, f100, u);
  T f10 = linearInterpolate(f010, f110, u);
  T f01 = linearInterpolate(f001, f101, u);
  T f11 = linearInterpolate(f011, f111, u);

  // interpolate along Y
  T f0 = linearInterpolate(f00, f10, v);
  T f1 = linearInterpolate(f01, f11, v);

  // interpolate along Z
  return linearInterpolate(f0, f1, w);
}

template <typename T>
T linearInterpolate(const T& a, const T& b, float t) {
  return a * (1 - t) + b * t;
}
