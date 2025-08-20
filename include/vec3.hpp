#pragma once

struct Vec3 {
  float x = 0.0F, y = 0.0F, z = 0.0F;

  // vec3 + vec3
  Vec3 operator+(const Vec3& other) const {
    return {x + other.x, y + other.y, z + other.z};
  }

  // vec3 * scalar (s)
  Vec3 operator*(const float s) const { return {x * s, y * s, z * s}; }
};
