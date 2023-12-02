#ifndef Select_HPP_
#define Select_HPP_

#include "abcgOpenGL.hpp"
#include "vertex.hpp"
#include <random>

enum class StatusSelect { moving, select };

class Select {
public:
  void onCreate();
  void onSetup();
  void onPaint(glm::mat4 m_ViewMatrix, glm::mat4 m_ProjMatrix);
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

  GLuint m_VAO{};
  GLuint m_VBO{};
  GLuint m_EBO{};

  bool m_hasNormals{false};
  std::vector<Vertex> m_vertices;
  std::vector<GLuint> m_indices;

  glm::mat4 m_viewMatrix{1.0f};

  // Shaders
  GLuint m_program{};
  GLint m_viewMatrixLocation{};
  GLint m_projMatrixLocation{};
  GLint m_modelMatrixLocation{};
  GLint m_normalMatrixLocation{};
  GLint m_lightPositionLocation{};
  GLint m_colorMatrixLocation{};

  // Material properties
  GLint m_KaLocation;
  GLint m_KdLocation;
  GLint m_KsLocation;

  glm::vec4 m_Ka{0.75f, 0.75f, 0.75f, 1.0f};
  glm::vec4 m_Kd{0.0f, 0.0f, 0.0f, 1.0f};
  glm::vec4 m_Ks{0.5f, 0.5f, 0.5f, 1.0f};

  // Light properties
  GLint m_IaLocation;
  GLint m_IdLocation;
  GLint m_IsLocation;

  glm::vec4 m_Ia{1.0f};
  glm::vec4 m_Id{1.0f};
  glm::vec4 m_Is{1.0f};

  glm::vec3 m_light{-0.05f, 1.0f, 0.0f};

  // Cores utilizadas no seletor
  glm::vec4 gray{0.3f, 0.3f, 0.3f, 1.0f};
  glm::vec4 white{1.0f, 1.0f, 1.0f, 1.0f};
  glm::vec4 yellow{1.0f, 0.8f, 0.0f, 1.0f};

  glm::vec4 m_colorCurrent{gray};

  abcg::Timer m_timer;
  float m_tempo{0.05f};

  // Armazena a posição
  glm::vec3 m_pInicial{0.0f, 0.30f, 0.0f};
};

#endif