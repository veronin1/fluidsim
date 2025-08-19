#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include "WindowConfig.hpp"
#include "WindowManager.hpp"
#include "colour.hpp"
#include <array>
#include <iostream>

const char *const vertexShaderSource =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

const char *const fragmentShaderSource =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    FragColor = vec4(0.47f, 0.27f, 0.035f, 1.0f);\n"
    "}\n";

constexpr size_t SHADER_INFO_LOG_SIZE = 512;

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

RenderPipeline::RenderPipeline(const std::array<float, 9> &vertices) {
  const GLuint vertexShader = createVertexShader();
  const GLuint fragmentShader = createFragmentShader();
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
  glDrawArrays(GL_TRIANGLES, 0, 3);
}

GLuint RenderPipeline::createVertexShader() {
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
    return 0;
  }
  return vertexShader;
}

GLuint RenderPipeline::createFragmentShader() {
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

void processInput(GLFWwindow *window);

int main() {
  const WindowConfig config{1024, 768, "My Fluid Simulator"};
  WindowManager window(config);

  // triangle vertices
  const std::array<float, 9> vertices = {-0.5F, -0.5F, 0.0F, 0.5F, -0.5F,
                                         0.0F,  0.0F,  0.5F, 0.0F};

  const RenderPipeline render(vertices);

  const Colour windowColour{0.2F, 0.3F, 0.3F, 1.0F};

  // render loop
  while (!window.shouldClose()) {
    processInput(WindowManager::getGLFWwindow());

    glClearColor(windowColour.r, windowColour.g, windowColour.b,
                 windowColour.a);
    glClear(GL_COLOR_BUFFER_BIT);

    render.draw();

    window.swapBuffers();
    WindowManager::pollEvents();
  }

  glfwTerminate();
  return 0;
}

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, 1);
  }
}
