#pragma once

struct Vec3 {
  float x = 0.0F, y = 0.0F, z = 0.0F;

  // vec3 + vec3
  Vec3 operator+(const Vec3& other) const {
    return {x + other.x, y + other.y, z + other.z};
  }

  // vec3 * scalar (s)
  Vec3 operator*(const float scalar) const {
    return {x * scalar, y * scalar, z * scalar};
  }

  // vec3 - vec3
  Vec3 operator-(const Vec3& other) const {
    return {x - other.x, y - other.y, z - other.z};
  }

  // Vec3 += Vec3
  Vec3& operator+=(const Vec3& other) {
    x += other.x;
    y += other.y;
    z += other.z;
    return *this;
  }

  // Vec3 -= Vec3
  Vec3& operator-=(const Vec3& other) {
    x -= other.x;
    y -= other.y;
    z -= other.z;
    return *this;
  }
};

// scalar * vec3
inline Vec3 operator*(float scalar, const Vec3& v) {
  return {v.x * scalar, v.y * scalar, v.z * scalar};
}
