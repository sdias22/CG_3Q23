#ifndef CAMERA_HPP_
#define CAMERA_HPP_

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include <string>
#include <unordered_map>

class Camera {
public:
  void computeViewMatrix();
  void computeProjectionMatrix(glm::vec2 const &size);
  void onCreate();
  void onUpdate(int camSelect);

  glm::mat4 const &getViewMatrix() const { return m_viewMatrix; }
  glm::mat4 const &getProjMatrix() const { return m_projMatrix; }

  glm::vec2 sizeT;

private:
  struct CameraPosition {
    std::string m_name{};
    glm::vec3 m_eye; // Camera position
    glm::vec3 m_at;  // Look-at point
    glm::vec3 m_up;  // "up" direction
    float m_angle;
  };

  glm::vec3 m_eyeAt_Current;
  glm::vec3 m_at_Current;
  glm::vec3 m_up_Current;
  float m_angle_Current;

  CameraPosition m_cameras[7];

  // Matrix to change from world space to camera space
  glm::mat4 m_viewMatrix;

  // Matrix to change from camera space to clip space
  glm::mat4 m_projMatrix;
};

#endif