#include "window.hpp"

#include <unordered_map>

void Window::onEvent(SDL_Event const &event) {
  // Mover a Camera para frente e trás em direção ao ponto de vista
  if (event.type == SDL_MOUSEWHEEL) {
    m_dollySpeed += (event.wheel.y > 0 ? 1.0f : -1.0f) / 5.0f;
    m_dollySpeed = glm::clamp(m_dollySpeed, -1.5f, 1.0f);
  }

  /*
    Teclas:
    W: Mover a Camera para cima
    S: Mover a Camera para baixa
    Q: Gira a câmera para esquerda em torno de seu eixo y
    E: Gira a câmera para direita em torno de seu eixo y
    A: Desloca a câmera para a esquerda
    D: Desloca a câmera para a direita
    Z: Rotacionar / Inclinar a Camera para esquerda
    X: Rotacionar / Inclinar a Camera para direita
  */

  if (event.type == SDL_KEYDOWN) {
    if (event.key.keysym.sym == SDLK_w)
      m_upSpeed = 1.0f;
    if (event.key.keysym.sym == SDLK_s)
      m_upSpeed = -1.0f;
    if (event.key.keysym.sym == SDLK_q)
      m_panSpeed = -1.0f;
    if (event.key.keysym.sym == SDLK_e)
      m_panSpeed = 1.0f;
    if (event.key.keysym.sym == SDLK_a)
      m_truckSpeed = -1.0f;
    if (event.key.keysym.sym == SDLK_d)
      m_truckSpeed = 1.0f;
    if (event.key.keysym.sym == SDLK_z)
      m_rotateSpeed = -1.0f;
    if (event.key.keysym.sym == SDLK_x)
      m_rotateSpeed = 1.0f;
  }
  if (event.type == SDL_KEYUP) {
    if (event.key.keysym.sym == SDLK_w && m_upSpeed > 0)
      m_upSpeed = 0.0f;
    if (event.key.keysym.sym == SDLK_s && m_upSpeed < 0)
      m_upSpeed = 0.0f;
    if (event.key.keysym.sym == SDLK_q && m_panSpeed < 0)
      m_panSpeed = 0.0f;
    if (event.key.keysym.sym == SDLK_e && m_panSpeed > 0)
      m_panSpeed = 0.0f;
    if (event.key.keysym.sym == SDLK_a && m_truckSpeed < 0)
      m_truckSpeed = 0.0f;
    if (event.key.keysym.sym == SDLK_d && m_truckSpeed > 0)
      m_truckSpeed = 0.0f;
    if (event.key.keysym.sym == SDLK_z && m_rotateSpeed < 0)
      m_rotateSpeed = 0.0f;
    if (event.key.keysym.sym == SDLK_x && m_rotateSpeed > 0)
      m_rotateSpeed = 0.0f;
  }

  /*
  // Mover as peças
  if (event.type == SDL_KEYDOWN) {
    if (event.key.keysym.sym == SDLK_SPACE)
      m_sMove = 1.0f;
    if (event.key.keysym.sym == SDLK_UP)
      m_vMove = 1.0f;
    if (event.key.keysym.sym == SDLK_DOWN)
      m_vMove = -1.0f;
    if (event.key.keysym.sym == SDLK_LEFT)
      m_hMove = -1.0f;
    if (event.key.keysym.sym == SDLK_RIGHT)
      m_hMove = 1.0f;
  }
  if (event.type == SDL_KEYUP) {
    if (event.key.keysym.sym == SDLK_UP && m_vMove > 0)
      m_vMove = 0.0f;
    if (event.key.keysym.sym == SDLK_DOWN && m_vMove < 0)
      m_vMove = 0.0f;
    if (event.key.keysym.sym == SDLK_LEFT && m_hMove < 0)
      m_hMove = 0.0f;
    if (event.key.keysym.sym == SDLK_RIGHT && m_hMove > 0)
      m_hMove = 0.0f;
  }
  */
}

