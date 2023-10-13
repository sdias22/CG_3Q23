#include "window.hpp"

void Window::onCreate() {
  auto const *vertexShader{R"gl(#version 300 es
                                   
    layout(location = 0) in vec2 inPosition;
    layout(location = 1) in vec4 inColor;

    out vec4 fragColor;

    void main() { 
      gl_Position = vec4(inPosition, 0, 1);
      fragColor = inColor;
    }
  )gl"};

  auto const *fragmentShader{R"gl(#version 300 es
                                     
    precision mediump float;

    in vec4 fragColor;

    out vec4 outColor;

    void main() { outColor = fragColor; }
  )gl"};

  m_program = abcg::createOpenGLProgram(
      {{.source = vertexShader, .stage = abcg::ShaderStage::Vertex},
       {.source = fragmentShader, .stage = abcg::ShaderStage::Fragment}});

  // Atualiza a semente do gerador randômico
  auto const seed{std::chrono::steady_clock::now().time_since_epoch().count()};
  m_randomEngine.seed(seed);

  // Limpa a tela com a cor Preta
  abcg::glClearColor(0, 0, 0, 1);
  abcg::glClear(GL_COLOR_BUFFER_BIT);
}

void Window::onPaint() {

  // Mapeamento das coordenadas da tela para o  viewport
  auto minSize{std::min(m_viewportSize.x, m_viewportSize.y)};
  int x{(m_viewportSize.x - minSize) / 2};
  int y{(m_viewportSize.y - minSize) / 2};

  abcg::glViewport(x, y, minSize, minSize);

  // abcg::glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);

  setupModel();

  // Contador para limpar a tela
  if (lmp == 2) {
    abcg::glClear(GL_COLOR_BUFFER_BIT);
    lmp = 0;
  } else if (lmp == 1) {
    lmp += 1;
  }

  // Tempo entre uma forma e outra ser mostrada
  if (m_timer.elapsed() < m_tempo)
    return;
  m_timer.restart();

  // Iniciar o programa que renderiza os shaders
  abcg::glUseProgram(m_program);
  abcg::glBindVertexArray(m_VAO);

  // De acordo com a forma selecionada, passa os vértices para desenhar a forma
  if (m_forma == "Linha") {
    abcg::glDrawArrays(GL_LINES, 0, 2);
  } else if (m_forma == "Triangulo") {
    abcg::glDrawArrays(GL_TRIANGLES, 0, 3);
  } else if (m_forma == "Quadrado") {
    abcg::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  } else {
    abcg::glDrawArrays(GL_POINTS, 0, 1);
  }

  // Resete o renderizador
  abcg::glBindVertexArray(0);
  abcg::glUseProgram(0);
}

