#ifndef CUBE_HPP_
#define CUBE_HPP_

#include "abcgOpenGL.hpp"
#include "vertex.hpp"
#include <random>

enum class StatusCube { on, off, select };

class Cube {
public:
  void onCreate();
  void onPaint(glm::mat4 m_ViewMatrix, glm::mat4 m_ProjMatrix);
  void onUpdate();
  void onState();
  void onDestroy();
  bool onGameOver();
  bool onSelect(int pos);
  void onDecision(int pos1, int pos2);

private:
  void onSetup();
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

  Cubes m_cubes[64];

  GLuint m_VAO{};
  GLuint m_VBO{};
  GLuint m_EBO{};

  bool m_hasNormals{false};
  std::vector<Vertex> m_vertices;
  std::vector<GLuint> m_indices;

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

  glm::vec4 m_Ka{0.5f, 0.5f, 0.5f, 1.0f};
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

  // Cores utilizadas nos cubos
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

  abcg::Timer m_timer;
  float m_tempo{0.05f};

  std::default_random_engine m_randomEngine;

  glm::vec3 m_pInicial{-0.8f, 0.075f, -0.8f};
};

#endif