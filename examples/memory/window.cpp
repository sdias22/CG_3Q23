#include "window.hpp"

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

// Limpa a tela e chama o create das demais classe para criarem os objetos
void Window::onCreate() {
  abcg::glClearColor(0, 0, 0, 1);
  abcg::glEnable(GL_DEPTH_TEST);

  // Câmera
  m_viewMatrix = glm::lookAt(glm::vec3(-0.0843976f, 1.1297f, 0.992652f),
                             glm::vec3(-0.0840492f, 0.108946f, -0.106467f),
                             glm::vec3(-0.0175263f, 0.732629f, -0.680398f));

  // Cria o Tabuleiro
  m_ground.onCreate();

  // Cria o Fundo;
  m_background.onCreate();

  // Cria os cubos
  m_cube.onCreate();

  // Cria o seletor
  m_select.onCreate();

  m_game = GameStatus::start;
}

void Window::onPaint() {
  // Clear color buffer and depth buffer
  abcg::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  abcg::glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);

  auto const aspect{gsl::narrow<float>(m_viewportSize.x) /
                    gsl::narrow<float>(m_viewportSize.y)};
  m_projMatrix = glm::perspective(glm::radians(100.0f), aspect, 0.1f, 5.0f);

  // Desenha o tabuleiro
  m_ground.onPaint(m_viewMatrix, m_projMatrix);

  m_background.onPaint(m_viewMatrix, m_projMatrix);

  if (m_game != GameStatus::gameover) {
    // Desenha o seletor
    m_select.onPaint(m_viewMatrix, m_projMatrix);
    // Desenha as peças
    m_cube.onPaint(m_viewMatrix, m_projMatrix);
  }
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

  /*
    Caso o estado do game seja "start" é atualizado o campo, os cubos coloridos
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
    while (m_timer.elapsed() < m_tempo * 4.5f)
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
      No estado Decision é chamado a função da classe Cube e limpo o vetor que
      armazena as posições selecionadas. Após isso é retornado o estado do game
      para "moving".
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
  auto const widgetSize{ImVec2(210, 42)};
  ImGui::SetNextWindowPos(ImVec2(m_viewportSize.x - widgetSize.x - 5, 5));
  ImGui::SetNextWindowSize(widgetSize);

  // Inicializa o Menu
  auto windowFlags{ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar};
  ImGui::Begin("Restart", nullptr, windowFlags);

  // Botão para limpar a tela
  if (ImGui::Button("Restart", ImVec2(195, 25))) {
    m_restart = true;
  }

  ImGui::End();

  // Bloco de texto com mensagens para o usuário
  // Tamanho e Posição do texto
  auto const widgetSizeT{ImVec2(190, 70)};
  ImGui::SetNextWindowPos(ImVec2((m_viewportSize.x - widgetSizeT.x) / 2, 150));
  ImGui::SetNextWindowSize(widgetSizeT);

  if (m_game == GameStatus::gameover && !m_restart) {
    // Inicializa o Menu
    ImGui::Begin("Texto", nullptr, windowFlags);

    ImGui::Text("Parabéns!\n");
    ImGui::Text("Você precisou de\n%d tentativas!", tries);
    ImGui::End();
  } else if (m_game == GameStatus::start) {
    // Inicializa o Menu
    ImGui::Begin("Texto", nullptr, windowFlags);

    ImGui::Text("O jogo está começando!\nMemorize as cores!");
    ImGui::End();
  } else if (m_restart) {
    // Inicializa o Menu
    ImGui::Begin("Texto", nullptr, windowFlags);

    ImGui::Text("O jogo está sendo\nreiniciado!");
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

void Window::onResize(glm::ivec2 const &size) { m_viewportSize = size; }

void Window::onDestroy() {
  // Destruir o Tabuleiro
  m_ground.onDestroy();

  // Destruir o Fundo
  m_background.onDestroy();

  // Destruir peças
  m_cube.onDestroy();

  // Destruir peças
  m_select.onDestroy();
}