void Window::onCreate() {
  auto const &assetsPath{abcg::Application::getAssetsPath()};
  abcg::glClearColor(0, 0, 0, 1);
  abcg::glEnable(GL_DEPTH_TEST);

  // Cria o Programa
  m_program =
      abcg::createOpenGLProgram({{.source = assetsPath + "pieces.vert",
                                  .stage = abcg::ShaderStage::Vertex},
                                 {.source = assetsPath + "pieces.frag",
                                  .stage = abcg::ShaderStage::Fragment}});

  // Cria o Tabuleiro
  m_ground.onCreate(m_program);

  // Criar as peças Brancas
  m_reiW.onCreate(m_program, Input::rei);
  m_rainhaW.onCreate(m_program, Input::rainha);
  m_bispoWL.onCreate(m_program, Input::bispo);
  m_cavaloWL.onCreate(m_program, Input::cavalo);
  m_cavaloWR.onCreate(m_program, Input::cavalo);
  m_torreWL.onCreate(m_program, Input::torre);
  m_torreWR.onCreate(m_program, Input::torre);
  m_peaoW1.onCreate(m_program, Input::peao);
  m_peaoW2.onCreate(m_program, Input::peao);
  m_peaoW3.onCreate(m_program, Input::peao);
  m_peaoW4.onCreate(m_program, Input::peao);
  m_peaoW5.onCreate(m_program, Input::peao);
  m_peaoW6.onCreate(m_program, Input::peao);
  m_peaoW7.onCreate(m_program, Input::peao);
  m_peaoW8.onCreate(m_program, Input::peao);

  // Criar as peças Cinza
  m_reiB.onCreate(m_program, Input::rei);
  m_rainhaB.onCreate(m_program, Input::rainha);
  m_bispoBL.onCreate(m_program, Input::bispo);
  m_cavaloBL.onCreate(m_program, Input::cavalo);
  m_cavaloBR.onCreate(m_program, Input::cavalo);
  m_torreBL.onCreate(m_program, Input::torre);
  m_torreBR.onCreate(m_program, Input::torre);
  m_peaoB1.onCreate(m_program, Input::peao);
  m_peaoB2.onCreate(m_program, Input::peao);
  m_peaoB3.onCreate(m_program, Input::peao);
  m_peaoB4.onCreate(m_program, Input::peao);
  m_peaoB5.onCreate(m_program, Input::peao);
  m_peaoB6.onCreate(m_program, Input::peao);
  m_peaoB7.onCreate(m_program, Input::peao);
  m_peaoB8.onCreate(m_program, Input::peao);

  // Get location of uniform variables
  m_viewMatrixLocation = abcg::glGetUniformLocation(m_program, "viewMatrix");
  m_projMatrixLocation = abcg::glGetUniformLocation(m_program, "projMatrix");
  m_modelMatrixLocation = abcg::glGetUniformLocation(m_program, "modelMatrix");
  m_colorLocation = abcg::glGetUniformLocation(m_program, "color");
}

