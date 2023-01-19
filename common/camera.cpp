#include "camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera() { update_vectors(); }

Camera::Camera(const glm::vec3& position) : position_{position} {
  update_vectors();
}

Camera::Camera(const glm::vec3& position, const glm::vec3& up, float yaw,
               float pitch)
    : position_{position}, up_{up}, yaw_{yaw}, pitch_{pitch} {
  update_vectors();
}

glm::mat4 Camera::calculate_view_matrix() const {
  return glm::lookAt(position_, position_ + front_, up_);
}

void Camera::process_keyboard_input(CameraMovement direction,
                                    float delta_time) {
  const float velocity = movement_speed_ * delta_time;
  if (direction == CameraMovement::Forward) {
    position_ += front_ * velocity;
  }
  if (direction == CameraMovement::Backward) {
    position_ -= front_ * velocity;
  }
  if (direction == CameraMovement::Left) {
    position_ -= right_ * velocity;
  }
  if (direction == CameraMovement::Right) {
    position_ += right_ * velocity;
  }
}

void Camera::process_mouse_movement(float offset_x, float offset_y,
                                    bool constrain_pitch) {
  offset_x *= mouse_sensitivity_;
  offset_y *= mouse_sensitivity_;

  yaw_ += offset_x;
  pitch_ += offset_y;

  if (constrain_pitch) {
    if (pitch_ > 89.0F) {
      pitch_ = 89.0F;
    }
    if (pitch_ < -89.0F) {
      pitch_ = -89.0F;
    }
  }

  update_vectors();
}

void Camera::process_mouse_scroll(float offset_y) {
  zoom_ -= offset_y;
  if (zoom_ < 1.0F) {
    zoom_ = 1.0F;
  }
  if (zoom_ > 45.0F) {
    zoom_ = 45.0F;
  }
}

void Camera::update_vectors() {
  front_ = glm::normalize(
      glm::vec3{cos(glm::radians(yaw_)) * cos(glm::radians(pitch_)),
                sin(glm::radians(pitch_)),
                sin(glm::radians(yaw_)) * cos(glm::radians(pitch_))});

  right_ = glm::normalize(glm::cross(front_, world_up_));
  up_ = glm::normalize(glm::cross(right_, front_));
}
