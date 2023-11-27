#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include "abcgOpenGL.hpp"
#include "background.hpp"
#include "camera.hpp"
#include "cube.hpp"
#include "ground.hpp"
#include "select.hpp"

enum class GameStatus { start, playing, gameover };

enum class PlayStatus { moving, select, decision };

class Window : public abcg::OpenGLWindow {
protected:
  void onEvent(SDL_Event const &event) override;
  void onCreate() override;
  void onUpdate() override;
  void onPaint() override;
  void onPaintUI() override;
  void onResize(glm::ivec2 const &size) override;
  void onDestroy() override;

private:
  void onGame();
  void onRestart();

  Ground m_ground;
  Background m_background;
  Cube m_cube;
  Select m_select;
  PlayStatus m_play{PlayStatus::moving};
  GameStatus m_game{GameStatus::playing};

  glm::ivec2 m_viewportSize{};

  Camera m_camera;
  int m_camSelect = 0;

  std::vector<Vertex> m_vertices;

  bool m_restart{false};

  abcg::Timer m_timer;
  abcg::Timer m_timerRestart;
  float m_tempo{0.500};

  int tries{0};

  int posSelect{0};
  std::vector<int> select{-1, -1};

  glm::vec3 m_positionCurrent{0.0f, 0.25f, 0.0f};
};

#endif