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
  m_normalMatrixLocation =
      abcg::glGetUniformLocation(m_program, "normalMatrix");

  // Light Properties
  m_lightPositionLocation =
      abcg::glGetUniformLocation(m_program, "lightPosition");

  m_IaLocation = abcg::glGetUniformLocation(m_program, "Ia");
  m_IdLocation = abcg::glGetUniformLocation(m_program, "Id");
  m_IsLocation = abcg::glGetUniformLocation(m_program, "Is");

  // Material properties
  m_KaLocation = abcg::glGetUniformLocation(m_program, "Ka");
  m_KdLocation = abcg::glGetUniformLocation(m_program, "Kd");
  m_KsLocation = abcg::glGetUniformLocation(m_program, "Ks");
}

void Ground::onCreate() {
  onSetup();

  // Unit quad on the xz plane
  std::vector<Vertex> m_vertices{
      {{.position = {+0.5f, 0.0f, -0.5f}, .normal{0.0f, 1.0f, 0.0f}},
       {.position = {-0.5f, 0.0f, -0.5f}, .normal{0.0f, 1.0f, 0.0f}},
       {.position = {+0.5f, 0.0f, +0.5f}, .normal{0.0f, 1.0f, 0.0f}},
       {.position = {-0.5f, 0.0f, +0.5f}, .normal{0.0f, 1.0f, 0.0f}}}};

  // Generate VBO
  abcg::glGenBuffers(1, &m_VBO);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  abcg::glBufferData(GL_ARRAY_BUFFER,
                     sizeof(m_vertices.at(0)) * m_vertices.size(),
                     m_vertices.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Create VAO and bind vertex attributes
  abcg::glGenVertexArrays(1, &m_VAO);
  abcg::glBindVertexArray(m_VAO);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

  auto const positionAttribute{
      abcg::glGetAttribLocation(m_program, "inPosition")};
  // if (positionAttribute >= 0) {
  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE,
                              sizeof(Vertex), nullptr);
  //}

  auto const normalAttribute{abcg::glGetAttribLocation(m_program, "inNormal")};
  if (normalAttribute >= 0) {
    abcg::glEnableVertexAttribArray(normalAttribute);
    auto const offset{offsetof(Vertex, normal)};
    abcg::glVertexAttribPointer(normalAttribute, 3, GL_FLOAT, GL_FALSE,
                                sizeof(Vertex),
                                reinterpret_cast<void *>(offset));
  }

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
      glm::vec4 m_color{(z + x) % 2 == 0 ? white : darkGray};

      abcg::glUniform4fv(m_KaLocation, 1, &m_color.x);
      abcg::glUniform4fv(m_KdLocation, 1, &m_Kd.x);
      abcg::glUniform4fv(m_KsLocation, 1, &m_Ks.x);

      abcg::glUniform3f(m_lightPositionLocation, m_light.x, m_light.y,
                        m_light.z);

      abcg::glUniform4fv(m_IaLocation, 1, &m_Ia.x);
      abcg::glUniform4fv(m_IdLocation, 1, &m_Id.x);
      abcg::glUniform4fv(m_IsLocation, 1, &m_Is.x);

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