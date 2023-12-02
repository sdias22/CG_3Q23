#ifndef GROUND_HPP_
#define GROUND_HPP_

#include "abcgOpenGL.hpp"
#include "vertex.hpp"

class Ground {
public:
  void onCreate();
  void onPaint(glm::mat4 m_ViewMatrix, glm::mat4 m_ProjMatrix);
  void onDestroy();

private:
  void onSetup();
  GLuint m_VAO{};
  GLuint m_VBO{};

  // Shaders
  GLuint m_program{};
  GLint m_viewMatrixLocation{};
  GLint m_projMatrixLocation{};
  GLint m_modelMatrixLocation{};
  GLint m_normalMatrixLocation{};
  GLint m_lightPositionLocation{};
  GLint m_colorLocation{};

  // Material properties
  GLint m_KaLocation;
  GLint m_KdLocation;
  GLint m_KsLocation;

  glm::vec4 m_Ka{0.5f, 0.5f, 0.5f, 1.0f};
  glm::vec4 m_Kd{1.0f, 1.0f, 1.0f, 1.0f};
  glm::vec4 m_Ks{1.0f, 1.0f, 1.0f, 1.0f};

  // Light properties
  GLint m_IaLocation;
  GLint m_IdLocation;
  GLint m_IsLocation;

  glm::vec4 m_Ia{1.0f};
  glm::vec4 m_Id{1.0f};
  glm::vec4 m_Is{1.0f};

  glm::vec3 m_light{-0.05f, 1.0f, 0.0f};

  glm::vec4 white{1.0f, 1.0f, 1.0f, 1.0f};
  glm::vec4 darkGray{0.1f, 0.1f, 0.1f, 1.0f};
};

#endif