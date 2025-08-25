#pragma once

#include <glad/glad.h>

#include <array>
#include <cstdlib>
#include <string>

constexpr size_t SHADER_INFO_LOG_SIZE = 512;

class RenderPipeline {
 public:
  RenderPipeline(const RenderPipeline &) = default;
  RenderPipeline(RenderPipeline &&) = delete;
  RenderPipeline &operator=(const RenderPipeline &) = default;
  RenderPipeline &operator=(RenderPipeline &&) = delete;
  explicit RenderPipeline(const std::array<float, 9> &vertices,
                          const std::string &vertexShaderSource,
                          const std::string &fragmentShaderSource);
  ~RenderPipeline();

  void draw() const;

 private:
  GLuint VAO{};
  GLuint VBO{};
  GLuint shaderProgram;
  GLsizei vertexCount{};

  static GLuint createVertexShader(const char *vertexShaderSource);
  static GLuint createFragmentShader(const char *fragmentShaderSource);
  static GLuint createShaderProgram(GLuint vertexShader, GLuint fragmentShader);
  void linkVertexAttributes();
};
