#include <glm/gtx/quaternion.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

// First-person Camera
class Camera {
 public:
  Camera(
    glm::vec3 position = glm::zero<glm::vec3>(),
    float pitch = 0.0f,
    float yaw = 0.0f
  )
      : position(position), pitch(pitch), yaw(yaw) {};
  glm::mat4 GetViewMatrix() const;
  glm::quat GetOrientation() const;

  // Update pitch and yaw by a given delta
  void Rotate(glm::vec2 deltaDegrees);
  // Move relative to the camera's current orientation
  void Move(glm::vec3 deltaPosition);

  glm::vec3 position;
  float pitch;
  float yaw;
};
