#pragma once

#include "vec3.hpp"
#include <vector>

constexpr float DENSITY_WATER_KG_PER_M3 = 997.0F;
constexpr float GRAVITY_FORCE_EARTH_M_PER_S2 = 9.807F;
constexpr float VISCOSITY_WATER_M2_PER_S = 1.0e-6F;
constexpr float WATER_DIFFUSION_RATE = 0.001F;

struct Liquid {
  std::vector<Vec3> velocity;
  std::vector<float> density;
  std::vector<float> pressure;
  float viscosity;
  float diffusionRate;

  Liquid(size_t nx, size_t ny, size_t nz, float visc, float diffRate)
      : velocity(nx * ny * nz),
        density(nx * ny * nz, DENSITY_WATER_KG_PER_M3),
        pressure(nx * ny * nz, 0.0F),
        viscosity(visc),
        diffusionRate(diffRate) {}
};
