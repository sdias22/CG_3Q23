# Chessboard 3D
### Projeto de Computação Gráfica
### Nome: Gabriel de Sousa Dias
### RA: 11201720272
### [Link para Aplicação em WebAssembly](https://sdias22.github.io/CG-WebAssembly/ChessBoard3D/)
### [Link para o Vídeo](https://youtu.be/wystuZoPspU)
## Descrição
O projeto trata-se de exibir um tabuleiro de xadrez em 3D, no qual o usuário tem o controle para navegar pelo tabuleiro e poder visualizá-lo de várias posições.
### pieces.vert
```
#version 300 es
layout(location = 0) in vec3 inPosition;
uniform vec4 color;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
out vec4 fragColor;
void main() {
  vec4 posEyeSpace = viewMatrix * modelMatrix * vec4(inPosition, 1);
  fragColor = color;
  gl_Position = projMatrix * posEyeSpace;
}
```
Equivalente ao apresentado no exemplo de aula [lookat](https://hbatagelo.github.io/cg/lookatproject.html#lookat.vert), entretanto é alterado para que mantenha a cor independente da distância da câmera.
### pieces.frag
```
#version 300 es
precision mediump float;
in vec4 fragColor;
out vec4 outColor;
void main() {
  if (gl_FrontFacing) {
    outColor = fragColor;
  } else {
    outColor = fragColor - 0.3f;
  }
}
```
Segue em linha com o fragment shader apresentando em aula, sendo alterado apenas a última linha no qual é feita uma redução de 0.3 na intensidade original
### main.cpp
```
#include "window.hpp"
int main(int argc, char **argv) {
  try {
    abcg::Application app(argc, argv);
    Window window;
    window.setOpenGLSettings({.samples = 4});
    window.setWindowSettings({
        .width = 600,
        .height = 600,
        .title = "ChessBoard 3D",
    });
    app.run(window);
  } catch (std::exception const &exception) {
    fmt::print(stderr, "{}\n", exception.what());
    return -1;
  }
  return 0;
}
```
Segue o padrão, onde a única mudanã é o título da janela
### camera.hpp
Em relação ao código exemplificado em aula [camera.hpp](https://hbatagelo.github.io/cg/lookatproject.html#camera.hpp) é acrescentado as funções de up e rotate, que respectivamente, são utilizadas para alterar a altura da câmera e rotacionar/inclinar a câmera.
```
void up(float speed);
void rotate(float speed);
```
### camera.cpp
Segue as funções citadas acima:
```
void Camera::up(float speed) {
  m_at += m_up * speed;
  m_eye += m_up * speed;
  computeViewMatrix();
}
void Camera::rotate(float speed) {
  glm::mat4 transform{1.0f};
  // Rotate camera around its local z axis
  auto const forward{glm::normalize(m_at - m_eye)};
  transform = glm::rotate(transform, speed, forward);
  m_up = transform * glm::vec4(m_up, 0.0f);
  computeViewMatrix();
}
```
Onde é utilizado a variável m_up para alterar a posição m_at e m_eye, que são responsáveis, respectivamente, por controlar a posição para onde a câmera está olhando e a posição da câmera.
### ground.hpp
Equivalente ao utilizado em aula [ground.hpp](https://hbatagelo.github.io/cg/lookatproject.html#ground.hpp)
### ground.cpp
Em relação ao utilizado em aula, é alterado a quantidade de quadrados em `auto const N{4};`, e é feito a translação em `model = glm::translate(model, glm::vec3(x * 0.2f, -0.1f, z * 0.2f));` e a escala é reduzida para melhor visualização: `model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));`
```
void Ground::onPaint() {
  abcg::glBindVertexArray(m_VAO);
  // Draw a grid of 2N x 2N tiles on the xz plane, centered around the
  // origin
  auto const N{4};
  for (auto const z : iter::range(-N, N)) {
    for (auto const x : iter::range(-N, N)) {
      // Set model matrix as a translation matrix
      glm::mat4 model{1.0f};
      model = glm::translate(model, glm::vec3(x * 0.2f, -0.1f, z * 0.2f));
      model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
      abcg::glUniformMatrix4fv(m_modelMatrixLoc, 1, GL_FALSE, &model[0][0]);
      // Set color (checkerboard pattern)
      auto const whiteOrBlack{(z + x) % 2 == 0 ? 1.0f : 0.1f};
      abcg::glUniform4f(m_colorLoc, whiteOrBlack, whiteOrBlack, whiteOrBlack,
                        1.0f);
      abcg::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
  }
  abcg::glBindVertexArray(0);
}
```
### piece.hpp
```enum class Input { rei, rainha, bispo, cavalo, torre, peao };
struct Piece {
  Input m_input{Input::rei};
};
```
Essa primeira parte é responsável por criar um estrutura que armazena as possível peças do jogo de xadrez, utilizado para fazer algumas configurações de rotação e escala, além de buscar o arquivo .obj de cada peça.
```
void onCreate(GLuint program, Input input);
```
onCreate é responsável por chamar a função `loadObj` dado a peça e criar o VAO fazer o bind do VBO/EBO.
```
void onPaint(glm::vec3 posGround, glm::vec4 colorPiece, Input input);
```
onPaint é responsável por posicionar, rotacionar e ajustar a escala das peças no tabuleiro. Posteriormente realizar o desenho na tela das peças. Para isso recebe como argumentos posGround que é um vetor que passa quando deve ser transladado (usado para posicionar) no tabuleiro, o argumento colorPiece que passa a cor que deve ser utilizada para pintar a peça e por fim input que diz qual é a peça a ser desenhada.
```
void onDestroy();
```
onDestroy é responsável por deletar o VAO, VBO e EBO.
```
void loadObj(std::string_view path);
void createBuffers()
standardize()
```
Equivalente a apresentada nas aulas.
### piece.cpp
Na função onPaint gostaria de destacar o processo de rotacionar, alterar a escala e a etapa de translação para cada peça que é demonstrada abaixo:
```
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
  if (colorPiece == gray) {
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
  if (colorPiece == gray) {
    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 0, 1));
  }
  model = glm::scale(model, glm::vec3(0.3f));
}
```
## Referência
Para execução do projeto foi utilizado a biblioteca [ABCG](https://github.com/hbatagelo/abcg).
