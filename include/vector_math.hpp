#pragma once
#include "vec3.hpp"
#include <cmath>
#include <cstddef>
#include <vector>

struct Grid3D {
  size_t nx, ny, nz;
  Grid3D(size_t x, size_t y, size_t z) : nx(x), ny(y), nz(z) {}
  [[nodiscard]] size_t idx(size_t x, size_t y, size_t z) const {
    return x + nx * (y + ny * z);
  }
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
  int x0 = static_cast<int>(std::floor(pos.x)), x1 = x0 + 1;
  int y0 = static_cast<int>(std::floor(pos.y)), y1 = y0 + 1;
  int z0 = static_cast<int>(std::floor(pos.z)), z1 = z0 + 1;

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

  T f00 = linearInterpolate(f000, f100, u);
  T f10 = linearInterpolate(f010, f110, u);
  T f01 = linearInterpolate(f001, f101, u);
  T f11 = linearInterpolate(f011, f111, u);

  T f0 = linearInterpolate(f00, f10, v);
  T f1 = linearInterpolate(f01, f11, v);

  return linearInterpolate(f0, f1, w);
}
