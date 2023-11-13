#include "pieces.hpp"

#include <unordered_map>

// Explicit specialization of std::hash for Vertex
template <> struct std::hash<Vertex> {
  size_t operator()(Vertex const &vertex) const noexcept {
    auto const h1{std::hash<glm::vec3>()(vertex.position)};
    return h1;
  }
};

void Pieces::onCreate(GLuint program, Input input) {
  auto const assetsPath{abcg::Application::getAssetsPath()};

  if (input == Input::rei) {
    loadObj(assetsPath + "rei.obj");
  }
  if (input == Input::rainha) {
    loadObj(assetsPath + "rainha.obj");
  }
  if (input == Input::bispo) {
    loadObj(assetsPath + "bispo.obj");
  }
  if (input == Input::cavalo) {
    loadObj(assetsPath + "cavalo.obj");
  }
  if (input == Input::torre) {
    loadObj(assetsPath + "torre.obj");
  }
  if (input == Input::peao) {
    loadObj(assetsPath + "peao.obj");
  }

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
      abcg::glGetAttribLocation(program, "inPosition")};

  if (positionAttribute >= 0) {
    abcg::glEnableVertexAttribArray(positionAttribute);
    abcg::glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE,
                                sizeof(Vertex), nullptr);
  }

  // Get location of uniform variables
  m_viewMatrixLocation = abcg::glGetUniformLocation(program, "viewMatrix");
  m_projMatrixLocation = abcg::glGetUniformLocation(program, "projMatrix");
  m_modelMatrixLocation = abcg::glGetUniformLocation(program, "modelMatrix");
  m_colorLocation = abcg::glGetUniformLocation(program, "color");

  // End of binding
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
  abcg::glBindVertexArray(0);
}

void Pieces::onPaint(glm::vec3 posGround, glm::vec4 colorPiece, Input input) {
  abcg::glBindVertexArray(m_VAO);

  glm::mat4 model{1.0f};
  model = glm::translate(model, posGround);

  if (input == Input::rei) {
    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
    model = glm::scale(model, glm::vec3(0.6f));
  }
  if (input == Input::rainha) {
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
    model = glm::scale(model, glm::vec3(0.5f));
  }
  if (input == Input::bispo) {
    model = glm::rotate(model, glm::radians(270.0f), glm::vec3(1, 0, 0));
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 0, 1));
    model = glm::scale(model, glm::vec3(0.5f));
  }
  if (input == Input::cavalo) {
    model = glm::rotate(model, glm::radians(270.0f), glm::vec3(1, 0, 0));
    if (colorPiece == white) {
      model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 0, 1));
    }
    model = glm::scale(model, glm::vec3(0.5f));
  }
  if (input == Input::torre) {
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
    model = glm::scale(model, glm::vec3(0.5f));
  }
  if (input == Input::peao) {
    model = glm::rotate(model, glm::radians(270.0f), glm::vec3(1, 0, 0));
    if (colorPiece == white) {
      model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 0, 1));
    }
    model = glm::scale(model, glm::vec3(0.3f));
  }

  abcg::glUniformMatrix4fv(m_modelMatrixLocation, 1, GL_FALSE, &model[0][0]);
  abcg::glUniform4f(m_colorLocation, colorPiece.x, colorPiece.y, colorPiece.z,
                    colorPiece.a);
  abcg::glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT,
                       nullptr);
  abcg::glBindVertexArray(0);
}

void Pieces::onUpdate() {}

void Pieces::onDestroy() {
  abcg::glDeleteBuffers(1, &m_EBO);
  abcg::glDeleteBuffers(1, &m_VBO);
  abcg::glDeleteVertexArrays(1, &m_VAO);
}

void Pieces::horizontalmove() {}

void Pieces::verticalmove() {}

void Pieces::createBuffers() {
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

void Pieces::loadObj(std::string_view path) { //, bool standardize) {

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
      auto const vx{attrib.vertices.at(startIndex + 0)};
      auto const vy{attrib.vertices.at(startIndex + 1)};
      auto const vz{attrib.vertices.at(startIndex + 2)};

      Vertex const vertex{.position = {vx, vy, vz}};

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

  Pieces::standardize();
  createBuffers();
}

void Pieces::standardize() {
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