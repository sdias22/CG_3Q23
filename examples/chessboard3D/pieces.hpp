#ifndef PIECES_HPP_
#define PIECES_HPP_

#include "abcgOpenGL.hpp"
#include "vertex.hpp"

enum class Input { rei, rainha, bispo, cavalo, torre, peao };

struct Piece {
  Input m_input{Input::rei};
};

class Pieces {
public:
  void onCreate(GLuint program, Input input);
  void onPaint(glm::vec3 posGround, glm::vec4 colorPiece, Input input);
  void onUpdate();
  void onDestroy();
  void horizontalmove();
  void verticalmove();
  void loadObj(std::string_view path);

private:
  void createBuffers();
  void standardize();

  GLuint m_VAO{};
  GLuint m_VBO{};
  GLuint m_EBO{};

  GLint m_viewMatrixLocation{};
  GLint m_projMatrixLocation{};
  GLint m_modelMatrixLocation{};
  GLint m_colorLocation{};

  std::vector<Vertex> m_vertices;
  std::vector<GLuint> m_indices;

  glm::vec4 white{0.9f, 0.9f, 0.9f, 1.0f};
  glm::vec4 gray{0.3f, 0.3f, 0.3f, 0.3f};
};

#endif