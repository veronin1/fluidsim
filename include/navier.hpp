#pragma once

#include "liquid.hpp"
#include "vector_math.hpp"
#include <vector>

int navier();

void applyForces(float timeStep, Liquid& fluid);

template <typename VelT, typename FieldT>
void advect(Grid3D& grid, const std::vector<VelT>& velocityField,
            std::vector<FieldT>& field, float timeStep) {
  constexpr float CELL_CENTER_OFFSET = 0.5F;

  for (size_t z = 0; z < grid.nz; ++z) {
    for (size_t y = 0; y < grid.ny; ++y) {
      for (size_t x = 0; x < grid.nx; ++x) {
        size_t index = grid.idx(x, y, z);
        Vec3 currentCell = {static_cast<float>(x) + CELL_CENTER_OFFSET,
                            static_cast<float>(y) + CELL_CENTER_OFFSET,
                            static_cast<float>(z) + CELL_CENTER_OFFSET};

        Vec3 prevPos = currentCell - velocityField[index] * timeStep;

        field[index] = trilinearInterpolate(grid, field, prevPos);
      }
    }
  }
}

template <typename T>
void diffuse(Grid3D& grid, std::vector<T>& data, std::vector<T>& temp,
             float coefficient, float timeStep) {
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
          (*dst)[index] = (*src)[index] + coefficient * timeStep * laplacian;
        }
      }
    }
    std::swap(dst, src);
  }
  if (src != &data) {
    data = *src;
  }
}

void computeDivergence(Grid3D& grid, const std::vector<Vec3>& velocity,
                       std::vector<float>& divergence);
void solvePressure(Grid3D& grid, std::vector<float>& divergence,
                   std::vector<float>& pressure);
void subtractPressureGradient(Grid3D& grid, std::vector<float>& pressure,
                              std::vector<Vec3>& velocity);
void project(Grid3D& grid, Liquid& fluid, std::vector<float>& divergence,
             std::vector<float>& pressure);
void simulateStep(Grid3D& grid, Liquid& fluid, std::vector<float>& divergence,
                  std::vector<float>& pressure, float timeStep);
