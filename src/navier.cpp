#include <algorithm>
#include <cstddef>
#include <vector>

using std::vector;

constexpr int gridSizeX = 100;
constexpr int gridSizeY = 100;
constexpr int gridSizeZ = 50;

constexpr float DENSITY_WATER_KG_PER_M3 = 997.0F;
constexpr float GRAVITY_FORCE_EARTH_M_PER_S2 = 9.807F;

struct Grid3D {
  int nx, ny, nz;
  Grid3D(int x, int y, int z) : nx(x), ny(y), nz(z) {}

  [[nodiscard]] int idx(int x, int y, int z) const;
};

int Grid3D::idx(int x, int y, int z) const { return x + (nx * (y + ny * z)); }

struct Vec3 {
  float x, y, z;
};

vector<Vec3> velocity(static_cast<size_t>(gridSizeX* gridSizeY* gridSizeZ));
std::vector<float> density(
    static_cast<size_t>(gridSizeX* gridSizeY* gridSizeZ));
std::vector<float> pressure(
    static_cast<size_t>(gridSizeX* gridSizeY* gridSizeZ));

int navier() {
  Grid3D grid(gridSizeX, gridSizeY, gridSizeZ);

  // set density to water density (non-changing)
  std::fill(density.begin(), density.end(), DENSITY_WATER_KG_PER_M3);

  // set pressure equal to earth gravity
  std::fill(pressure.begin(), pressure.end(), GRAVITY_FORCE_EARTH_M_PER_S2);

  return 0;
}
