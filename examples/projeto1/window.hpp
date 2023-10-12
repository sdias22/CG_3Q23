#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include <random>

#include "abcgOpenGL.hpp"

class Window : public abcg::OpenGLWindow {
protected:
  void onCreate() override;
  void onPaint() override;
  void onPaintUI() override;
  void onResize(glm::ivec2 const &size) override;
  void onDestroy() override;

private:
  // Usado para verificar se a forma terá uma única cor ou a composição de 3
  // cores
  bool uniqueColors{false};
  bool enabled{false};

  // Usado para pegar o tamanho da tela
  glm::ivec2 m_viewportSize{};

  // Usada para pegar o tempo entre uma forma e outra ser mostrada
  abcg::Timer m_timer;
  float m_tempo{0.500f};

  // Usado para armazernar a forma
  std::string m_forma = "Ponto";

  // Usado para armazenar a forma do desenho
  std::string m_formaDesenho = "Circulo";

  // Cores usadas para forma
  std::array<glm::vec4, 4> m_colors{{{0.36f, 0.83f, 1.00f, 1},
                                     {0.63f, 0.00f, 0.61f, 1},
                                     {1.00f, 0.69f, 0.30f, 1},
                                     {0.75f, 0.75f, 0.75f, 1}}};

  // Cor única para forma
  std::array<glm::vec4, 4> m_uniqueColor{{{1.0f, 1.0f, 1.0f, 1.0f},
                                          {1.0f, 1.0f, 1.0f, 1.0f},
                                          {1.0f, 1.0f, 1.0f, 1.0f},
                                          {1.0f, 1.0f, 1.0f, 1.0f}}};

  // Usado na renderização
  GLuint m_VAO{};
  GLuint m_VBOPositions{};
  GLuint m_VBOColors{};
  GLuint m_program{};

  // Gerador de números pseudoaleatórios
  std::default_random_engine m_randomEngine;

  // Usado para fazer o desenho circular
  float angle{};
  float radius = 0.75;

  // Usado para marcar a posição da forma
  glm::vec2 m_P{};

  void setupModel();
};

#endif