#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
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

  Shader shader{"transformations.vert", "transformations.frag"};

  const std::array vertices{
      // positions        // texture coords
      0.5F,  0.5F,  0.0F, 1.0F, 1.0F,  // top right
      0.5F,  -0.5F, 0.0F, 1.0F, 0.0F,  // bottom right
      -0.5F, -0.5F, 0.0F, 0.0F, 0.0F,  // bottom left
      -0.5F, 0.5F,  0.0F, 0.0F, 1.0F   // top left
  };

  const std::array indices{
      0U, 1U, 3U,  // first triangle
      1U, 2U, 3U   // second triangle
  };

  unsigned int vao = 0;
  glGenVertexArrays(1, &vao);

  unsigned int vbo = 0;
  glGenBuffers(1, &vbo);

  unsigned int ebo = 0;
  glGenBuffers(1, &ebo);

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(),
               GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(),
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (const void*)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (const void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  unsigned int texture1 = 0;
  glGenTextures(1, &texture1);
  glBindTexture(GL_TEXTURE_2D, texture1);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  int width = 0;
  int height = 0;
  int nr_channels = 0;

  stbi_set_flip_vertically_on_load(1);

  unsigned char* data = stbi_load("resources/textures/container.jpg", &width,
                                  &height, &nr_channels, 0);
  if (data != nullptr) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cerr << "Failed to load texture\n";
  }

  stbi_image_free(data);

  unsigned int texture2 = 0;
  glGenTextures(1, &texture2);
  glBindTexture(GL_TEXTURE_2D, texture2);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  data = stbi_load("resources/textures/awesomeface.png", &width, &height,
                   &nr_channels, 0);
  if (data != nullptr) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cout << "Failed to load texture\n";
  }

  stbi_image_free(data);

  shader.use();
  shader.set_int("texture1", 0);
  shader.set_int("texture2", 1);

  while (glfwWindowShouldClose(window) != 1) {
    process_input(window);

    glClearColor(0.25F, 0.25F, 0.25F, 1.0F);
    glClear(GL_COLOR_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);

    shader.use();
    glUniformMatrix4fv(
        glGetUniformLocation(shader.get_id(), "transform"), 1, GL_FALSE,
        glm::value_ptr(
            glm::translate(glm::identity<glm::mat4>(), {0.5F, -0.5F, 0.0F}) *
            glm::rotate(glm::identity<glm::mat4>(),
                        static_cast<float>(glfwGetTime()),
                        {0.0F, 0.0F, 1.0F})));

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
  glDeleteBuffers(1, &ebo);
  glDeleteTextures(1, &texture1);
  glDeleteTextures(1, &texture2);

  glfwTerminate();
  return 0;
}

void process_input(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, 1);
  }
}
