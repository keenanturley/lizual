#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <SDL3/SDL_log.h>

#include "Camera.h"

glm::mat4 Camera::GetViewMatrix() const {
  // View Matrix = inverse(view * orientation)
  glm::mat4 view = glm::identity<glm::mat4>();
  view = glm::translate(view, position);
  glm::quat orientation = GetOrientation();
  view = view * glm::toMat4(orientation);
  view = glm::inverse(view);
  return view;
}

glm::quat Camera::GetOrientation() const {
  return glm::quat{glm::vec3(glm::radians(pitch), glm::radians(yaw), 0.0f)};
}

void Camera::Rotate(glm::vec2 deltaDegrees) {
  if (deltaDegrees == glm::zero<glm::vec2>()) { return; }

  // Compute target first
  float targetPitch = glm::mod(pitch + deltaDegrees.x, 360.0f);
  SDL_Log("target pitch = %f", targetPitch);
  // Clamp it between [0,90] or [270,360]
  // It can't be less than 0 because we just modded it, so it can only be in
  // [0, 360]. We need to clamp (90, 180] down to 90 and clamp (180, 270] to
  // 270.
  if (targetPitch > 90.0f && targetPitch <= 180.0f) {
    targetPitch = 90.0f;
  } else if (targetPitch > 180.0f && targetPitch < 270.0f) {
    targetPitch = 270.0f;
  }
  pitch = targetPitch;

  // Yaw has no constraints, so just mod it to make sure it doesn't go out of
  // control.
  yaw = glm::mod(yaw + deltaDegrees.y, 360.0f);
  SDL_Log("pitch = %f, yaw = %f", pitch, yaw);
}

void Camera::Move(glm::vec3 deltaPosition) {
  if (deltaPosition == glm::zero<glm::vec3>()) { return; }
  position += GetOrientation() * deltaPosition;
}
