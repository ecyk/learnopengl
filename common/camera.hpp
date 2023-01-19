#pragma once

#include <glm/glm.hpp>

enum class CameraMovement { Forward, Backward, Left, Right };

class Camera {
 public:
  Camera();
  Camera(const glm::vec3& position);
  Camera(const glm::vec3& position, const glm::vec3& up, float yaw,
         float pitch);

  glm::mat4 calculate_view_matrix() const;
  void process_keyboard_input(CameraMovement direction, float delta_time);
  void process_mouse_movement(float offset_x, float offset_y,
                              bool constrain_pitch = true);
  void process_mouse_scroll(float offset_y);

 private:
  void update_vectors();

 public:
  glm::vec3 position_{};
  glm::vec3 front_{0.0f, 0.0f, -1.0f};
  glm::vec3 up_{0.0f, 1.0f, 0.0f};
  glm::vec3 right_{};
  glm::vec3 world_up_{up_};

  float yaw_{-90.0F};
  float pitch_{};

  float movement_speed_{2.5F};
  float mouse_sensitivity_{0.1F};
  float zoom_{45.0F};
};
