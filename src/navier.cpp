#include <algorithm>
#include <cstddef>
#include <vector>

constexpr int numX = 100;
constexpr int numY = 100;
constexpr int numZ = 50;

constexpr float DENSITY_WATER_KG_PER_M3 = 997.0F;
constexpr float GRAVITY_FORCE_EARTH_M_PER_S2 = 9.807F;

inline int idx(int x, int y, int z) { return x + (numX * (numY * z + y)); }

struct Vec3 {
  float x, y, z;
};

std::vector<Vec3> velocity(static_cast<size_t>(numX* numY* numZ));
std::vector<float> density(static_cast<size_t>(numX* numY* numZ));
std::vector<float> pressure(static_cast<size_t>(numX* numY* numZ));

int navier() {
  // set density to water density (non-changing)
  std::fill(density.begin(), density.end(), DENSITY_WATER_KG_PER_M3);

  // set pressure equal to earth gravity
  std::fill(pressure.begin(), pressure.end(), GRAVITY_FORCE_EARTH_M_PER_S2);

  return 0;
}
