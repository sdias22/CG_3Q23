#include "Select.hpp"

#include <unordered_map>

// Explicit specialization of std::hash for Vertex
template <> struct std::hash<Vertex> {
  size_t operator()(Vertex const &vertex) const noexcept {
    auto const h1{std::hash<glm::vec3>()(vertex.position)};
    auto const h2{std::hash<glm::vec3>()(vertex.normal)};
    return abcg::hashCombine(h1, h2);
  }
};

// Criar program e armazena o local com as infos. dos Shaders
void Select::onSetup() {
  auto const &assetsPath{abcg::Application::getAssetsPath()};

  // Cria o Programa
  m_program = abcg::createOpenGLProgram(
      {{.source = assetsPath + "cube.vert", .stage = abcg::ShaderStage::Vertex},
       {.source = assetsPath + "cube.frag",
        .stage = abcg::ShaderStage::Fragment}});

  // Release previous VAO
  abcg::glDeleteVertexArrays(1, &m_VAO);

  // Create VAO
  abcg::glGenVertexArrays(1, &m_VAO);
  abcg::glBindVertexArray(m_VAO);

  // Bind EBO and VBO
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

  // Bind vertex attributes
  auto const positionAttribute{
      abcg::glGetAttribLocation(m_program, "inPosition")};

  if (positionAttribute >= 0) {
    abcg::glEnableVertexAttribArray(positionAttribute);
    abcg::glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE,
                                sizeof(Vertex), nullptr);
  }

  auto const normalAttribute{abcg::glGetAttribLocation(m_program, "inNormal")};
  if (normalAttribute >= 0) {
    abcg::glEnableVertexAttribArray(normalAttribute);
    auto const offset{offsetof(Vertex, normal)};
    abcg::glVertexAttribPointer(normalAttribute, 3, GL_FLOAT, GL_FALSE,
                                sizeof(Vertex),
                                reinterpret_cast<void *>(offset));
  }

  // Get location of uniform variables
  m_viewMatrixLocation = abcg::glGetUniformLocation(m_program, "viewMatrix");
  m_projMatrixLocation = abcg::glGetUniformLocation(m_program, "projMatrix");
  m_modelMatrixLocation = abcg::glGetUniformLocation(m_program, "modelMatrix");
  m_normalMatrixLocation =
      abcg::glGetUniformLocation(m_program, "normalMatrix");
  m_colorMatrixLocation = abcg::glGetUniformLocation(m_program, "color");

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

  // End of binding
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
  abcg::glBindVertexArray(0);
}

/*
  Cria o objeto de Seleção (VAO, VBO e EBO) e atualiza a cor, posição e status
do objeto
*/
void Select::onCreate() {
  auto const assetsPath{abcg::Application::getAssetsPath()};
  loadObj(assetsPath + "select.obj");

  // Criar program e armazena o local com as infos. dos Shaders
  onSetup();

  // Inicializa o m_select
  m_select.m_status = StatusSelect::moving;
  m_select.m_position = m_pInicial;
  m_select.m_color = gray;
}

// Desenha na tela a objeto de seleção
void Select::onPaint(glm::mat4 m_ViewMatrix, glm::mat4 m_ProjMatrix) {
  abcg::glUseProgram(m_program);

  // Set uniform variables for viewMatrix and projMatrix
  abcg::glUniformMatrix4fv(m_viewMatrixLocation, 1, GL_FALSE,
                           &m_ViewMatrix[0][0]);
  abcg::glUniformMatrix4fv(m_projMatrixLocation, 1, GL_FALSE,
                           &m_ProjMatrix[0][0]);

  abcg::glBindVertexArray(m_VAO);

  glm::mat4 model{1.0f};

  model = glm::translate(model, m_select.m_position);
  model = glm::scale(model, glm::vec3(0.45f));

  auto const modelViewMatrix{glm::mat3(m_viewMatrix * model)};
  auto const normalMatrix{glm::inverseTranspose(modelViewMatrix)};
  abcg::glUniformMatrix3fv(m_normalMatrixLocation, 1, GL_FALSE,
                           &normalMatrix[0][0]);

  abcg::glUniformMatrix4fv(m_modelMatrixLocation, 1, GL_FALSE, &model[0][0]);

  abcg::glUniform4fv(m_colorMatrixLocation, 1, &m_select.m_color.x);

  abcg::glUniform4fv(m_KaLocation, 1, &m_Ka.x);
  abcg::glUniform4fv(m_KdLocation, 1, &m_Kd.x);
  abcg::glUniform4fv(m_KsLocation, 1, &m_Ks.x);

  abcg::glUniform3f(m_lightPositionLocation, m_light.x, m_light.y, m_light.z);

  abcg::glUniform4fv(m_IaLocation, 1, &m_Ia.x);
  abcg::glUniform4fv(m_IdLocation, 1, &m_Id.x);
  abcg::glUniform4fv(m_IsLocation, 1, &m_Is.x);

  abcg::glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT,
                       nullptr);
  abcg::glBindVertexArray(0);

  abcg::glUseProgram(0);
}

void Select::onUpdate() {
  // Atualiza a posição atual
  m_positionCurrent = m_select.m_position;

  /*
    Em cada atualização é esperado um tempo para que quando o estado do select
    seja "moving", ou seja, se movendo para selecionar um cubo. O select é
    colorido de cinza ou branco fazendo com que o select fique piscando.
  */
  if (m_timer.elapsed() < m_tempo)
    return;
  m_timer.restart();

  if (m_select.m_status == StatusSelect::moving) {
    if (m_select.m_color == gray) {
      m_select.m_color = white;
    } else {
      m_select.m_color = gray;
    }
  }
}