void Window::onUpdate() {
  auto const deltaTime{gsl::narrow_cast<float>(getDeltaTime())};

  // Update LookAt camera
  m_camera.dolly(m_dollySpeed * deltaTime);
  m_camera.truck(m_truckSpeed * deltaTime);
  m_camera.pan(m_panSpeed * deltaTime);
  m_camera.up(m_upSpeed * deltaTime);
  m_camera.rotate(m_rotateSpeed * deltaTime);

  /*
  // Mover peças Brancas
  m_reiW.horizontalmove();
  m_reiW.verticalmove();

  m_rainhaW.horizontalmove();
  m_rainhaW.verticalmove();

  m_bispoWL.horizontalmove();
  m_bispoWL.verticalmove();

  m_bispoWR.horizontalmove();
  m_bispoWR.verticalmove();

  m_cavaloWL.horizontalmove();
  m_cavaloWL.verticalmove();

  m_cavaloWR.horizontalmove();
  m_cavaloWR.verticalmove();

  m_torreWL.horizontalmove();
  m_torreWL.verticalmove();

  m_torreWR.horizontalmove();
  m_torreWR.verticalmove();

  m_peaoW1.horizontalmove();
  m_peaoW1.verticalmove();

  m_peaoW2.horizontalmove();
  m_peaoW2.verticalmove();

  m_peaoW3.horizontalmove();
  m_peaoW3.verticalmove();

  m_peaoW4.horizontalmove();
  m_peaoW4.verticalmove();

  m_peaoW5.horizontalmove();
  m_peaoW5.verticalmove();

  m_peaoW6.horizontalmove();
  m_peaoW6.verticalmove();

  m_peaoW7.horizontalmove();
  m_peaoW7.verticalmove();

  m_peaoW8.horizontalmove();
  m_peaoW8.verticalmove();

  // Mover peças Cinza
  m_reiB.horizontalmove();
  m_reiB.verticalmove();

  m_rainhaB.horizontalmove();
  m_rainhaB.verticalmove();

  m_bispoBL.horizontalmove();
  m_bispoBL.verticalmove();

  m_bispoBR.horizontalmove();
  m_bispoBR.verticalmove();

  m_cavaloBL.horizontalmove();
  m_cavaloBL.verticalmove();

  m_cavaloBR.horizontalmove();
  m_cavaloBR.verticalmove();

  m_torreBL.horizontalmove();
  m_torreBL.verticalmove();

  m_torreBR.horizontalmove();
  m_torreBR.verticalmove();

  m_peaoB1.horizontalmove();
  m_peaoB1.verticalmove();

  m_peaoB2.horizontalmove();
  m_peaoB2.verticalmove();

  m_peaoB3.horizontalmove();
  m_peaoB3.verticalmove();

  m_peaoB4.horizontalmove();
  m_peaoB4.verticalmove();

  m_peaoB5.horizontalmove();
  m_peaoB5.verticalmove();

  m_peaoB6.horizontalmove();
  m_peaoB6.verticalmove();

  m_peaoB7.horizontalmove();
  m_peaoB7.verticalmove();

  m_peaoB8.horizontalmove();
  m_peaoB8.verticalmove();
  */
}

