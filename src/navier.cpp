#include <math.h>

#include <algorithm>
#include <cmath>
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
void applyForces(float timeStep, Liquid& fluid) {
  for (auto& vel : fluid.velocity) {
    vel.z += GRAVITY_FORCE_EARTH_M_PER_S2 * timeStep;
  }
}

void advectVelocity(Grid3D& grid, Liquid& fluid, float timeStep) {
  const Liquid& fluidPrev = fluid;
  constexpr float CELL_CENTER_OFFSET = 0.5F;

  for (size_t z = 0; z < grid.nz; ++z) {
    for (size_t y = 0; y < grid.ny; ++y) {
      for (size_t x = 0; x < grid.nx; ++x) {
        size_t index = grid.idx(x, y, z);
        Vec3 currentCell = {static_cast<float>(x) + CELL_CENTER_OFFSET,
                            static_cast<float>(y) + CELL_CENTER_OFFSET,
                            static_cast<float>(z) + CELL_CENTER_OFFSET};

        Vec3 prevPos = {
            currentCell.x - (fluidPrev.velocity[index].x * timeStep),
            currentCell.y - (fluidPrev.velocity[index].y * timeStep),
            currentCell.z - (fluidPrev.velocity[index].z * timeStep)};
      }
    }
  }
}

Vec3 trilinearInterpolate(const Grid3D& grid, const std::vector<Vec3>& field,
                          const Vec3& pos) {
  int x0 = -1;
  int y0 = -1;
  int z0 = -1;
  int x1 = -1;
  int y1 = -1;
  int z1 = -1;

  x0 = static_cast<int>(std::floor(pos.x)), x1 = x0 + 1;
  y0 = static_cast<int>(std::floor(pos.y)), y1 = y0 + 1;
  z0 = static_cast<int>(std::floor(pos.z)), z1 = z0 + 1;

  float u = NAN;
  float v = NAN;
  float w = NAN;

  u = pos.x - x0;
  v = pos.y - y0;
  w = pos.z - z0;

  Vec3 f000;
  Vec3 f100;
  Vec3 f010;
  Vec3 f110;
  Vec3 f001;
  Vec3 f101;
  Vec3 f011;
  Vec3 f111;

  f000 = field[grid.idx(x0, y0, z0)];
  f100 = field[x1, y0, z0];
  f010 = field[x0, y1, z0];
}
