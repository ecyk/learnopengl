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

#include "camera.hpp"
#include "shader.hpp"

float delta_time{0.0F};
float last_frame{0.0F};

float mouse_last_x{400.0F};
float mouse_last_y{300.0F};

bool first_mouse_input{true};

Camera camera{{0.0F, 0.0F, 3.0F}};

glm::vec3 light_pos{1.2F, 1.0F, 2.0F};

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

  Shader shader{"colors.vert", "colors.frag"};
  Shader light_cube_shader{"light_cube.vert", "light_cube.frag"};

  const std::array vertices{
      -0.5F, -0.5F, -0.5F, 0.5F,  -0.5F, -0.5F, 0.5F,  0.5F,  -0.5F,
      0.5F,  0.5F,  -0.5F, -0.5F, 0.5F,  -0.5F, -0.5F, -0.5F, -0.5F,

      -0.5F, -0.5F, 0.5F,  0.5F,  -0.5F, 0.5F,  0.5F,  0.5F,  0.5F,
      0.5F,  0.5F,  0.5F,  -0.5F, 0.5F,  0.5F,  -0.5F, -0.5F, 0.5F,

      -0.5F, 0.5F,  0.5F,  -0.5F, 0.5F,  -0.5F, -0.5F, -0.5F, -0.5F,
      -0.5F, -0.5F, -0.5F, -0.5F, -0.5F, 0.5F,  -0.5F, 0.5F,  0.5F,

      0.5F,  0.5F,  0.5F,  0.5F,  0.5F,  -0.5F, 0.5F,  -0.5F, -0.5F,
      0.5F,  -0.5F, -0.5F, 0.5F,  -0.5F, 0.5F,  0.5F,  0.5F,  0.5F,

      -0.5F, -0.5F, -0.5F, 0.5F,  -0.5F, -0.5F, 0.5F,  -0.5F, 0.5F,
      0.5F,  -0.5F, 0.5F,  -0.5F, -0.5F, 0.5F,  -0.5F, -0.5F, -0.5F,

      -0.5F, 0.5F,  -0.5F, 0.5F,  0.5F,  -0.5F, 0.5F,  0.5F,  0.5F,
      0.5F,  0.5F,  0.5F,  -0.5F, 0.5F,  0.5F,  -0.5F, 0.5F,  -0.5F};

  unsigned int cube_vao = 0;
  glGenVertexArrays(1, &cube_vao);

  unsigned int vbo = 0;
  glGenBuffers(1, &vbo);

  glBindVertexArray(cube_vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(),
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                        (const void*)0);
  glEnableVertexAttribArray(0);

  unsigned int light_cube_vao = 0;
  glGenVertexArrays(1, &light_cube_vao);

  glBindVertexArray(light_cube_vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                        (const void*)0);
  glEnableVertexAttribArray(0);

  while (glfwWindowShouldClose(window) != 1) {
    const auto current_frame = static_cast<float>(glfwGetTime());
    delta_time = current_frame - last_frame;
    last_frame = current_frame;

    process_input(window);

    glClearColor(0.25F, 0.25F, 0.25F, 1.0F);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.use();
    shader.set_vec3("objectColor", {1.0F, 0.5F, 0.31F});
    shader.set_vec3("lightColor", {1.0F, 1.0F, 1.0F});

    const glm::mat4 view = camera.calculate_view_matrix();
    const glm::mat4 projection = glm::perspective(
        glm::radians(camera.zoom_), 800.0F / 600.0F, 0.1F, 100.0F);

    shader.set_mat4("model", glm::identity<glm::mat4>());
    shader.set_mat4("view", view);
    shader.set_mat4("projection", projection);

    glBindVertexArray(cube_vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    light_cube_shader.use();
    light_cube_shader.set_mat4(
        "model", glm::translate(glm::identity<glm::mat4>(), light_pos) *
                     glm::scale(glm::identity<glm::mat4>(),
                                glm::vec3{0.2F, 0.2F, 0.2F}));
    light_cube_shader.set_mat4("view", view);
    light_cube_shader.set_mat4("projection", projection);

    glBindVertexArray(light_cube_vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &cube_vao);
  glDeleteVertexArrays(1, &light_cube_vao);
  glDeleteBuffers(1, &vbo);
  glDeleteProgram(shader.id_);
  shader.id_ = 0;

  glfwTerminate();
  return 0;
}

void process_input(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, 1);
  }

  const float camera_speed = 0.05F;
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
