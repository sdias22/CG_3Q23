#include "background.hpp"
#include <filesystem>

void Background::onCreate() {
  auto const assetsPath{abcg::Application::getAssetsPath()};
  m_program =
      abcg::createOpenGLProgram({{.source = assetsPath + "background.vert",
                                  .stage = abcg::ShaderStage::Vertex},
                                 {.source = assetsPath + "background.frag",
                                  .stage = abcg::ShaderStage::Fragment}});

  loadTexture(assetsPath + "background/");

  m_viewMatrixLocation = abcg::glGetUniformLocation(m_program, "viewMatrix");
  m_projMatrixLocation = abcg::glGetUniformLocation(m_program, "projMatrix");

  // Generate VBO
  abcg::glGenBuffers(1, &m_VBO);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  abcg::glBufferData(GL_ARRAY_BUFFER, sizeof(m_Positions), m_Positions.data(),
                     GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  auto const positionAttribute{
      abcg::glGetAttribLocation(m_program, "inPosition")};

  // Create VAO
  abcg::glGenVertexArrays(1, &m_VAO);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(m_VAO);

  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);
}

void Background::onPaint(glm::mat4 m_ViewMatrix, glm::mat4 m_ProjMatrix) {
  abcg::glUseProgram(m_program);

  // Set uniform variables for viewMatrix and projMatrix
  abcg::glUniformMatrix4fv(m_viewMatrixLocation, 1, GL_FALSE,
                           &m_ViewMatrix[0][0]);
  abcg::glUniformMatrix4fv(m_projMatrixLocation, 1, GL_FALSE,
                           &m_ProjMatrix[0][0]);

  auto const m_TextureLocation{abcg::glGetUniformLocation(m_program, "m_Tex")};

  abcg::glUniform1i(m_TextureLocation, 0);

  abcg::glBindVertexArray(m_VAO);

  abcg::glActiveTexture(GL_TEXTURE0);
  abcg::glBindTexture(GL_TEXTURE_CUBE_MAP, m_Texture);

  abcg::glEnable(GL_CULL_FACE);
  abcg::glFrontFace(GL_CW);
  abcg::glDepthFunc(GL_LEQUAL);
  abcg::glDrawArrays(GL_TRIANGLES, 0, m_Positions.size());
  abcg::glDepthFunc(GL_LESS);
  abcg::glDisable(GL_CULL_FACE);
  abcg::glFrontFace(GL_CCW);

  abcg::glBindVertexArray(0);
  abcg::glUseProgram(0);
}

void Background::onDestroy() {
  abcg::glDeleteBuffers(1, &m_VBO);
  abcg::glDeleteVertexArrays(1, &m_VAO);
  abcg::glDeleteProgram(m_program);
}

void Background::loadTexture(std::string const &path) {
  if (!std::filesystem::exists(path))
    return;

  abcg::glDeleteTextures(1, &m_Texture);
  m_Texture = abcg::loadOpenGLCubemap(
      {.paths = {path + "posx.jpg", path + "negx.jpg", path + "posy.jpg",
                 path + "negy.jpg", path + "posz.jpg", path + "negz.jpg"}});
}