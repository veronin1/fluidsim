#pragma once

#include "vec3.hpp"
#include <vector>

struct Liquid {
  std::vector<Vec3> velocity;
  std::vector<float> density;
  std::vector<float> pressure;
};
