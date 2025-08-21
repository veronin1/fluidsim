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

void advectVelocity(Grid3D& grid, Liquid& fluid, float timeStep) {
  Liquid& fluidPrev = fluid;
  constexpr float CELL_CENTER_OFFSET = 0.5F;

  for (size_t z = 0; z < grid.nz; ++z) {
    for (size_t y = 0; y < grid.ny; ++y) {
      for (size_t x = 0; x < grid.nx; ++x) {
        size_t index = grid.idx(x, y, z);
        Vec3 currentCell = {static_cast<float>(x) + CELL_CENTER_OFFSET,
                            static_cast<float>(y) + CELL_CENTER_OFFSET,
                            static_cast<float>(z) + CELL_CENTER_OFFSET};

        Vec3 prevPos = currentCell - fluidPrev.velocity[index] * timeStep;

        fluid.velocity[index] =
            trilinearInterpolate(grid, fluidPrev.velocity, prevPos);
      }
    }
  }
}

void advectDensity(Grid3D& grid, Liquid& fluid, float timeStep) {
  Liquid& fluidPrev = fluid;
  constexpr float CELL_CENTER_OFFSET = 0.5F;

  for (size_t z = 0; z < grid.nz; ++z) {
    for (size_t y = 0; y < grid.ny; ++y) {
      for (size_t x = 0; x < grid.nx; ++x) {
        size_t index = grid.idx(x, y, z);
        Vec3 currentCell = {static_cast<float>(x) + CELL_CENTER_OFFSET,
                            static_cast<float>(y) + CELL_CENTER_OFFSET,
                            static_cast<float>(z) + CELL_CENTER_OFFSET};

        Vec3 prevPos = currentCell - fluidPrev.velocity[index] * timeStep;

        fluid.density[index] =
            trilinearInterpolate(grid, fluidPrev.density, prevPos);
      }
    }
  }
}

template <typename T>
void diffuseVelocity(Grid3D& grid, std::vector<T>& data, std::vector<T>& temp,
                     float diffusionRate, float timeStep) {
  constexpr float NUM_OF_NEIGHBOURS = 6.0F;
  constexpr size_t MAX_ITERATIONS = 20;

  std::vector<T>* src = &data;
  std::vector<T>* dst = &temp;

  for (size_t i = 0; i < MAX_ITERATIONS; ++i) {
    for (size_t z = 0; z < grid.nz; ++z) {
      for (size_t y = 0; y < grid.ny; ++y) {
        for (size_t x = 0; x < grid.nx; ++x) {
          size_t index = grid.idx(x, y, z);
          T neighbourSum{};

          // x neighbours
          neighbourSum += (*src)[grid.idx(x - 1, y, z)];
          neighbourSum += (*src)[grid.idx(x + 1, y, z)];

          // y neighbours
          neighbourSum += (*src)[grid.idx(x, y - 1, z)];
          neighbourSum += (*src)[grid.idx(x, y + 1, z)];

          // z neighbours

          neighbourSum += (*src)[grid.idx(x, y, z - 1)];
          neighbourSum += (*src)[grid.idx(x, y, z + 1)];

          // laplacian
          T laplacian = neighbourSum - NUM_OF_NEIGHBOURS * (*src)[index];

          // diffuse velocity
          (*dst)[index] = (*src)[index] + diffusionRate * timeStep * laplacian;
        }
      }
    }
    std::swap(dst, src);
  }
  if (src != &data) {
    data = *src;
  }
}

void diffuseDensity(Grid3D& grid, Liquid& fluid, float timeStep) {
  constexpr float NUM_OF_NEIGHBOURS = 6.0F;
  constexpr size_t MAX_ITERATIONS = 20;

  for (size_t i = 0; i < MAX_ITERATIONS; ++i) {
    Liquid fluidPrev = fluid;

    for (size_t z = 0; z < grid.nz; ++z) {
      for (size_t y = 0; y < grid.ny; ++y) {
        for (size_t x = 0; x < grid.nx; ++x) {
          size_t index = grid.idx(x, y, z);
          float neighbourSum = 0.0F;

          // x neighbours
          neighbourSum += fluidPrev.density[grid.idx(x - 1, y, z)];
          neighbourSum += fluidPrev.density[grid.idx(x + 1, y, z)];

          // y neighbours
          neighbourSum += fluidPrev.density[grid.idx(x, y - 1, z)];
          neighbourSum += fluidPrev.density[grid.idx(x, y + 1, z)];

          // z neighbours
          neighbourSum += fluidPrev.density[grid.idx(x, y, z - 1)];
          neighbourSum += fluidPrev.density[grid.idx(x, y, z + 1)];

          // laplacian
          float laplacian =
              neighbourSum - (NUM_OF_NEIGHBOURS * fluidPrev.density[index]);

          // diffuse density
          fluid.density[index] =
              fluid.density[index] + fluid.diffusionRate * timeStep * laplacian;
        }
      }
    }
  }
}
