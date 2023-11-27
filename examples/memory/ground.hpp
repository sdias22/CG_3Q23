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
  GLint m_colorLocation{};
};

#endif