/*
  Caso o estado seja select (decidindo se os cubos escolhidas são iguais), é
  colorido com a cor amarela.
*/
void Select::onSelect(bool sel) {
  if (sel) {
    m_select.m_status = StatusSelect::select;
    m_select.m_color = yellow;
  } else {
    m_select.m_status = StatusSelect::moving;
  }
}

/*
  É movimentado no eixo x / horizontal, dentro do intervalo de blocos, caso
  contrário não é feito nada
*/
void Select::onxMove(float move) {
  if (m_select.m_position.x + move >= -0.8f &&
      m_select.m_position.x + move <= 0.6f) {
    m_select.m_position =
        glm::vec3(m_select.m_position.x + move, m_select.m_position.y,
                  m_select.m_position.z);
  }
}

/*
  É movimentado no eixo y / vertical, dentro do intervalo de blocos, caso
  contrário não é feito nada
*/
void Select::onzMove(float move) {
  if (m_select.m_position.z + move >= -0.8f &&
      m_select.m_position.z + move <= 0.6f) {
    m_select.m_position =
        glm::vec3(m_select.m_position.x, m_select.m_position.y,
                  m_select.m_position.z + move);
  }
}

void Select::loadObj(std::string_view path) {

  tinyobj::ObjReader reader;

  if (!reader.ParseFromFile(path.data())) {
    if (!reader.Error().empty()) {
      throw abcg::RuntimeError(
          fmt::format("Failed to load model {} ({})", path, reader.Error()));
    }
    throw abcg::RuntimeError(fmt::format("Failed to load model {}", path));
  }

  if (!reader.Warning().empty()) {
    fmt::print("Warning: {}\n", reader.Warning());
  }

  auto const &attrib{reader.GetAttrib()};
  auto const &shapes{reader.GetShapes()};

  m_vertices.clear();
  m_indices.clear();

  m_hasNormals = false;

  // A key:value map with key=Vertex and value=index
  std::unordered_map<Vertex, GLuint> hash{};

  // Loop over shapes
  for (auto const &shape : shapes) {
    // Loop over indices
    for (auto const offset : iter::range(shape.mesh.indices.size())) {
      // Access to vertex
      auto const index{shape.mesh.indices.at(offset)};

      // Vertex position
      auto const startIndex{3 * index.vertex_index};
      glm::vec3 position{attrib.vertices.at(startIndex + 0),
                         attrib.vertices.at(startIndex + 1),
                         attrib.vertices.at(startIndex + 2)};

      // Vertex normal
      glm::vec3 normal{};
      if (index.normal_index >= 0) {
        m_hasNormals = true;
        auto const normalStartIndex{3 * index.normal_index};
        normal = {attrib.normals.at(normalStartIndex + 0),
                  attrib.normals.at(normalStartIndex + 1),
                  attrib.normals.at(normalStartIndex + 2)};
      }

      Vertex const vertex{.position = position, .normal = normal};

      // If hash doesn't contain this vertex
      if (!hash.contains(vertex)) {
        // Add this index (size of m_vertices)
        hash[vertex] = m_vertices.size();
        // Add this vertex
        m_vertices.push_back(vertex);
      }

      m_indices.push_back(hash[vertex]);
    }
  }

  Select::standardize();
  if (!m_hasNormals) {
    computeNormals();
  }
  createBuffers();
}

// Cria o EBO e VBO utilizados para imprimir os blocos
void Select::createBuffers() {
  // Delete previous buffers
  abcg::glDeleteBuffers(1, &m_EBO);
  abcg::glDeleteBuffers(1, &m_VBO);

  // VBO
  abcg::glGenBuffers(1, &m_VBO);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  abcg::glBufferData(GL_ARRAY_BUFFER,
                     sizeof(m_vertices.at(0)) * m_vertices.size(),
                     m_vertices.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // EBO
  abcg::glGenBuffers(1, &m_EBO);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
  abcg::glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     sizeof(m_indices.at(0)) * m_indices.size(),
                     m_indices.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Select::standardize() {
  // Center to origin and normalize largest bound to [-1, 1]

  // Get bounds
  glm::vec3 max(std::numeric_limits<float>::lowest());
  glm::vec3 min(std::numeric_limits<float>::max());
  for (auto const &vertex : m_vertices) {
    max = glm::max(max, vertex.position);
    min = glm::min(min, vertex.position);
  }

  // Center and scale
  auto const center{(min + max) / 2.0f};
  auto const scaling{0.5f / glm::length(max - min)};
  for (auto &vertex : m_vertices) {
    vertex.position = (vertex.position - center) * scaling;
  }
}

void Select::computeNormals() {
  // Clear previous vertex normals
  for (auto &vertex : m_vertices) {
    vertex.normal = glm::vec3(0.0f);
  }

  // Compute face normals
  for (auto const offset :
       iter::range(0UL, static_cast<unsigned long>(m_indices.size()), 3UL)) {
    // Get face vertices
    auto &a{m_vertices.at(m_indices.at(offset + 0))};
    auto &b{m_vertices.at(m_indices.at(offset + 1))};
    auto &c{m_vertices.at(m_indices.at(offset + 2))};

    // Compute normal
    auto const edge1{b.position - a.position};
    auto const edge2{c.position - b.position};
    auto const normal{glm::cross(edge1, edge2)};

    // Accumulate on vertices
    a.normal += normal;
    b.normal += normal;
    c.normal += normal;
  }

  // Normalize
  for (auto &vertex : m_vertices) {
    vertex.normal = glm::normalize(vertex.normal);
  }

  m_hasNormals = true;
}

void Select::onDestroy() {
  abcg::glDeleteBuffers(1, &m_EBO);
  abcg::glDeleteBuffers(1, &m_VBO);
  abcg::glDeleteVertexArrays(1, &m_VAO);
  abcg::glDeleteProgram(m_program);
}