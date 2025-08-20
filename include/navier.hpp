#pragma once

#include <vector>

struct Grid3D {
  size_t nx, ny, nz;
  Grid3D(size_t x, size_t y, size_t z) : nx(x), ny(y), nz(z) {}

  [[nodiscard]] size_t idx(size_t x, size_t y, size_t z) const {
    return x + nx * (y + ny * z);
  }
};

struct Vec3 {
  float x = 0.0F, y = 0.0F, z = 0.0F;

  // vec3 + vec3
  Vec3 operator+(const Vec3& other) const {
    return {x + other.x, y + other.y, z + other.z};
  }

  // vec3 * scalar (s)
  Vec3 operator*(const float s) const { return {x * s, y * s, z * s}; }
};

struct Liquid {
  std::vector<Vec3> velocity;
  std::vector<float> density;
  std::vector<float> pressure;
};

int navier();
void applyForces(float timeStep, Liquid& fluid);
void advectVelocity(Grid3D& grid, Liquid& fluid, float timeStep);
template <typename T>
T trilinearInterpolate(const Grid3D& grid, const std::vector<T>& field,
                       const Vec3& pos);
template <typename T>
T linearInterpolate(const T& a, const T& b, float t);