void Window::onPaint() {
  // Clear color buffer and depth buffer
  abcg::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  abcg::glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);

  abcg::glUseProgram(m_program);

  // Set uniform variables for viewMatrix and projMatrix
  // These matrices are used for every scene object
  abcg::glUniformMatrix4fv(m_viewMatrixLocation, 1, GL_FALSE,
                           &m_camera.getViewMatrix()[0][0]);
  abcg::glUniformMatrix4fv(m_projMatrixLocation, 1, GL_FALSE,
                           &m_camera.getProjMatrix()[0][0]);

  // Desenhar as Peças Cinza
  m_reiB.onPaint(glm::vec3(0.0f, 0.020f, -0.8f), gray, Input::rei);
  m_rainhaB.onPaint(glm::vec3(-0.2f, 0.02f, -0.8f), gray, Input::rainha);
  m_bispoBL.onPaint(glm::vec3(-0.4f, 0.01f, -0.8f), gray, Input::bispo);
  m_bispoBL.onPaint(glm::vec3(0.2f, 0.01f, -0.8f), gray, Input::bispo);
  m_cavaloBL.onPaint(glm::vec3(-0.6f, 0.0f, -0.8f), gray, Input::cavalo);
  m_cavaloBR.onPaint(glm::vec3(0.4f, 0.0f, -0.8f), gray, Input::cavalo);
  m_torreBL.onPaint(glm::vec3(-0.8f, -0.005f, -0.8f), gray, Input::torre);
  m_torreBR.onPaint(glm::vec3(0.6f, -0.005f, -0.8f), gray, Input::torre);
  m_peaoB1.onPaint(glm::vec3(-0.8f, -0.04f, -0.6f), gray, Input::peao);
  m_peaoB2.onPaint(glm::vec3(-0.6f, -0.04f, -0.6f), gray, Input::peao);
  m_peaoB3.onPaint(glm::vec3(-0.4f, -0.04f, -0.6f), gray, Input::peao);
  m_peaoB4.onPaint(glm::vec3(-0.2f, -0.04f, -0.6f), gray, Input::peao);
  m_peaoB5.onPaint(glm::vec3(0.0f, -0.04f, -0.6f), gray, Input::peao);
  m_peaoB6.onPaint(glm::vec3(0.2f, -0.04f, -0.6f), gray, Input::peao);
  m_peaoB7.onPaint(glm::vec3(0.4f, -0.04f, -0.6f), gray, Input::peao);
  m_peaoB8.onPaint(glm::vec3(0.6f, -0.04f, -0.6f), gray, Input::peao);

  // Desenhar as Peças Brancas
  m_reiW.onPaint(glm::vec3(0.0f, 0.02f, 0.6f), white, Input::rei);
  m_rainhaW.onPaint(glm::vec3(-0.2f, 0.01f, 0.6f), white, Input::rainha);
  m_bispoWL.onPaint(glm::vec3(-0.4f, 0.01f, 0.6f), white, Input::bispo);
  m_bispoWL.onPaint(glm::vec3(0.2f, 0.01f, 0.6f), white, Input::bispo);
  m_cavaloWL.onPaint(glm::vec3(-0.6f, 0.0f, 0.6f), white, Input::cavalo);
  m_cavaloWR.onPaint(glm::vec3(0.4f, 0.0f, 0.6f), white, Input::cavalo);
  m_torreWL.onPaint(glm::vec3(-0.8f, -0.005f, 0.6f), white, Input::torre);
  m_torreWR.onPaint(glm::vec3(0.6f, -0.005f, 0.6f), white, Input::torre);
  m_peaoW1.onPaint(glm::vec3(-0.8f, -0.04f, 0.4f), white, Input::peao);
  m_peaoW2.onPaint(glm::vec3(-0.6f, -0.04f, 0.4f), white, Input::peao);
  m_peaoW3.onPaint(glm::vec3(-0.4f, -0.04f, 0.4f), white, Input::peao);
  m_peaoW4.onPaint(glm::vec3(-0.2f, -0.04f, 0.4f), white, Input::peao);
  m_peaoW5.onPaint(glm::vec3(0.0f, -0.04f, 0.4f), white, Input::peao);
  m_peaoW6.onPaint(glm::vec3(0.2f, -0.04f, 0.4f), white, Input::peao);
  m_peaoW7.onPaint(glm::vec3(0.4f, -0.04f, 0.4f), white, Input::peao);
  m_peaoW8.onPaint(glm::vec3(0.6f, -0.04f, 0.4f), white, Input::peao);

  // Desenha o tabuleiro
  m_ground.onPaint();

  abcg::glUseProgram(0);
}

void Window::onResize(glm::ivec2 const &size) {
  m_viewportSize = size;
  m_camera.computeProjectionMatrix(size);
}

void Window::onDestroy() {
  // Destruir o Tabuleiro
  m_ground.onDestroy();

  // Destruir Peças Brancas
  m_reiW.onDestroy();
  m_rainhaW.onDestroy();
  m_bispoWL.onDestroy();
  m_bispoWR.onDestroy();
  m_cavaloWL.onDestroy();
  m_cavaloWR.onDestroy();
  m_torreWL.onDestroy();
  m_torreWR.onDestroy();
  m_peaoW1.onDestroy();
  m_peaoW2.onDestroy();
  m_peaoW3.onDestroy();
  m_peaoW4.onDestroy();
  m_peaoW5.onDestroy();
  m_peaoW6.onDestroy();
  m_peaoW7.onDestroy();
  m_peaoW8.onDestroy();

  // Destruir Peças Cinza
  m_reiB.onDestroy();
  m_rainhaB.onDestroy();
  m_bispoBL.onDestroy();
  m_bispoBR.onDestroy();
  m_cavaloBL.onDestroy();
  m_cavaloBR.onDestroy();
  m_torreBL.onDestroy();
  m_torreBR.onDestroy();
  m_peaoB1.onDestroy();
  m_peaoB2.onDestroy();
  m_peaoB3.onDestroy();
  m_peaoB4.onDestroy();
  m_peaoB5.onDestroy();
  m_peaoB6.onDestroy();
  m_peaoB7.onDestroy();
  m_peaoB8.onDestroy();
  abcg::glDeleteProgram(m_program);
}