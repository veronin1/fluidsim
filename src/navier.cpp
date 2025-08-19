#include <algorithm>
#include <cstddef>
#include <vector>

constexpr size_t gridSizeX = 100;
constexpr size_t gridSizeY = 100;
constexpr size_t gridSizeZ = 50;

constexpr float DENSITY_WATER_KG_PER_M3 = 997.0F;
constexpr float GRAVITY_FORCE_EARTH_M_PER_S2 = 9.807F;

struct Grid3D {
  size_t nx, ny, nz;
  Grid3D(size_t x, size_t y, size_t z) : nx(x), ny(y), nz(z) {}

  [[nodiscard]] size_t idx(size_t x, size_t y, size_t z) const {
    return x + nx * (y + ny * z);
  }
};

struct Vec3 {
  float x = 0.0f, y = 0.0f, z = 0.0f;
};

struct Liquid {
  std::vector<Vec3> velocity;
  std::vector<float> density;
  std::vector<float> pressure;
};

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
void applyForces(Grid3D& grid, float timeStep, std::vector<Vec3>& velocity) {
  for (size_t z = 0; z < grid.nz; ++z) {
    for (size_t y = 0; y < grid.ny; ++y) {
      for (size_t x = 0; x < grid.nx; ++x) {
        size_t i = grid.idx(x, y, z);
        velocity[i].z += GRAVITY_FORCE_EARTH_M_PER_S2 * timeStep;
      }
    }
  }
}

void advection(Grid3D& grid, float timeStep,
