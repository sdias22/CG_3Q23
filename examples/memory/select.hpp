#ifndef Select_HPP_
#define Select_HPP_

#include "abcgOpenGL.hpp"
#include "vertex.hpp"
#include <random>

enum class StatusSelect { moving, select };

class Select {
public:
  void onCreate(GLuint program);
  void onPaint();
  void onUpdate();
  void onSelect(bool sel, float move);
  void onxMove(float move);
  void onzMove(float move);
  void onDestroy();

  glm::vec3 m_positionCurrent{0.0f, 0.25f, 0.0f};

private:
  void createBuffers();
  void loadObj(std::string_view path);
  void standardize();

  struct Selects {
    StatusSelect m_status;
    glm::vec4 m_color;
    glm::vec3 m_position;
  };

  Selects m_select;

  abcg::Timer m_timer;
  float m_tempo{0.05f};

  GLuint m_VAO{};
  GLuint m_VBO{};
  GLuint m_EBO{};
  std::vector<Vertex> m_vertices;
  std::vector<GLuint> m_indices;

  GLint m_viewMatrixLocation{};
  GLint m_projMatrixLocation{};
  GLint m_modelMatrixLocation{};
  GLint m_colorLocation{};

  glm::vec4 gray{0.3f, 0.3f, 0.3f, 1.0f};
  glm::vec4 white{1.0f, 1.0f, 1.0f, 1.0f};
  glm::vec4 yellow{1.0f, 0.8f, 0.0f, 1.0f};

  glm::vec4 m_colorCurrent{gray};

  glm::vec3 m_pInicial{0.0f, 0.35f, 0.0f};
};

#endif