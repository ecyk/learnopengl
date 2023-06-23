#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>

#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "camera.hpp"
#include "model.hpp"
#include "shader.hpp"

float delta_time{0.0F};
float last_frame{0.0F};

float mouse_last_x{400.0F};
float mouse_last_y{300.0F};

bool first_mouse_input{true};

Camera camera{{0.0F, 0.0F, 3.0F}};

void process_input(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

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

  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  if (gladLoadGL(glfwGetProcAddress) == 0) {
    std::cerr << "Failed to initialize OpenGL context\n";
    return 1;
  }

  glEnable(GL_DEPTH_TEST);

  glEnable(GL_STENCIL_TEST);
  glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
  glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);

  const Shader shader{"stencil_testing.vert", "stencil_testing.frag"};
  const Shader shader_single_color{"stencil_testing.vert", "single_color.frag"};

  const std::array cube_vertices{
      -0.5F, -0.5F, -0.5F, 0.0F, 0.0F, 0.5F,  -0.5F, -0.5F, 1.0F, 0.0F,
      0.5F,  0.5F,  -0.5F, 1.0F, 1.0F, 0.5F,  0.5F,  -0.5F, 1.0F, 1.0F,
      -0.5F, 0.5F,  -0.5F, 0.0F, 1.0F, -0.5F, -0.5F, -0.5F, 0.0F, 0.0F,

      -0.5F, -0.5F, 0.5F,  0.0F, 0.0F, 0.5F,  -0.5F, 0.5F,  1.0F, 0.0F,
      0.5F,  0.5F,  0.5F,  1.0F, 1.0F, 0.5F,  0.5F,  0.5F,  1.0F, 1.0F,
      -0.5F, 0.5F,  0.5F,  0.0F, 1.0F, -0.5F, -0.5F, 0.5F,  0.0F, 0.0F,

      -0.5F, 0.5F,  0.5F,  1.0F, 0.0F, -0.5F, 0.5F,  -0.5F, 1.0F, 1.0F,
      -0.5F, -0.5F, -0.5F, 0.0F, 1.0F, -0.5F, -0.5F, -0.5F, 0.0F, 1.0F,
      -0.5F, -0.5F, 0.5F,  0.0F, 0.0F, -0.5F, 0.5F,  0.5F,  1.0F, 0.0F,

      0.5F,  0.5F,  0.5F,  1.0F, 0.0F, 0.5F,  0.5F,  -0.5F, 1.0F, 1.0F,
      0.5F,  -0.5F, -0.5F, 0.0F, 1.0F, 0.5F,  -0.5F, -0.5F, 0.0F, 1.0F,
      0.5F,  -0.5F, 0.5F,  0.0F, 0.0F, 0.5F,  0.5F,  0.5F,  1.0F, 0.0F,

      -0.5F, -0.5F, -0.5F, 0.0F, 1.0F, 0.5F,  -0.5F, -0.5F, 1.0F, 1.0F,
      0.5F,  -0.5F, 0.5F,  1.0F, 0.0F, 0.5F,  -0.5F, 0.5F,  1.0F, 0.0F,
      -0.5F, -0.5F, 0.5F,  0.0F, 0.0F, -0.5F, -0.5F, -0.5F, 0.0F, 1.0F,

      -0.5F, 0.5F,  -0.5F, 0.0F, 1.0F, 0.5F,  0.5F,  -0.5F, 1.0F, 1.0F,
      0.5F,  0.5F,  0.5F,  1.0F, 0.0F, 0.5F,  0.5F,  0.5F,  1.0F, 0.0F,
      -0.5F, 0.5F,  0.5F,  0.0F, 0.0F, -0.5F, 0.5F,  -0.5F, 0.0F, 1.0F};

  const std::array plane_vertices{
      5.0F, -0.5F, 5.0F,  2.0F,  0.0F,  -5.0F, -0.5F, 5.0F,
      0.0F, 0.0F,  -5.0F, -0.5F, -5.0F, 0.0F,  2.0F,

      5.0F, -0.5F, 5.0F,  2.0F,  0.0F,  -5.0F, -0.5F, -5.0F,
      0.0F, 2.0F,  5.0F,  -0.5F, -5.0F, 2.0F,  2.0F};

  unsigned int cube_vao = 0;
  glGenVertexArrays(1, &cube_vao);

  unsigned int cube_vbo = 0;
  glGenBuffers(1, &cube_vbo);

  glBindVertexArray(cube_vao);
  glBindBuffer(GL_ARRAY_BUFFER, cube_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices.data(),
               GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (const void*)0);

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (const void*)(3 * sizeof(float)));
  glBindVertexArray(0);

  unsigned int plane_vao = 0;
  glGenVertexArrays(1, &plane_vao);

  unsigned int plane_vbo = 0;
  glGenBuffers(1, &plane_vbo);

  glBindVertexArray(plane_vao);
  glBindBuffer(GL_ARRAY_BUFFER, plane_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(plane_vertices), plane_vertices.data(),
               GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (const void*)0);

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (const void*)(3 * sizeof(float)));
  glBindVertexArray(0);

  const Texture cube_texture{"resources/textures/marble.jpg"};
  const Texture plane_texture{"resources/textures/metal.png"};

  shader.use();
  shader.set_int("texture1", 0);

  while (glfwWindowShouldClose(window) != 1) {
    const auto current_frame = static_cast<float>(glfwGetTime());
    delta_time = current_frame - last_frame;
    last_frame = current_frame;

    process_input(window);

    glClearColor(0.25F, 0.25F, 0.25F, 1.0F);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glm::mat4 model{glm::scale(
        glm::translate(glm::identity<glm::mat4>(), {0.0F, 0.0F, 0.0F}),
        {1.0F, 1.0F, 1.0F})};
    const glm::mat4 view{camera.calculate_view_matrix()};
    const glm::mat4 projection{glm::perspective(glm::radians(camera.zoom_),
                                                800.0F / 600.0F, 0.1F, 100.0F)};

    shader_single_color.use();
    shader_single_color.set_mat4("projection", projection);
    shader_single_color.set_mat4("view", view);

    shader.use();
    shader.set_mat4("view", view);
    shader.set_mat4("projection", projection);

    glStencilMask(0x00);
    glBindVertexArray(plane_vao);
    plane_texture.bind();
    shader.set_mat4("model", glm::mat4{1.0F});
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);
    glBindVertexArray(cube_vao);
    glActiveTexture(GL_TEXTURE0);
    cube_texture.bind();
    model = glm::translate(model, glm::vec3{-1.0F, 0.0F, -1.0F});
    shader.set_mat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    model = glm::mat4{1.0F};
    model = glm::translate(model, glm::vec3{2.0F, 0.0F, 0.0F});
    shader.set_mat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00);
    glDisable(GL_DEPTH_TEST);
    shader_single_color.use();
    const float scale = 1.1F;
    glBindVertexArray(cube_vao);
    cube_texture.bind();
    model = glm::mat4{1.0F};
    model = glm::translate(model, glm::vec3{-1.0F, 0.0F, -1.0F});
    model = glm::scale(model, glm::vec3{scale, scale, scale});
    shader_single_color.set_mat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    model = glm::mat4{1.0F};
    model = glm::translate(model, glm::vec3{2.0F, 0.0F, 0.0F});
    model = glm::scale(model, glm::vec3{scale, scale, scale});
    shader_single_color.set_mat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glStencilMask(0xFF);
    glStencilFunc(GL_ALWAYS, 0, 0xFF);
    glEnable(GL_DEPTH_TEST);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &cube_vao);
  glDeleteVertexArrays(1, &plane_vao);
  glDeleteBuffers(1, &cube_vbo);
  glDeleteBuffers(1, &plane_vbo);

  glfwTerminate();
  return 0;
}

void process_input(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, 1);
  }

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    camera.process_keyboard_input(CameraMovement::Forward, delta_time);
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    camera.process_keyboard_input(CameraMovement::Backward, delta_time);
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    camera.process_keyboard_input(CameraMovement::Left, delta_time);
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    camera.process_keyboard_input(CameraMovement::Right, delta_time);
  }
}

void mouse_callback(GLFWwindow*, double xpos, double ypos) {
  if (first_mouse_input) {
    mouse_last_x = static_cast<float>(xpos);
    mouse_last_y = static_cast<float>(ypos);
    first_mouse_input = false;
  }

  const float offset_x = static_cast<float>(xpos) - mouse_last_x;
  const float offset_y = mouse_last_y - static_cast<float>(ypos);

  mouse_last_x = static_cast<float>(xpos);
  mouse_last_y = static_cast<float>(ypos);

  camera.process_mouse_movement(offset_x, offset_y);
}

void scroll_callback(GLFWwindow*, double, double yoffset) {
  camera.process_mouse_scroll(static_cast<float>(yoffset));
}
