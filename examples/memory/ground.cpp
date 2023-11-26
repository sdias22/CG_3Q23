#include "ground.hpp"

void Ground::onCreate(GLuint m_program) {
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

  // Save location of uniform variables
  m_modelMatrixLoc = abcg::glGetUniformLocation(m_program, "modelMatrix");
  m_colorLoc = abcg::glGetUniformLocation(m_program, "color");
}

void Ground::onPaint() {
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

      abcg::glUniformMatrix4fv(m_modelMatrixLoc, 1, GL_FALSE, &model[0][0]);

      // Set color (checkerboard pattern)
      auto const whiteOrDarkGray{(z + x) % 2 == 0 ? 1.0f : 0.1f};
      abcg::glUniform4f(m_colorLoc, whiteOrDarkGray, whiteOrDarkGray,
                        whiteOrDarkGray, 1.0f);

      abcg::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
  }

  abcg::glBindVertexArray(0);
}

void Ground::onDestroy() {
  abcg::glDeleteBuffers(1, &m_VBO);
  abcg::glDeleteVertexArrays(1, &m_VAO);
}