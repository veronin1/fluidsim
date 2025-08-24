#include "RenderPipeline.hpp"

#include <iostream>

RenderPipeline::RenderPipeline(const std::array<float, 9>& vertices,
                               const char* vertexShaderSource,
                               const char* fragmentShaderSource)
    : vertexCount(static_cast<GLsizei>(vertices.size() / 3)) {
  const GLuint vertexShader = createVertexShader(vertexShaderSource);
  const GLuint fragmentShader = createFragmentShader(fragmentShaderSource);
  shaderProgram = createShaderProgram(vertexShader, fragmentShader);

  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER,
               static_cast<GLsizeiptr>(vertices.size() * sizeof(float)),
               vertices.data(), GL_STATIC_DRAW);

  linkVertexAttributes();
}

RenderPipeline::~RenderPipeline() {
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteProgram(shaderProgram);
}

void RenderPipeline::draw() const {
  glUseProgram(shaderProgram);
  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}

GLuint RenderPipeline::createVertexShader(const char* vertexShaderSource) {
  GLuint vertexShader = 0;

  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
  glCompileShader(vertexShader);

  int success = 0;
  std::array<char, SHADER_INFO_LOG_SIZE> infoLog = {0};
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

  if (success != GL_TRUE) {
    glGetShaderInfoLog(vertexShader, SHADER_INFO_LOG_SIZE, nullptr,
                       infoLog.data());
    std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
              << infoLog.data() << '\n';
    glDeleteShader(vertexShader);
    return 0;
  }
  return vertexShader;
}

GLuint RenderPipeline::createFragmentShader(const char* fragmentShaderSource) {
  GLuint fragmentShader = 0;

  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
  glCompileShader(fragmentShader);
  int success = 0;
  std::array<char, SHADER_INFO_LOG_SIZE> infoLog = {0};
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (success != GL_TRUE) {
    glGetShaderInfoLog(fragmentShader, SHADER_INFO_LOG_SIZE, nullptr,
                       infoLog.data());
    std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
              << infoLog.data() << '\n';
    glDeleteShader(fragmentShader);
    return 0;
  }
  return fragmentShader;
}

GLuint RenderPipeline::createShaderProgram(GLuint vertexShader,
                                           GLuint fragmentShader) {
  const GLuint program = glCreateProgram();
  glAttachShader(program, vertexShader);
  glAttachShader(program, fragmentShader);
  glLinkProgram(program);

  int success = 0;
  std::array<char, SHADER_INFO_LOG_SIZE> infoLog = {0};
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (success != GL_TRUE) {
    glGetProgramInfoLog(program, SHADER_INFO_LOG_SIZE, nullptr, infoLog.data());
    std::cerr << "ERROR::SHADER_PROGRAM::LINK_FAILED\n"
              << infoLog.data() << '\n';
  }

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  return program;
}

void RenderPipeline::linkVertexAttributes() {
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
  glEnableVertexAttribArray(0);
}
