#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <array>
#include <cstdint>
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
    "    FragColor = vec4(1.0f, 0.5f, 1.0f, 1.0f);\n"
    "}\n";

constexpr size_t SHADER_INFO_LOG_SIZE = 512;

void processInput(GLFWwindow *window);

enum class Version : uint8_t {
  OPEN_GL_VERSION_MAJOR = 4,
  OPEN_GL_VERSION_MINOR = 6
};

class WindowConfig {
 public:
  static constexpr int DEFAULT_WIDTH = 800;
  static constexpr int DEFAULT_HEIGHT = 600;
  static constexpr const char *DEFAULT_TITLE = "fluidsim";

  int width;
  int height;
  const char *title;

  WindowConfig()
      : width(DEFAULT_WIDTH), height(DEFAULT_HEIGHT), title(DEFAULT_TITLE) {}

  WindowConfig(int winWidth, int winHeight, const char *winTitle)
      : width(winWidth), height(winHeight), title(winTitle) {}
};

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  (void)window;
  glViewport(0, 0, width, height);
}

int main() {
  // initialise and configure
  if (glfwInit() == 0) {
    std::cerr << "Failed to initialise GLFW" << '\n';
    return -1;
  };
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,
                 static_cast<int>(Version::OPEN_GL_VERSION_MAJOR));
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,
                 static_cast<int>(Version::OPEN_GL_VERSION_MINOR));
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // create window with config(default)
  const WindowConfig config;

  GLFWwindow *window = glfwCreateWindow(config.width, config.height,
                                        config.title, nullptr, nullptr);
  if (window == nullptr) {
    std::cerr << "Failed to create GLFW window" << '\n';
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  // load opengl function pointer (GLAD)
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  if (gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)) ==
      0) {
    std::cerr << "Failed to initialise GLAD" << '\n';
    glfwTerminate();
    return -1;
  }

  glViewport(0, 0, config.width, config.height);

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  // render loop
  while (glfwWindowShouldClose(window) == 0) {
    processInput(window);

    glClearColor(0.2F, 0.3F, 0.3F, 1.0F);
    glClear(GL_COLOR_BUFFER_BIT);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, 1);
  }
}

const std::array<float, 9> vertices = {-0.5F, -0.5F, 0.0F, 0.5F, -0.5F,
                                       0.0F,  0.0F,  0.5F, 0.0F};

void vertex_buffer_object() {
  unsigned int VBO = 0;

  glGenBuffers(1, &VBO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(),
               GL_STATIC_DRAW);
}

GLuint createVertexShader() {
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

GLuint createFragmentShader() {
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

GLuint createShaderProgram(GLuint vertexShader, GLuint fragmentShader) {
  GLuint shaderProgram = 0;
  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  int success = 0;
  std::array<char, SHADER_INFO_LOG_SIZE> infoLog = {0};
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (success != GL_TRUE) {
    glGetProgramInfoLog(shaderProgram, SHADER_INFO_LOG_SIZE, nullptr,
                        infoLog.data());
    std::cerr << "ERROR::SHADER::PROGRAM::LINK_FAILED\n"
              << infoLog.data() << '\n';
    return 0;
  }

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  return shaderProgram;
}

void linkVertexAttributes() {
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
  glEnableVertexAttribArray(0);
}

void virtualArrayObjeect() {
  unsigned int VAO = 0;

  glGenVertexArrays(1, &VAO);
}

void draw(GLuint shaderProgram, unsigned int VAO) {
  glUseProgram(shaderProgram);
  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLES, 0, 3);
}
