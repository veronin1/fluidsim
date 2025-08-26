#include "FluidRenderer.hpp"

#include "RenderPipeline.hpp"
#include "liquid.hpp"
#include <vector>

constexpr float LEFT = -1.0F;
constexpr float RIGHT = 1.0F;
constexpr float BOTTOM = -1.0F;
constexpr float TOP = 1.0F;

constexpr float U0 = 0.0F;
constexpr float U1 = 1.0F;
constexpr float V0 = 0.0F;
constexpr float V1 = 1.0F;

// 2 triangles (quad)
const std::array<float, 24> vertices = {
    // x, y, u, v
    LEFT, BOTTOM, U0, V0, RIGHT, BOTTOM, U1, V0, RIGHT, TOP, U1, V1,

    LEFT, BOTTOM, U0, V0, RIGHT, TOP,    U1, V1, LEFT,  TOP, U0, V1};

const std::string vertexShaderSource = loadShaderSource("shaders/quad.vert");
const std::string fragmentShaderSource = loadShaderSource("shaders/quad.frag");

FluidRenderer::FluidRenderer(Grid3D& grid)
    : pipeline(vertices, vertexShaderSource, fragmentShaderSource),
      sliceZ(grid.nz / 2) {
  // Create Texture
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_2D, textureID);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, static_cast<GLsizei>(grid.nx),
               static_cast<GLsizei>(grid.ny), 0, GL_RED, GL_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void FluidRenderer::updateSlice(const Liquid& fluid, Grid3D& grid) const {
  std::vector<float> sliceArray(grid.nx * grid.ny);

  float maxSpeed = 0.0f;
  for (size_t y = 0; y < grid.ny; ++y) {
    for (size_t x = 0; x < grid.nx; ++x) {
      const int ix = static_cast<int>(x), iy = static_cast<int>(y);
      const int iz = static_cast<int>(grid.nz) / 2;

      Vec3 vel = fluid.velocity[grid.idx(ix, iy, iz)];
      float speed = std::sqrt(vel.x * vel.x + vel.y * vel.y + vel.z * vel.z);
      if (speed > maxSpeed) maxSpeed = speed;
      sliceArray[y * grid.nx + x] = speed / (maxSpeed + 1e-6f);
    }
  }

  glBindTexture(GL_TEXTURE_2D, textureID);

  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, static_cast<GLsizei>(grid.nx),
                  static_cast<GLsizei>(grid.ny), GL_RED, GL_FLOAT,
                  sliceArray.data());
}

void FluidRenderer::draw() {
  glBindTexture(GL_TEXTURE_2D, textureID);
  pipeline.draw();
}

FluidRenderer::~FluidRenderer() { glDeleteTextures(1, &textureID); }
