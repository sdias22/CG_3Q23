#include "ground.hpp"

void Ground::onSetup() {
  auto const &assetsPath{abcg::Application::getAssetsPath()};

  // Cria o Programa
  m_program =
      abcg::createOpenGLProgram({{.source = assetsPath + "ground.vert",
                                  .stage = abcg::ShaderStage::Vertex},
                                 {.source = assetsPath + "ground.frag",
                                  .stage = abcg::ShaderStage::Fragment}});

  // Save location of uniform variables
  m_viewMatrixLocation = abcg::glGetUniformLocation(m_program, "viewMatrix");
  m_projMatrixLocation = abcg::glGetUniformLocation(m_program, "projMatrix");
  m_modelMatrixLocation = abcg::glGetUniformLocation(m_program, "modelMatrix");
  m_colorLocation = abcg::glGetUniformLocation(m_program, "color");
}

void Ground::onCreate() {
  onSetup();

  // Unit quad on the xz plane
  std::array<glm::vec3, 4> m_vertices{{{-0.5000f, 0.0f, +0.5000f},
                                       {-0.5000f, 0.0f, -0.5000f},
                                       {+0.5000f, 0.0f, +0.5000f},
                                       {+0.5000f, 0.0f, -0.5000f}}};

  // Generate VBO
  abcg::glGenBuffers(1, &m_VBO);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  abcg::glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices.data(),
                     GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Create VAO and bind vertex attributes
  abcg::glGenVertexArrays(1, &m_VAO);
  abcg::glBindVertexArray(m_VAO);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  auto const positionAttribute{
      abcg::glGetAttribLocation(m_program, "inPosition")};
  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);
  abcg::glBindVertexArray(0);
}

void Ground::onPaint(glm::mat4 m_ViewMatrix, glm::mat4 m_ProjMatrix) {
  abcg::glUseProgram(m_program);

  // Set uniform variables for viewMatrix and projMatrix
  abcg::glUniformMatrix4fv(m_viewMatrixLocation, 1, GL_FALSE,
                           &m_ViewMatrix[0][0]);
  abcg::glUniformMatrix4fv(m_projMatrixLocation, 1, GL_FALSE,
                           &m_ProjMatrix[0][0]);

  abcg::glBindVertexArray(m_VAO);

  // Draw a grid of 2N x 2N tiles on the xz plane, centered around the
  // origin
  auto const N{4};

  for (auto const z : iter::range(-N, N)) {
    for (auto const x : iter::range(-N, N)) {
      // Set model matrix as a translation matrix
      glm::mat4 model{1.0f};

      model = glm::translate(model, glm::vec3(x * 0.2f, 0.0f, z * 0.2f));
      model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));

      abcg::glUniformMatrix4fv(m_modelMatrixLocation, 1, GL_FALSE,
                               &model[0][0]);

      // Set color (checkerboard pattern)
      auto const whiteOrDarkGray{(z + x) % 2 == 0 ? 1.0f : 0.1f};
      abcg::glUniform4f(m_colorLocation, whiteOrDarkGray, whiteOrDarkGray,
                        whiteOrDarkGray, 1.0f);

      abcg::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
  }

  abcg::glBindVertexArray(0);

  abcg::glUseProgram(0);
}

void Ground::onDestroy() {
  abcg::glDeleteBuffers(1, &m_VBO);
  abcg::glDeleteVertexArrays(1, &m_VAO);
  abcg::glDeleteProgram(m_program);
}