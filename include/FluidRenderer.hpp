#pragma once

#include "RenderPipeline.hpp"
#include "liquid.hpp"
#include "vector_math.hpp"

class FluidRenderer {
 private:
  RenderPipeline pipeline;
  GLuint textureID = 0;
  size_t sliceZ;

 public:
  explicit FluidRenderer(Grid3D& grid);
  void updateSlice(const Liquid& fluid, Grid3D& grid) const;
  void draw();
  ~FluidRenderer();
};
