#pragma once

#include "RenderPipeline.hpp"
#include "liquid.hpp"
#include "vector_math.hpp"

class FluidRenderer {
 private:
  GLuint textureID;
  GLuint shaderProgram;
  GLuint VAO, VBO;
  size_t sliceZ;

 public:
  FluidRenderer(Grid3D& grid);
  void updateSlice(const Liquid& fluid, Grid3D& grid);
  void draw();
  ~FluidRenderer();
};
