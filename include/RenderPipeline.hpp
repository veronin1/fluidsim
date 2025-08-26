#pragma once

#include <glad/glad.h>

#include <array>
#include <cstdlib>
#include <string>

constexpr size_t SHADER_INFO_LOG_SIZE = 512;

constexpr size_t VERTICES_PER_QUAD = 6;
constexpr size_t FLOATS_PER_VERTEX = 4;
constexpr size_t VERTEX_ARRAY_SIZE = VERTICES_PER_QUAD * FLOATS_PER_VERTEX;

class RenderPipeline {
 public:
  RenderPipeline(const RenderPipeline &) = default;
  RenderPipeline(RenderPipeline &&) = delete;
  RenderPipeline &operator=(const RenderPipeline &) = default;
  RenderPipeline &operator=(RenderPipeline &&) = delete;
  explicit RenderPipeline(const std::array<float, VERTEX_ARRAY_SIZE> &vertices,
                          const std::string &vertexShaderSource,
                          const std::string &fragmentShaderSource);
  ~RenderPipeline();
  [[nodiscard]] GLuint getShaderProgram() const { return shaderProgram; }

  void draw() const;

 private:
  GLuint VAO{};
  GLuint VBO{};
  GLuint shaderProgram;
  GLsizei vertexCount{};

  static GLuint createVertexShader(const char *vertexShaderSource);
  static GLuint createFragmentShader(const char *fragmentShaderSource);
  static GLuint createShaderProgram(GLuint vertexShader, GLuint fragmentShader);
  static void linkVertexAttributes();
};

// load Shader from filePath
std::string loadShaderSource(const std::string &filePath);
