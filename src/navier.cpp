#include "navier.hpp"

#include "liquid.hpp"
#include "vec3.hpp"
#include "vector_math.hpp"
#include <algorithm>
#include <cstddef>
#include <vector>

constexpr size_t gridSizeX = 100;
constexpr size_t gridSizeY = 100;
constexpr size_t gridSizeZ = 50;

constexpr float DENSITY_WATER_KG_PER_M3 = 997.0F;
constexpr float GRAVITY_FORCE_EARTH_M_PER_S2 = 9.807F;

int navier() {
  Grid3D grid(gridSizeX, gridSizeY, gridSizeZ);

  std::vector<Vec3> velocity(gridSizeX * gridSizeY * gridSizeZ);
  std::vector<float> density(gridSizeX * gridSizeY * gridSizeZ);
  std::vector<float> pressure(gridSizeX * gridSizeY * gridSizeZ);

  // set density to water density (non-changing)
  std::fill(density.begin(), density.end(), DENSITY_WATER_KG_PER_M3);

  return 0;
}

// apply gravity (testing)
void applyForces(float timeStep, Liquid& fluid) {
  for (auto& vel : fluid.velocity) {
    vel.z += GRAVITY_FORCE_EARTH_M_PER_S2 * timeStep;
  }
}
