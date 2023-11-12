#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include "abcgOpenGL.hpp"
#include "camera.hpp"
#include "ground.hpp"
#include "pieces.hpp"

class Window : public abcg::OpenGLWindow {
protected:
  void onEvent(SDL_Event const &event) override;
  void onCreate() override;
  void onUpdate() override;
  void onPaint() override;
  void onResize(glm::ivec2 const &size) override;
  void onDestroy() override;

private:
  Ground m_ground;

  glm::ivec2 m_viewportSize{};

  GLuint m_program{};

  GLint m_viewMatrixLocation{};
  GLint m_projMatrixLocation{};
  GLint m_modelMatrixLocation{};
  GLint m_colorLocation{};

  Camera m_camera;
  float m_dollySpeed{};
  float m_truckSpeed{};
  float m_panSpeed{};
  float m_upSpeed{};
  float m_rotateSpeed{};

  Pieces m_reiW;
  Pieces m_rainhaW;
  Pieces m_bispoWL;
  Pieces m_bispoWR;
  Pieces m_cavaloWL;
  Pieces m_cavaloWR;
  Pieces m_torreWL;
  Pieces m_torreWR;
  Pieces m_peaoW1;
  Pieces m_peaoW2;
  Pieces m_peaoW3;
  Pieces m_peaoW4;
  Pieces m_peaoW5;
  Pieces m_peaoW6;
  Pieces m_peaoW7;
  Pieces m_peaoW8;

  Pieces m_reiB;
  Pieces m_rainhaB;
  Pieces m_bispoBL;
  Pieces m_bispoBR;
  Pieces m_cavaloBL;
  Pieces m_cavaloBR;
  Pieces m_torreBL;
  Pieces m_torreBR;
  Pieces m_peaoB1;
  Pieces m_peaoB2;
  Pieces m_peaoB3;
  Pieces m_peaoB4;
  Pieces m_peaoB5;
  Pieces m_peaoB6;
  Pieces m_peaoB7;
  Pieces m_peaoB8;

  /*
  float m_hMove{};
  float m_vMove{};
  float m_sMove{};
  */

  glm::vec4 white{0.9f, 0.9f, 0.9f, 1.0f};
  glm::vec4 gray{0.3f, 0.3f, 0.3f, 0.3f};

  std::vector<Vertex> m_vertices;
};

#endif