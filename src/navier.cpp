
#include <cstddef>
#include <vector>
// xyz = 1 px of liquid

constexpr int numX = 100;
constexpr int numY = 100;
constexpr int numZ = 50;

struct Vec3 {
  float x, y, z;
};

std::vector<Vec3> velocity(static_cast<size_t>(numX* numY* numZ));
std::vector<float> density(static_cast<size_t>(numX* numY* numZ));
std::vector<float> pressure(static_cast<size_t>(numX* numY* numZ * 0.0F));

int navier(int x, int y, int z) {}
