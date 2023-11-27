#ifndef BACKGROUND_HPP_
#define BACKGROUND_HPP_

#include "abcgOpenGL.hpp"
#include "vertex.hpp"

class Background {
public:
  void onCreate();
  void onPaint(glm::mat4 m_ViewMatrix, glm::mat4 m_ProjMatrix);
  void onDestroy();

private:
  void loadTexture(std::string const &path);
  GLuint m_VAO{};
  GLuint m_VBO{};
  GLuint m_Texture{};

  // Shaders
  GLuint m_program{};
  GLint m_viewMatrixLocation{};
  GLint m_projMatrixLocation{};
  GLint m_modelMatrixLocation{};
  GLint m_colorLocation{};

  std::array<glm::vec3, 36> const m_Positions{{// Front
                                               {-2, -2, +2},
                                               {+2, -2, +2},
                                               {+2, +2, +2},
                                               {-2, -2, +2},
                                               {+2, +2, +2},
                                               {-2, +2, +2},
                                               // Back
                                               {+2, -2, -2},
                                               {-2, -2, -2},
                                               {-2, +2, -2},
                                               {+2, -2, -2},
                                               {-2, +2, -2},
                                               {+2, +2, -2},
                                               // Right
                                               {+2, -2, -2},
                                               {+2, +2, -2},
                                               {+2, +2, +2},
                                               {+2, -2, -2},
                                               {+2, +2, +2},
                                               {+2, -2, +2},
                                               // Left
                                               {-2, -2, +2},
                                               {-2, +2, +2},
                                               {-2, +2, -2},
                                               {-2, -2, +2},
                                               {-2, +2, -2},
                                               {-2, -2, -2},
                                               // Top
                                               {-2, +2, +2},
                                               {+2, +2, +2},
                                               {+2, +2, -2},
                                               {-2, +2, +2},
                                               {+2, +2, -2},
                                               {-2, +2, -2},
                                               // Bottom
                                               {-2, -2, -2},
                                               {+2, -2, -2},
                                               {+2, -2, +2},
                                               {-2, -2, -2},
                                               {+2, -2, +2},
                                               {-2, -2, +2}}};
};

#endif