#include "window.hpp"

#include <iostream>
#include <unordered_map>

/*
  Utilizado para movimentar o seletor e por meio do botão "espaço" selecionar
  os cubos
*/
void Window::onEvent(SDL_Event const &event) {
  if (m_game == GameStatus::playing) {
    if (event.type == SDL_KEYDOWN) {
      if (event.key.keysym.sym == SDLK_w || event.key.keysym.sym == SDLK_UP) {
        m_select.onzMove(-0.2f);
      }

      if (event.key.keysym.sym == SDLK_s || event.key.keysym.sym == SDLK_DOWN) {
        m_select.onzMove(0.2f);
      }

      if (event.key.keysym.sym == SDLK_a || event.key.keysym.sym == SDLK_LEFT) {
        m_select.onxMove(-0.2f);
      }

      if (event.key.keysym.sym == SDLK_d ||
          event.key.keysym.sym == SDLK_RIGHT) {
        m_select.onxMove(0.2f);
      }

      if (event.key.keysym.sym == SDLK_SPACE) {
        m_select.onSelect(true);
        m_play = PlayStatus::select;
      }
    }
    if (event.type == SDL_KEYUP) {
      if (event.key.keysym.sym == SDLK_SPACE) {
        m_select.onSelect(false);
      }
    }
  }
}

// Cria o programa e chama o create das demais classe para criarem os objetos
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
  m_lightDirLocation =
      abcg::glGetUniformLocation(m_program, "lightDirWorldSpace");

  m_lightPositionLocation =
      abcg::glGetUniformLocation(m_program, "lightPosition");

  m_IaLocation = abcg::glGetUniformLocation(m_program, "Ia");
  m_IdLocation = abcg::glGetUniformLocation(m_program, "Id");
  m_IsLocation = abcg::glGetUniformLocation(m_program, "Is");

  m_game = GameStatus::start;
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

  abcg::glUniform3f(m_lightPositionLocation, m_light.x, m_light.y, m_light.z);

  abcg::glUniform4fv(m_lightDirLocation, 1, &m_light.x);
  abcg::glUniform4fv(m_IaLocation, 1, &m_Ia.x);
  abcg::glUniform4fv(m_IdLocation, 1, &m_Id.x);
  abcg::glUniform4fv(m_IsLocation, 1, &m_Is.x);

  // Desenha o tabuleiro
  m_ground.onPaint();

  if (m_game != GameStatus::gameover) {
    // Desenha as peças e seleção
    m_select.onPaint();
    m_cube.onPaint();
  }

  abcg::glUseProgram(0);
}

void Window::onUpdate() {
  onGame();
  /*
    Caso seja o fim do jogo, é feito o return até ser alterado o status para
    iniciar novamente o jogo
  */
  if (m_game == GameStatus::gameover) {
    return;
  }

  // Atualiza a câmera para a selecionada no onPaintUI
  m_camera.onUpdate(m_camSelect);

  /*
    Caso o estado do game seja "start" é desenhado o campo, os cubos coloridos
    e o seletor, após um intervalo de tempo é oculto as cores dos cubos e
    alterado o estado do jogo para "playing", permitando que o usuário movimente
    e selecione os cubos
  */
  if (m_game == GameStatus::start) {
    m_cube.onUpdate();
    m_select.onUpdate();

    if (m_timer.elapsed() < m_tempo * 5.0f)
      return;

    m_cube.onState();

    m_timer.restart();
    while (m_timer.elapsed() < m_tempo * 1.5f)
      ;
    m_timer.restart();

    m_cube.onUpdate();

    m_game = GameStatus::playing;

    /*
      Já jogando, quando selecionado um cubo é calculado a posição no vetor
      dado a posição do seletor e caso seja o 2° cubo selecionado é alterado o
      estado do jogo para "decision"
    */
  } else if (m_play == PlayStatus::select) {

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

    /*
      No estado Decision é chamado a função da classe Cube e caso o retorno seja
      true é limpo o vetor que armazena as posições selecionadas.
      Após isso é retornado o estado do game para "moving"
    */
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

  } else if (m_play == PlayStatus::moving) {
    m_select.onSelect(false);
    m_cube.onUpdate();
    m_select.onUpdate();
  }
}

void Window::onPaintUI() {
  // Menu de escolha da Câmera e botão de reinicio do jogo
  // Tamanho e Posição do Menu
  auto const widgetSize{ImVec2(210, 70)};
  ImGui::SetNextWindowPos(ImVec2(355, 5));
  ImGui::SetNextWindowSize(widgetSize);

  // Inicializa o Menu
  auto windowFlags{ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar};
  ImGui::Begin("Camera", nullptr, windowFlags);

  // ComboBox para escolha da camera
  static std::size_t currentInd{};
  std::vector<std::string> const comboCam{"Visão Lateral", "Vista Superior"};

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
  if (ImGui::Button("Restart", ImVec2(190, 25))) {
    m_restart = true;
  }

  ImGui::End();

  // Bloco de texto com mensagens para o usuário
  // Tamanho e Posição do texto
  auto const widgetSizeT{ImVec2(190, 60)};
  ImGui::SetNextWindowPos(ImVec2((m_viewportSize.x - widgetSizeT.x) / 2,
                                 (m_viewportSize.y - widgetSizeT.y) / 2));
  ImGui::SetNextWindowSize(widgetSizeT);

  if (m_game == GameStatus::gameover && !m_restart) {
    // Inicializa o Menu
    ImGui::Begin("Texto", nullptr, windowFlags);

    ImGui::Text("Congratulations!\n");
    ImGui::Text("You need: %d tries!", tries);
    ImGui::End();
  } else if (m_game == GameStatus::start) {
    // Inicializa o Menu
    ImGui::Begin("Texto", nullptr, windowFlags);

    ImGui::Text("O jogo está começando!\nMemorize as cores!\n");
    ImGui::End();
  } else if (m_restart) {
    // Inicializa o Menu
    ImGui::Begin("Texto", nullptr, windowFlags);

    ImGui::Text("O jogo está sendo\nreiniciado!\n");
    ImGui::End();
  }
}

// Verifica se o jogo acabou ou foi reiniciado utilizando o botão do onPaintUI
void Window::onGame() {
  if (m_restart || (m_cube.onGameOver() && m_game != GameStatus::start)) {
    m_game = GameStatus::gameover;
    if (m_timer.elapsed() < m_tempo * 7.5)
      return;
    m_timer.restart();
    onRestart();
  }
}

// Reinicia as variáveis e o jogo
void Window::onRestart() {
  abcg::glClearColor(0, 0, 0, 1);
  m_restart = false;
  tries = 0;
  posSelect = 0;
  select[0] = -1;
  select[1] = -1;

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