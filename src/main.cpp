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

enum class OpenGLVersion : uint8_t { VERSION_MAJOR = 4, VERSION_MINOR = 6 };

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

void processInput(GLFWwindow *window);

struct Colour {
  float r, g, b, a;

  Colour(float red, float green, float blue, float alpha)
      : r(red), g(green), b(blue), a(alpha) {}
};

class WindowManager {
 public:
  WindowManager(const WindowManager &) = default;
  WindowManager(WindowManager &&) = delete;
  WindowManager &operator=(const WindowManager &) = default;
  WindowManager &operator=(WindowManager &&) = delete;
  explicit WindowManager(const WindowConfig &config)
      : width(config.width), height(config.height), title(config.title) {
    if (glfwInit() == 0) {
      throw std::runtime_error("Failed to initialize GLFW\n");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,
                   static_cast<int>(OpenGLVersion::VERSION_MAJOR));
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,
                   static_cast<int>(OpenGLVersion::VERSION_MINOR));
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (window == nullptr) {
      glfwTerminate();
      throw std::runtime_error("Failed to create GLFW window\n");
    }

    glfwMakeContextCurrent(window);
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    if (gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)) ==
        0) {
      glfwTerminate();
      throw std::runtime_error("Failed to initialise GLAD\n");
    }
    glViewport(0, 0, config.width, config.height);

    glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);
  }

  ~WindowManager() {
    if (window != nullptr) {
      glfwDestroyWindow(window);
    }
    glfwTerminate();
  }

  [[nodiscard]] bool shouldClose() const {
    return glfwWindowShouldClose(window) != 0;
  }
  void swapBuffers() { glfwSwapBuffers(window); }
  static void pollEvents() { glfwPollEvents(); }

  static GLFWwindow *getGLFWwindow() { return glfwGetCurrentContext(); }

 private:
  int width, height;
  const char *title = nullptr;
  GLFWwindow *window = nullptr;

  static void frameBufferSizeCallback(GLFWwindow *window, int width,
                                      int height) {
    (void)window;
    glViewport(0, 0, width, height);
  }
};

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
