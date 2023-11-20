#include "cube.hpp"

#include <unordered_map>

// Explicit specialization of std::hash for Vertex
template <> struct std::hash<Vertex> {
  size_t operator()(Vertex const &vertex) const noexcept {
    auto const h1{std::hash<glm::vec3>()(vertex.position)};
    return h1;
  }
};

void Cube::onCreate(GLuint program) {
  auto const assetsPath{abcg::Application::getAssetsPath()};
  loadObj(assetsPath + "cube.obj");

  for (size_t i = 0; i < 8; i++) {
    m_ColorList[i] = pink;
  }

  for (size_t i = 8; i < 16; i++) {
    m_ColorList[i] = yellow;
  }
  for (size_t i = 16; i < 24; i++) {
    m_ColorList[i] = red;
  }
  for (size_t i = 24; i < 32; i++) {
    m_ColorList[i] = blue;
  }
  for (size_t i = 32; i < 40; i++) {
    m_ColorList[i] = green;
  }
  for (size_t i = 40; i < 48; i++) {
    m_ColorList[i] = orange;
  }
  for (size_t i = 48; i < 56; i++) {
    m_ColorList[i] = purple;
  }
  for (size_t i = 56; i < 64; i++) {
    m_ColorList[i] = brown;
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

  for (size_t i = 0; i < 8; i++) {
    for (size_t j = 0; j < 8; j++) {
      int p = i * 8 + j;
      float z = 0.2f * i;
      float x = 0.2f * j;
      m_cubes[p].m_status = StatusCube::on;
      m_cubes[p].m_position =
          glm::vec3((m_pInicial.x + x), m_pInicial.y, (m_pInicial.z + z));
      m_cubes[p].m_color = black;
    }
  }

  m_randomEngine.seed(
      std::chrono::steady_clock::now().time_since_epoch().count());

  std::uniform_int_distribution<int> rd_cubeColor(0, 63);

  for (size_t i = 0; i < 64; i++) {
    m_colorEmpty = false;
    while (m_colorEmpty == false) {
      int pos = rd_cubeColor(m_randomEngine);
      if (m_cubes[pos].m_color == black) {
        m_cubes[pos].m_color = m_ColorList[i];
        m_colorEmpty = true;
      }
    }
  }
}

void Cube::onPaint() {

  for (size_t i = 0; i < 64; i++) {
    abcg::glBindVertexArray(m_VAO);

    glm::mat4 model{1.0f};

    model = glm::translate(model, m_cubes[i].m_position);
    model = glm::scale(model, m_cubes[i].m_scale);

    abcg::glUniformMatrix4fv(m_modelMatrixLocation, 1, GL_FALSE, &model[0][0]);
    if (m_cubes[i].m_status == StatusCube::off) {
      m_colorCurrent = ((i / 8) + i) % 2 == 0 ? gray : white;

      abcg::glUniform4f(m_colorLocation, m_colorCurrent.x, m_colorCurrent.y,
                        m_colorCurrent.z, m_colorCurrent.a);
    } else {
      abcg::glUniform4f(m_colorLocation, m_cubes[i].m_color.x,
                        m_cubes[i].m_color.y, m_cubes[i].m_color.z,
                        m_cubes[i].m_color.a);
    }
    abcg::glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT,
                         nullptr);
    abcg::glBindVertexArray(0);
  }
}

void Cube::onUpdate() {
  if (m_timer.elapsed() < m_tempo)
    return;
  m_timer.restart();

  for (size_t i = 0; i < 64; i++) {
    if (m_cubes[i].m_status == StatusCube::select) {
      if (m_cubes[i].m_scale.y > 0.45f) {
        m_cubes[i].m_scale = glm::vec3(0.5f, 0.25f, 0.5f);
      } else {
        m_cubes[i].m_scale = glm::vec3(0.5f);
      }
    } else {
      m_cubes[i].m_scale = glm::vec3(0.5f, 0.25f, 0.5f);
    }
  }
}

void Cube::onState() {
  for (size_t i = 0; i < 64; i++) {
    m_cubes[i].m_status = StatusCube::off;
  }
}

bool Cube::onSelect(int pos) {
  if (m_cubes[pos].m_status != StatusCube::off) {
    return false;
  } else {
    m_cubes[pos].m_status = StatusCube::select;
    return true;
  }
}

void Cube::onDecision(int pos1, int pos2) {
  if ((m_cubes[pos1].m_status == StatusCube::select &&
       m_cubes[pos2].m_status == StatusCube::select) &&
      (m_cubes[pos1].m_color == m_cubes[pos2].m_color)) {
    m_cubes[pos1].m_status = StatusCube::on;
    m_cubes[pos2].m_status = StatusCube::on;
  } else {
    m_cubes[pos1].m_status = StatusCube::off;
    m_cubes[pos2].m_status = StatusCube::off;
  }
}

bool Cube::onGameOver() {
  for (size_t i = 0; i < 64; i++) {
    if (m_cubes[i].m_status != StatusCube::on) {
      return false;
    }
  }

  return true;
}

void Cube::loadObj(std::string_view path) {

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

  Cube::standardize();
  createBuffers();
}

void Cube::createBuffers() {
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

void Cube::standardize() {
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

void Cube::onDestroy() {
  abcg::glDeleteBuffers(1, &m_EBO);
  abcg::glDeleteBuffers(1, &m_VBO);
  abcg::glDeleteVertexArrays(1, &m_VAO);
}