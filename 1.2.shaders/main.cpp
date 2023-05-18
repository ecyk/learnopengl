#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>

#include <array>
#include <iostream>

#include "shader.hpp"

void process_input(GLFWwindow* window);

int main() {
  glfwSetErrorCallback(
      [](int, const char* description) { std::cerr << description << '\n'; });

  if (glfwInit() != 1) {
    return 1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  GLFWwindow* window =
      glfwCreateWindow(800, 600, "learnopengl", nullptr, nullptr);
  if (window == nullptr) {
    glfwTerminate();
    return 1;
  }

  glfwMakeContextCurrent(window);

  glfwSetFramebufferSizeCallback(window,
                                 [](GLFWwindow*, int width, int height) {
                                   glViewport(0, 0, width, height);
                                 });

  if (gladLoadGL(glfwGetProcAddress) == 0) {
    std::cerr << "Failed to initialize OpenGL context\n";
    return 1;
  }

  Shader shader{"shaders.vert", "shaders.frag"};

  const std::array vertices{
      // positions         // colors
      0.5F,  -0.5F, 0.0F, 1.0F, 0.0F, 0.0F,  // bottom right
      -0.5F, -0.5F, 0.0F, 0.0F, 1.0F, 0.0F,  // bottom left
      0.0F,  0.5F,  0.0F, 0.0F, 0.0F, 1.0F   // top
  };

  unsigned int vao = 0;
  glGenVertexArrays(1, &vao);

  unsigned int vbo = 0;
  glGenBuffers(1, &vbo);

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(),
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (const void*)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (const void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  // wireframe mode
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  while (glfwWindowShouldClose(window) != 1) {
    process_input(window);

    glClearColor(0.25F, 0.25F, 0.25F, 1.0F);
    glClear(GL_COLOR_BUFFER_BIT);

    shader.use();
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);

  glfwTerminate();
  return 0;
}

void process_input(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, 1);
  }
}