void Window::onPaintUI() {
  abcg::OpenGLWindow::onPaintUI();
  {

    auto colorEditFlags{ImGuiColorEditFlags_NoTooltip |
                        ImGuiColorEditFlags_NoPicker};

    // Tamanho e Posição do Menu de Delay + Limpar a janela
    auto const widgetSizeL{ImVec2(165, 65)};
    ImGui::SetNextWindowSize(widgetSizeL);
    ImGui::SetNextWindowPos(ImVec2(180, 5));

    // Inicializa o Delay + Limpar a janela
    ImGui::Begin(" ", nullptr, ImGuiWindowFlags_NoDecoration);

    // Slider para pegar o delay a ser usado
    ImGui::PushItemWidth(100);
    ImGui::SliderFloat("Delay", &m_tempo, 0.0f, 1.0f, "%.3f ms");
    ImGui::PopItemWidth();

    // Botão para limpar a tela
    if (ImGui::Button("Clear window", ImVec2(150, 25))) {
      abcg::glClear(GL_COLOR_BUFFER_BIT);
    }

    ImGui::End();

    // Tamanho e Posição do Menu de Forma
    auto const widgetSize{ImVec2(210, 65)};
    ImGui::SetNextWindowPos(ImVec2(355, 5));
    ImGui::SetNextWindowSize(widgetSize);

    // Inicializa o Menu de Forma
    auto windowFlags{ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar};
    ImGui::Begin("Forma", nullptr, windowFlags);

    // ComboBox para escolha da forma do desenho
    static std::size_t currentInd{};
    std::vector comboFormaD{"Circulo", "Triangulo", "Quadrado"};

    if (ImGui::BeginCombo("Desenho", comboFormaD.at(currentInd))) {
      for (auto ind{0U}; ind < comboFormaD.size(); ++ind) {
        bool const isSelected{currentInd == ind};
        if (ImGui::Selectable(comboFormaD.at(ind), isSelected)) {
          currentInd = ind;
          lmp = 1;
        }
        if (isSelected)
          ImGui::SetItemDefaultFocus();
      }

      ImGui::EndCombo();
      m_formaDesenho = comboFormaD.at(currentInd);
    }

    // ComboBox para escolha da forma
    static std::size_t currentIndex{};
    std::vector comboItems{"Ponto", "Linha", "Triangulo", "Quadrado"};

    if (ImGui::BeginCombo("Forma", comboItems.at(currentIndex))) {
      for (auto index{0U}; index < comboItems.size(); ++index) {
        bool const isSelected{currentIndex == index};
        if (ImGui::Selectable(comboItems.at(index), isSelected)) {
          currentIndex = index;
          lmp = 1;
        }
        if (isSelected)
          ImGui::SetItemDefaultFocus();
      }

      ImGui::EndCombo();

      m_forma = comboItems.at(currentIndex);
    }

    ImGui::End();

    // Tamanho e Posição do Menu Cor
    auto const widgetSizeC{ImVec2(290, 110)};
    ImGui::SetNextWindowPos(ImVec2(5, m_viewportSize.y - widgetSizeC.y - 5));
    ImGui::SetNextWindowSize(widgetSizeC);

    // Inicializa o Menu de Cor
    ImGui::Begin("Cor", nullptr, windowFlags);

    // CheckBox para desenhar com uma única cor ou não;
    ImGui::Checkbox("Cor única", &enabled);

    // Caso o CheckBox esteja ativo ou forma for um Ponto aparece a opção de
    // escolher 1 cor, caso contrário aparece as outras opções para montar o
    // degradê

    // Opção de cores dividido em 2 menus para melhor distribuição na tela
    if (enabled || m_forma == "Ponto") {
      ImGui::Text("Selecione uma cor única:");
      ImGui::PushItemWidth(215);
      ImGui::ColorEdit3("Cor", &m_uniqueColor.at(0).x, colorEditFlags);
      ImGui::PopItemWidth();

      m_uniqueColor.at(1) = m_uniqueColor.at(0);
      m_uniqueColor.at(2) = m_uniqueColor.at(0);
      m_uniqueColor.at(3) = m_uniqueColor.at(0);

      // Configurar para cores únicas
      uniqueColors = true;

    } else if (m_forma == "Linha") {
      ImGui::Text("Selecione cores diferentes:");
      ImGui::PushItemWidth(215);
      ImGui::ColorEdit3("Cor 1", &m_colors.at(0).x, colorEditFlags);
      ImGui::ColorEdit3("Cor 2", &m_colors.at(1).x, colorEditFlags);
      ImGui::PopItemWidth();

      // Configurar para cores diferentes
      uniqueColors = false;
    } else if (m_forma == "Triangulo") {
      ImGui::Text("Selecione cores diferentes:");
      ImGui::PushItemWidth(215);
      ImGui::ColorEdit3("Cor 1", &m_colors.at(0).x, colorEditFlags);
      ImGui::ColorEdit3("Cor 2", &m_colors.at(1).x, colorEditFlags);
      ImGui::PopItemWidth();

      // Configurar para cores diferentes
      uniqueColors = false;
    } else if (m_forma == "Quadrado") {
      ImGui::Text("Selecione cores diferentes:");
      ImGui::PushItemWidth(215);
      ImGui::ColorEdit3("Cor 1", &m_colors.at(0).x, colorEditFlags);
      ImGui::ColorEdit3("Cor 2", &m_colors.at(1).x, colorEditFlags);
      ImGui::PopItemWidth();

      // Configurar para cores diferentes
      uniqueColors = false;
    }

    ImGui::End();

    // Se tem mais de 2 cores aparece esse segundo Menu
    if ((!enabled) && (m_forma != "Ponto" && m_forma != "Linha")) {
      // Tamanho e Posição do Menu Cor2
      auto const widgetSizeC2{ImVec2(295, 110)};
      ImGui::SetNextWindowPos(
          ImVec2(305, m_viewportSize.y - widgetSizeC2.y - 5));
      ImGui::SetNextWindowSize(widgetSizeC);

      // Inicialização do Menu de Cor
      ImGui::Begin("Cor2", nullptr, windowFlags);

      ImGui::Text("\n\n");
      ImGui::Spacing();
      ImGui::Spacing();

      if (m_forma == "Triangulo") {
        ImGui::PushItemWidth(215);
        ImGui::ColorEdit3("Cor 3", &m_colors.at(2).x, colorEditFlags);
        ImGui::PopItemWidth();

        // Configurar para cores diferentes
        uniqueColors = false;

      } else if (m_forma == "Quadrado") {
        ImGui::PushItemWidth(215);
        ImGui::ColorEdit3("Cor 3", &m_colors.at(2).x, colorEditFlags);
        ImGui::ColorEdit3("Cor 4", &m_colors.at(3).x, colorEditFlags);
        ImGui::PopItemWidth();

        // Configurar para cores diferentes
        uniqueColors = false;
      }

      ImGui::End();
    }
  }
}

void Window::onResize(glm::ivec2 const &size) {
  m_viewportSize = size;
  angle = 0.0f;
  abcg::glClear(GL_COLOR_BUFFER_BIT);
}

