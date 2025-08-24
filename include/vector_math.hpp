#pragma once

#include "vec3.hpp"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <vector>

struct Grid3D {
  size_t nx, ny, nz;
  Grid3D(size_t x, size_t y, size_t z) : nx(x), ny(y), nz(z) {}
  [[nodiscard]] size_t idx(size_t x, size_t y, size_t z) const {
    x = std::clamp(x, size_t(0), nx - 1);
    y = std::clamp(y, size_t(0), ny - 1);
    z = std::clamp(z, size_t(0), nz - 1);

    return x + nx * (y + ny * z);
  }

  [[nodiscard]] size_t size() const { return nx * ny * nz; }
};

// linear interpolation
template <typename T>
T linearInterpolate(const T& a, const T& b, float t) {
  return a * (1 - t) + b * t;
}

// trilinear interpolation
template <typename T>
T trilinearInterpolate(const Grid3D& grid, const std::vector<T>& field,
                       const Vec3& pos) {
  // integer corner indices
  int x0 = static_cast<int>(std::floor(pos.x));
  int x1 = x0 + 1;
  int y0 = static_cast<int>(std::floor(pos.y));
  int y1 = y0 + 1;
  int z0 = static_cast<int>(std::floor(pos.z));
  int z1 = z0 + 1;

  // interpolation weights
  float u = pos.x - static_cast<float>(x0);
  float v = pos.y - static_cast<float>(y0);
  float w = pos.z - static_cast<float>(z0);

  //  get field values
  T f000 = field[grid.idx(static_cast<size_t>(x0), static_cast<size_t>(y0),
                          static_cast<size_t>(z0))];
  T f100 = field[grid.idx(static_cast<size_t>(x1), static_cast<size_t>(y0),
                          static_cast<size_t>(z0))];
  T f010 = field[grid.idx(static_cast<size_t>(x0), static_cast<size_t>(y1),
                          static_cast<size_t>(z0))];
  T f110 = field[grid.idx(static_cast<size_t>(x1), static_cast<size_t>(y1),
                          static_cast<size_t>(z0))];
  T f001 = field[grid.idx(static_cast<size_t>(x0), static_cast<size_t>(y0),
                          static_cast<size_t>(z1))];
  T f101 = field[grid.idx(static_cast<size_t>(x1), static_cast<size_t>(y0),
                          static_cast<size_t>(z1))];
  T f011 = field[grid.idx(static_cast<size_t>(x0), static_cast<size_t>(y1),
                          static_cast<size_t>(z1))];
  T f111 = field[grid.idx(static_cast<size_t>(x1), static_cast<size_t>(y1),
                          static_cast<size_t>(z1))];

  // trilinear interpolation
  T f00 = linearInterpolate(f000, f100, u);
  T f10 = linearInterpolate(f010, f110, u);
  T f01 = linearInterpolate(f001, f101, u);
  T f11 = linearInterpolate(f011, f111, u);

  T f0 = linearInterpolate(f00, f10, v);
  T f1 = linearInterpolate(f01, f11, v);

  return linearInterpolate(f0, f1, w);
}
