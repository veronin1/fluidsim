#pragma once

#include <glad/glad.h>

#include <array>

class RenderPipeline {
 public:
  RenderPipeline(const RenderPipeline &) = default;
  RenderPipeline(RenderPipeline &&) = delete;
  RenderPipeline &operator=(const RenderPipeline &) = default;
  RenderPipeline &operator=(RenderPipeline &&) = delete;
  explicit RenderPipeline(const std::array<float, 9> &vertices);
  ~RenderPipeline();

  void draw() const;

 private:
  GLuint VAO{};
  GLuint VBO{};
  GLuint shaderProgram;

  static GLuint createVertexShader();
  static GLuint createFragmentShader();
  static GLuint createShaderProgram(GLuint vertexShader, GLuint fragmentShader);
  static void linkVertexAttributes();
};
