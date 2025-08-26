#include "FluidRenderer.hpp"

#include "RenderPipeline.hpp"
#include "navier.hpp"

// FluidRenderer::FluidRenderer(Grid3D &grid) : sliceZ(grid.nz / 2) {
//   shaderProgram =
// }

void FluidRenderer::updateSlice(const Liquid& fluid, Grid3D& grid) {
  float maxDensity = 0.0F;
  for (size_t y = 0; y < grid.ny; ++y) {
    for (size_t x = 0; x < grid.nx; ++x) {
      const int ix = static_cast<int>(x);
      const int iy = static_cast<int>(y);
      const float value =
          fluid.density[grid.idx(ix, iy, static_cast<int>(sliceZ))];
      maxDensity = std::max(value, maxDensity);
    }
  }

  std::vector<float> sliceArray(grid.nx * grid.ny);

  for (size_t y = 0; y < grid.ny; ++y) {
    for (size_t x = 0; x < grid.nx; ++x) {
      const int ix = static_cast<int>(x);
      const int iy = static_cast<int>(y);
      const float value =
          fluid.density[grid.idx(ix, iy, static_cast<int>(sliceZ))];
      const float normalised = value / maxDensity;

      size_t flatIndex = (y * grid.nx) + x;
      sliceArray[flatIndex] = normalised;
    }
  }

  glBindTexture(GL_TEXTURE_2D, textureID);

  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, static_cast<GLsizei>(grid.nx),
                  static_cast<GLsizei>(grid.ny), GL_RED, GL_FLOAT,
                  sliceArray.data());
}