void Window::setupModel() {
  abcg::glDeleteBuffers(1, &m_VBOPositions);
  abcg::glDeleteBuffers(1, &m_VBOColors);
  abcg::glDeleteVertexArrays(1, &m_VAO);

  /*--- Formato da impressão ---*/

  if (m_formaDesenho == "Circulo") {
    // Calcula a posição
    m_P.x = radius * std::sin(angle);
    m_P.y = radius * std::cos(angle);
    // Recalcula o novo ângulo
    angle -= (2.0 * M_PI) / 0.5 * getDeltaTime();

    // Gera um raio aleatório
    std::uniform_real_distribution<float> realDistribution(0.01f, 0.5f);
    radius = realDistribution(m_randomEngine);

  } else if (m_formaDesenho == "Quadrado") {

    std::uniform_real_distribution<float> realDistribution(-0.5f, 0.5f);
    m_P.x = realDistribution(m_randomEngine);
    m_P.y = realDistribution(m_randomEngine);

    // Superior Esquerdo v = -0.5 e h = 0.5
    // Superior Direito v = 0.5 e h = 0.5
    // Inferior Direito v = 0.5 e h = -0.5
    // Inferior Esquerdo v = -0.5 e h = -0.5

  } else if (m_formaDesenho == "Triangulo") {

    // Calcula a coordenada Y
    std::uniform_real_distribution<float> realDistribution(-0.5f, 0.5f);
    m_P.y = realDistribution(m_randomEngine);

    // Dado a coordenada Y, valida os possíveis valores da coordenada x
    float xMin{};
    float xMax{};

    if (m_P.y > 0.0f) {
      xMax = (m_P.y / 2) - 0.25f;
      xMin = xMax * -1;
    } else {
      xMin = (m_P.y / 2) - 0.25f;
      xMax = abs(xMin);
    }

    // Calcula a coordenada X
    std::uniform_real_distribution<float> x_realDistribution(xMin, xMax);
    m_P.x = x_realDistribution(m_randomEngine);
  }

  /*--- Linha ---*/
  std::array<glm::vec2, 2> const lpositions{
      {{m_P.x, m_P.y}, {m_P.x - 0.1f, m_P.y - 0.1f}}};

  /*--- Triangulo ---*/
  std::array<glm::vec2, 3> const tpositions{{{m_P.x, m_P.y},
                                             {m_P.x + 0.1f, m_P.y - 0.1f},
                                             {m_P.x - 0.1f, m_P.y - 0.1f}}};

  /*--- Quadrado ---*/
  std::array<glm::vec2, 4> const qpositions{{
      {m_P.x - 0.1f, m_P.y + 0.1f},
      {m_P.x + 0.1f, m_P.y + 0.1f},
      {m_P.x - 0.1f, m_P.y - 0.1f},
      {m_P.x + 0.1f, m_P.y - 0.1f},
  }};

  // Gera o VBO das posições de acordo com a forma selecionada
  abcg::glGenBuffers(1, &m_VBOPositions);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBOPositions);
  if (m_forma == "Linha") {
    abcg::glBufferData(GL_ARRAY_BUFFER, sizeof(lpositions), lpositions.data(),
                       GL_STATIC_DRAW);
  } else if (m_forma == "Triangulo") {
    abcg::glBufferData(GL_ARRAY_BUFFER, sizeof(tpositions), tpositions.data(),
                       GL_STATIC_DRAW);
  } else if (m_forma == "Quadrado") {
    abcg::glBufferData(GL_ARRAY_BUFFER, sizeof(qpositions), qpositions.data(),
                       GL_STATIC_DRAW);
  } else {
    abcg::glBufferData(GL_ARRAY_BUFFER, sizeof(m_P), &m_P, GL_STATIC_DRAW);
  }

  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Gera o VBO das cores
  abcg::glGenBuffers(1, &m_VBOColors);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBOColors);
  if (uniqueColors) {
    abcg::glBufferData(GL_ARRAY_BUFFER, sizeof(m_uniqueColor),
                       m_uniqueColor.data(), GL_STATIC_DRAW);
  } else {
    abcg::glBufferData(GL_ARRAY_BUFFER, sizeof(m_colors), m_colors.data(),
                       GL_STATIC_DRAW);
  }

  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  auto const positionAttribute{
      abcg::glGetAttribLocation(m_program, "inPosition")};
  auto const colorAttribute{abcg::glGetAttribLocation(m_program, "inColor")};

  // Create VAO
  abcg::glGenVertexArrays(1, &m_VAO);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(m_VAO);

  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBOPositions);
  abcg::glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  abcg::glEnableVertexAttribArray(colorAttribute);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBOColors);
  abcg::glVertexAttribPointer(colorAttribute, 4, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);
}

void Window::onDestroy() {
  abcg::glDeleteProgram(m_program);
  abcg::glDeleteBuffers(1, &m_VBOPositions);
  abcg::glDeleteBuffers(1, &m_VBOColors);
  abcg::glDeleteVertexArrays(1, &m_VAO);
}