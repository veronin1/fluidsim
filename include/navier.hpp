#pragma once

#include "liquid.hpp"
#include "vector_math.hpp"
#include <vector>

int navier();
void applyForces(float timeStep, Liquid& fluid);
void advectVelocity(Grid3D& grid, Liquid& fluid, float timeStep);
void advectDensity(Grid3D& grid, Liquid& fluid, float timeStep);

template <typename T>
void diffuse(Grid3D& grid, std::vector<T>& data, std::vector<T>& temp,
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
