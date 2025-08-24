#pragma once

#include "liquid.hpp"
#include "vector_math.hpp"
#include <vector>

int navier();

void applyForces(float timeStep, const Vec3& force, Liquid& fluid);

void printDensitySlice(Grid3D& grid, const std::vector<float>& density,
                       size_t zSlice);

template <typename VelT, typename FieldT>
void advect(Grid3D& grid, const std::vector<VelT>& velocityField,
            std::vector<FieldT>& field, float timeStep) {
  constexpr float CELL_CENTER_OFFSET = 0.5F;

  for (size_t z = 0; z < grid.nz; ++z) {
    for (size_t y = 0; y < grid.ny; ++y) {
      for (size_t x = 0; x < grid.nx; ++x) {
        // cast loop indices to int for grid indexing
        int ix = static_cast<int>(x);
        int iy = static_cast<int>(y);
        int iz = static_cast<int>(z);

        size_t index = grid.idx(ix, iy, iz);
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
          // cast loop indices to int for neighbor calculations
          int ix = static_cast<int>(x);
          int iy = static_cast<int>(y);
          int iz = static_cast<int>(z);

          size_t index = grid.idx(ix, iy, iz);
          T neighbourSum{};

          // x neighbours
          neighbourSum += (*src)[grid.idx(ix - 1, iy, iz)];
          neighbourSum += (*src)[grid.idx(ix + 1, iy, iz)];

          // y neighbours
          neighbourSum += (*src)[grid.idx(ix, iy - 1, iz)];
          neighbourSum += (*src)[grid.idx(ix, iy + 1, iz)];

          // z neighbours
          neighbourSum += (*src)[grid.idx(ix, iy, iz - 1)];
          neighbourSum += (*src)[grid.idx(ix, iy, iz + 1)];

          // laplacian
          T laplacian = neighbourSum - NUM_OF_NEIGHBOURS * (*src)[index];

          // diffuse
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
