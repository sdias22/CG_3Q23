#include "window.hpp"

#include <iostream>
#include <unordered_map>

void Window::onEvent(SDL_Event const &event) {
  if (event.type == SDL_KEYDOWN) {
    if (event.key.keysym.sym == SDLK_w) {
      m_select.onzMove(0.2f);
    }

    if (event.key.keysym.sym == SDLK_s) {
      m_select.onzMove(-0.2f);
    }

    if (event.key.keysym.sym == SDLK_a) {
      m_select.onxMove(-0.2f);
    }

    if (event.key.keysym.sym == SDLK_d) {
      m_select.onxMove(0.2f);
    }

    if (event.key.keysym.sym == SDLK_SPACE) {
      m_select.onSelect(true, -0.075f);
      m_play = PlayStatus::select;
    }
  }
  if (event.type == SDL_KEYUP) {
    if (event.key.keysym.sym == SDLK_SPACE && m_ymove > 0) {
      m_select.onSelect(false, 0.075f);
    }
  }
}

void Window::onCreate() {
  auto const &assetsPath{abcg::Application::getAssetsPath()};
  abcg::glClearColor(0, 0, 0, 1);
  abcg::glEnable(GL_DEPTH_TEST);

  // Cria o Programa
  m_program = abcg::createOpenGLProgram(
      {{.source = assetsPath + "cube.vert", .stage = abcg::ShaderStage::Vertex},
       {.source = assetsPath + "cube.frag",
        .stage = abcg::ShaderStage::Fragment}});

  // Cria o Tabuleiro
  m_ground.onCreate(m_program);

  // Cria as posições das Cameras
  m_camera.onCreate();

  // Cria os cubos
  m_cube.onCreate(m_program);

  // Cria o seletor
  m_select.onCreate(m_program);

  // Get location of uniform variables
  m_viewMatrixLocation = abcg::glGetUniformLocation(m_program, "viewMatrix");
  m_projMatrixLocation = abcg::glGetUniformLocation(m_program, "projMatrix");
  m_modelMatrixLocation = abcg::glGetUniformLocation(m_program, "modelMatrix");
  m_colorLocation = abcg::glGetUniformLocation(m_program, "color");

  m_game = GameStatus::playing;
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

  // Desenha o tabuleiro
  m_ground.onPaint();

  if (m_game == GameStatus::playing) {
    // Desenha as peças e seleção
    m_select.onPaint();
    m_cube.onPaint();
  }

  abcg::glUseProgram(0);
}

void Window::onUpdate() {
  onGame();

  m_camera.onUpdate(m_camSelect);

  if (m_play == PlayStatus::select) {

    m_positionCurrent = m_select.m_positionCurrent;

    int pos = ((m_positionCurrent.x + 0.8) / 0.2) +
              ((m_positionCurrent.z + 0.8) / 0.2) * 8;

    if (m_cube.onSelect(pos)) {
      select[posSelect] = pos;
      posSelect += 1;
    }

    m_timer.restart();
    while (m_timer.elapsed() < m_tempo * 1.5f)
      ;
    m_timer.restart();

    if (posSelect == 2) {
      m_play = PlayStatus::decision;
    } else {
      m_play = PlayStatus::moving;
    }

  } else if (m_play == PlayStatus::decision) {
    m_select.onUpdate();

    m_timer.restart();
    while (m_timer.elapsed() < m_tempo * 2.5f)
      m_cube.onUpdate();
    m_timer.restart();

    m_cube.onDecision(select[0], select[1]);
    tries += 1;
    posSelect = 0;
    select[0] = -1;
    select[1] = -1;
    m_play = PlayStatus::moving;

  } else {
    m_select.onSelect(false, 0.0f);
    m_cube.onUpdate();
    m_select.onUpdate();
  }
}

void Window::onPaintUI() {
  // Tamanho e Posição do Menu
  auto const widgetSize{ImVec2(210, 70)};
  ImGui::SetNextWindowPos(ImVec2(355, 5));
  ImGui::SetNextWindowSize(widgetSize);

  // Inicializa o Menu
  auto windowFlags{ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar};
  ImGui::Begin("Camera", nullptr, windowFlags);

  // ComboBox para escolha da camera
  static std::size_t currentInd{};
  std::vector<std::string> const comboCam{
      "Vista de Cima", "CAM1", "CAM2", "CAM3", "CAM4", "CAM5", "CAM6"};

  if (ImGui::BeginCombo("Camera", comboCam.at(currentInd).c_str())) {
    for (auto ind{0U}; ind < comboCam.size(); ++ind) {
      bool const isSelected{currentInd == ind};
      if (ImGui::Selectable(comboCam.at(ind).c_str(), isSelected)) {
        currentInd = ind;
      }
      if (isSelected)
        ImGui::SetItemDefaultFocus();
    }
    ImGui::EndCombo();
  }
  m_camSelect = currentInd;

  // Botão para limpar a tela
  if (ImGui::Button("Restart", ImVec2(150, 25))) {
    m_restart = true;
  }

  ImGui::End();

  // Tamanho e Posição do texto
  auto const widgetSizeT{ImVec2(210, 45)};
  ImGui::SetNextWindowPos(ImVec2((m_viewportSize.x - widgetSizeT.x) / 2,
                                 (m_viewportSize.y - widgetSizeT.y) / 2));
  ImGui::SetNextWindowSize(widgetSizeT);

  if (m_game == GameStatus::gameover) {

    // Inicializa o Menu
    ImGui::Begin("Texto", nullptr, windowFlags);

    ImGui::Text("Congratulations!\n");
    ImGui::Text("You need: %d tries!", tries);
    ImGui::End();
  }
}

void Window::onGame() {
  if (m_restart) {
    onRestart();
  } else if (m_cube.onGameOver()) {
    m_game = GameStatus::gameover;
    m_timerRestart.restart();
    while (m_timerRestart.elapsed() < m_tempo * 1.5)
      ;
    m_timerRestart.restart();
    onRestart();
  }
}

void Window::onRestart() {
  abcg::glClearColor(0, 0, 0, 1);
  m_restart = false;
  tries = 0;
  onDestroy();
  onCreate();
}

void Window::onResize(glm::ivec2 const &size) {
  m_viewportSize = size;
  m_camera.computeProjectionMatrix(size);
  m_camera.sizeT = size;
}

void Window::onDestroy() {
  // Destruir o Tabuleiro
  m_ground.onDestroy();

  // Destruir peças
  m_cube.onDestroy();

  // Destruir peças
  m_select.onDestroy();

  abcg::glDeleteProgram(m_program);
}