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
  void onSelect(bool sel);
  void onxMove(float move);
  void onzMove(float move);
  void onDestroy();

  // Armazena a posição atual da forma utilizada para seleção
  glm::vec3 m_positionCurrent{0.0f, 0.25f, 0.0f};

private:
  void createBuffers();
  void loadObj(std::string_view path);
  void standardize();
  void computeNormals();

  /*
  Estrutura utilziada para armazenar o status, cor e posição da forma de
  seleção
  */
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

  bool m_hasNormals{false};

  GLint m_viewMatrixLocation{};
  GLint m_projMatrixLocation{};
  GLint m_modelMatrixLocation{};
  GLint m_colorLocation{};

  GLint m_normalMatrixLocation{};

  glm::mat4 m_modelMatrix{1.0f};
  glm::mat4 m_viewMatrix{1.0f};

  GLint m_KaLocation;
  GLint m_KdLocation;
  GLint m_KsLocation;

  glm::vec4 m_Ka{0.1f, 0.1f, 0.1f, 1.0f};
  glm::vec4 m_Kd{0.7f, 0.7f, 0.7f, 1.0f};
  glm::vec4 m_Ks{1.0f};

  // Cores utilizadas na forma
  glm::vec4 gray{0.3f, 0.3f, 0.3f, 1.0f};
  glm::vec4 white{1.0f, 1.0f, 1.0f, 1.0f};
  glm::vec4 yellow{1.0f, 0.8f, 0.0f, 1.0f};

  glm::vec4 m_colorCurrent{gray};

  // Armazena a posição
  glm::vec3 m_pInicial{0.0f, 0.30f, 0.0f};
};

#endif