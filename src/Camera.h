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

  // Update pitch and yaw by a given delta
  void Rotate(glm::vec2 deltaDegrees);

  glm::vec3 position;
  float pitch;
  float yaw;
};
