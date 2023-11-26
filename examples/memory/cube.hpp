#ifndef CUBE_HPP_
#define CUBE_HPP_

#include "abcgOpenGL.hpp"
#include "vertex.hpp"
#include <random>

enum class StatusCube { on, off, select };

class Cube {
public:
  void onCreate(GLuint program);
  void onPaint();
  void onUpdate();
  void onState();
  void onDestroy();
  bool onGameOver();
  bool onSelect(int pos);
  void onDecision(int pos1, int pos2);

private:
  void createBuffers();
  void loadObj(std::string_view path);
  void standardize();
  void computeNormals();

  // Estrutura para armazenar as informações sobre cada cubo
  struct Cubes {
    StatusCube m_status;
    glm::vec4 m_color;
    glm::vec3 m_position;
    glm::vec3 m_scale{0.5f};
  };

  abcg::Timer m_timer;
  float m_tempo{0.05f};

  int n = 64;
  Cubes m_cubes[64];

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

  // Armazena as cores utilizadas nos cubos
  glm::vec4 gray{0.3f, 0.3f, 0.3f, 1.0f};
  glm::vec4 pink{1.0f, 0.0f, 1.0f, 1.0f};
  glm::vec4 yellow{1.0f, 0.8f, 0.0f, 1.0f};
  glm::vec4 red{1.0f, 0.25f, 0.25f, 1.0f};
  glm::vec4 blue{0.3608f, 0.8314f, 1.0f, 1.0f};
  glm::vec4 green{0.2f, 0.8f, 0.2f, 1.0f};
  glm::vec4 orange{1.0f, 0.6902f, 0.30196f, 1.0f};
  glm::vec4 purple{0.4f, 0.0f, 0.4f, 1.0f};
  glm::vec4 brown{0.6314f, 0.3608f, 0.2784f, 1.0f};
  glm::vec4 black{0.0f, 0.0f, 0.0f, 1.0f};
  glm::vec4 white{1.0f, 1.0f, 1.0f, 1.0f};

  glm::vec4 m_ColorList[64];

  glm::vec4 m_colorCurrent{gray};

  bool m_colorEmpty{false};

  std::default_random_engine m_randomEngine;

  glm::vec3 m_pInicial{-0.8f, 0.075f, -0.8f};
};

#endif