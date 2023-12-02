# Memory

#### Nome: Gabriel de Sousa Dias
#### RA: 11201720272

#### [Link para Aplicação em WebAssembly](https://sdias22.github.io/CG-WebAssembly/Memory/)

## Descrição

Trata-se da implementação de um jogo da memória, no qual é implementado os conceitos da disciplina de Computação Gráfica. Onde o usuário pode percorrer o tabuleiro (8 x 8) composto por 64 cubos, onde são sorteados de forma aleatórias 8 cores para os cubos. 

O Usuário pode selecionar 2 cubos por vez e é verificado se são iguais.

Para finalização do jogo é necessário fazer a combinação de todos os cubos, assim não tendo nenhum cubo branco ou cinza.

## Composição dos Arquivos

```
memory/
│    background.cpp
│    background.hpp
|    cube.hpp
|    cube.cpp
|    ground.hpp
│    ground.cpp
|    main.cpp
|    select.cpp
|    select.hpp
|    vertex.hpp
|    window.cpp
|    window.hpp
└──  assets/
     │   background.frag
     │   background.vert  
     │   cube.frag
     │   cube.vert    
     │   cube.obj
     │   ground.frag    
     │   ground.vert
     |   select.obj
     └── background/
         |   negx.png
         |   negy.png
         |   negz.png
         |   posx.png
         |   posy.png
         └── posz.png
```

### Vertex Shader e Fragment Shader

#### background.vert

```
#version 300 es

layout(location = 0) in vec3 inPosition;

out vec3 fragTexCoord;

uniform mat4 viewMatrix;
uniform mat4 projMatrix;

void main() {
  fragTexCoord = inPosition;

  vec4 P = projMatrix * viewMatrix * vec4(inPosition, 1.0);
  gl_Position = P.xyww;
}
```

O atributo de entrada é a posição do vértice do cubo `(inPosition)` no qual é tratada junto com a matriz de posição e visão para que as faces estejam preenchendo o fundo da cena.

#### background.frag

```
#version 300 es

precision mediump float;

in vec3 fragTexCoord;

out vec4 outColor;

uniform samplerCube m_Tex;

void main() { outColor = texture(m_Tex, fragTexCoord); }
```

O fragment shader recebe a saída do vertex shader (fragTexCoord), que são as coordenadas utilizadas para amostrar o mapa de textura cúbido.

#### cube.vert

```
#version 300 es

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat3 normalMatrix;

uniform vec3 lightPosition;

out vec3 fragV;
out vec3 fragL;
out vec3 fragN;

void main() {
  vec3 P = (viewMatrix * modelMatrix * vec4(inPosition, 1.0)).xyz;
  vec3 N = normalMatrix * inNormal;
  vec3 L = lightPosition;

  fragL = L;
  fragV = -P;
  fragN = N;

  gl_Position = projMatrix * vec4(P, 1.0);
}
```

Neste Vertex Shader destaca-se `vec3 L = lightPosition;`, no qual é passado a posição da luz que tem uma posição fixa acima do tabuleiro.

#### cube.frag

```
#version 300 es

precision mediump float;

in vec3 fragN;
in vec3 fragL;
in vec3 fragV;

uniform vec4 color;

// Light properties
uniform vec4 Ia, Id, Is;

// Material properties
uniform vec4 Ka, Kd, Ks;

out vec4 outColor;

vec4 BlinnPhong(vec3 N, vec3 L, vec3 V) {
  N = normalize(N);
  L = normalize(L);

  // Compute lambertian term
  float lambertian = max(dot(N, L), 0.0);

  // Compute specular term
  float specular = 0.0;
  if (lambertian > 0.0) {
    V = normalize(V);
    vec3 H = normalize(L + V);
    float angle = max(dot(H, N), 0.0);
    specular = pow(angle, 30.0);
  }

  vec4 diffuseColor = Kd * Id * lambertian;
  vec4 specularColor = Ks * Is * specular;
  vec4 ambientColor = Ka * Ia;

  return ambientColor + diffuseColor + specularColor;
}

void main() {
  vec4 colorAt = color * BlinnPhong(fragN, fragL, fragV);
  if (gl_FrontFacing) {
    outColor = colorAt;
  } else {
    outColor = vec4(color.r*0.5, color.g *0.5, color.b *0.5, 1.0);
  }
}
```

Neste Fragment Shader é utilizada o modelo de BlinnPhong para fazer a iluminação e as cores do dos cubos, no qual é utilzada o expoente de brilho especular com 30.0 com intuito de deixar o centro mais iluminado e ao se deslocar de tals posição é reduzido o a iluminação.
Além disso destaca-se `vec4 colorAt = color * BlinnPhong(fragN, fragL, fragV);`, no qual utilizamos o BlinnPhong para iluminar um objeto colorido, pois o intuito é o objeto não refletir a sua própria cor e sim uma cor fixa definida no Ka(constante de reflexão ambiente). Em adicional, quando o objeto não está de frente para a o ponto de vista do usuário é reduzido pela metada a sua cor `outColor = vec4(color.r*0.5, color.g *0.5, color.b *0.5, 1.0);`.

#### ground.vert

```
#version 300 es

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat3 normalMatrix;

uniform vec3 lightPosition;

out vec3 fragV;
out vec3 fragL;
out vec3 fragN;

void main() {
  vec3 P = (viewMatrix * modelMatrix * vec4(inPosition, 1.0)).xyz;
  vec3 N = normalMatrix * inNormal;
  vec3 L = lightPosition;

  fragL = L;
  fragV = -P;
  fragN = N;

  gl_Position = projMatrix * vec4(P, 1.0);
}
```

Segue a mesma ideia utilizada no código Cube.vert citado acima.

#### ground.frag

```
#version 300 es

precision mediump float;

in vec3 fragN;
in vec3 fragL;
in vec3 fragV;

// Light properties
uniform vec4 Ia, Id, Is;

// Material properties
uniform vec4 Ka, Kd, Ks;

out vec4 outColor;

vec4 Phong(vec3 N, vec3 L, vec3 V) {
  N = normalize(N);
  L = normalize(L);

  // Compute lambertian term
  float lambertian = max(dot(N, L), 0.0);

  // Compute specular term
  float specular = 0.0;
  if (lambertian > 0.0) {
    vec3 R = reflect(-L, N);
    V = normalize(V);
    float angle = max(dot(R, V), 0.0);
    specular = pow(angle, 25.0f);
  }

  vec4 diffuseColor = Kd * Id * lambertian;
  vec4 specularColor = Ks * Is * specular;
  vec4 ambientColor = Ka * Ia;

  return ambientColor + diffuseColor + specularColor;
}

void main() {
  vec4 colorAt = Phong(fragN, fragL, fragV);
  if (gl_FrontFacing) {
    outColor = colorAt * 0.75;
  } else {
    outColor = vec4(colorAt.r*0.3, colorAt.g *0.3, colorAt.b *0.3, 1.0);
  }
}
```

Como destaque do ground.frag podemos citar que é utilizado o modelo de Phong para fazer a iluminação e as cores do tabuleiro. No qual é multiplicado por 0.75 para diminuir a intensidade da cor exibida.

### Background (Skybox)

#### Background.hpp

Como destaque temos a marcação das posições para montar o skybox, no qual é utilizado o valor base 3, para que cubo esteja alinhada o "chão" com o tabuleiro.

```
std::array<glm::vec3, 36> const m_Positions{{// Front
                                               {-3, -3, +3},
                                               {+3, -3, +3},
                                               {+3, +3, +3},
                                               {-3, -3, +3},
                                               {+3, +3, +3},
                                               {-3, +3, +3},
                                               // Back
                                               {+3, -3, -3},
                                               {-3, -3, -3},
                                               {-3, +3, -3},
                                               {+3, -3, -3},
                                               {-3, +3, -3},
                                               {+3, +3, -3},
                                               // Right
                                               {+3, -3, -3},
                                               {+3, +3, -3},
                                               {+3, +3, +3},
                                               {+3, -3, -3},
                                               {+3, +3, +3},
                                               {+3, -3, +3},
                                               // Left
                                               {-3, -3, +3},
                                               {-3, +3, +3},
                                               {-3, +3, -3},
                                               {-3, -3, +3},
                                               {-3, +3, -3},
                                               {-3, -3, -3},
                                               // Top
                                               {-3, +3, +3},
                                               {+3, +3, +3},
                                               {+3, +3, -3},
                                               {-3, +3, +3},
                                               {+3, +3, -3},
                                               {-3, +3, -3},
                                               // Bottom
                                               {-3, -3, -3},
                                               {+3, -3, -3},
                                               {+3, -3, +3},
                                               {-3, -3, -3},
                                               {+3, -3, +3},
                                               {-3, -3, +3}}};
```

#### Background.cpp

`onCreate:`Iniciado o Program com o Fragment e Vertex Shader, criando o VAO e o VBO.

`onPaint:` Responsável por realizar a renderização da SkyBox.

`onDestroy:` Deleta o VAO, VBO e o Program.

`loadTexture:` É carregado as imagens utilizadas para construir a SkyBox. 

### Ground

#### ground.hpp

Temos em relevância os atributos `m_Kd` e `m_Ks` responsáveis no código por refletir totalmente a luz difusa e a especular.
```
glm::vec4 m_Kd{1.0f, 1.0f, 1.0f, 1.0f};
glm::vec4 m_Ks{1.0f, 1.0f, 1.0f, 1.0f};
```

Também ressalta-se o atributo `m_light` que armazena é passada ao Shader a posição da luz.
```
glm::vec3 m_light{-0.05f, 1.0f, 0.0f};
```

#### ground.cpp

`onSetup:` Cria o Program, VAO e VBO. Além de salvar a localização das variávies uniformes do programa de renderização.

`onCreate:` Cria os vértices utilizados para criar os quadrados do tabuleiro, além de passar para o Shader o inPosition e o inNormal.

`onPaint:` É feito o desenho do tabuleiro utilizando um loop de tamanho N = 4 para geração de 64 quadrados. Destacando a utilizadação do Ka sendo alternado entre a cor branca e o cinza escuro:

```
glm::vec4 m_color{(z + x) % 2 == 0 ? white : darkGray};
abcg::glUniform4fv(m_KaLocation, 1, &m_color.x);
```

`onDestroy:` Deleta o VAO, VBO e o Program.

### Cube

#### Cube.hpp

Como atributos que temos em destaque:

`enum class StatusCube { on, off, select };` No qual determinamos os possíveis estados que os Cubos podem ter, onde:

* `on` exibe a cor do cubo com sua escala padrão, representando que já foi combinado com outro cubo.
* `off` exibe a cor cinza ou branca do cubo com a escala padrão, representando que o cubo ainda não foi combinado com outro.
* `select` exibe a cor do cubo onde sua escala fica variando para ter um movimento de "piscar", representando que o cubo foi selecionado e está aguardando a seleção de outro cubo.

Destacamos também a estrutura `Cubes` que armazena o status citado acima, a cor, posição e a escala padrão que o cubo deve ser desenhada.

```
  struct Cubes {
    StatusCube m_status;
    glm::vec4 m_color;
    glm::vec3 m_position;
    glm::vec3 m_scale{0.5f};
  };
```

`Cubes m_cubes[64];` que armazena todos as estruturas dos Cubos utilizados no código.

Em relação a propriedade dos materiais (Cubos) temos o objeto refletindo apenas metade da luz ambiente (Ka). Já o Kd é 0.0f para que não seja refletida a luz difusa. Por fim temos metade da luz especular refletida pelos cubos.
```
  glm::vec4 m_Ka{0.5f, 0.5f, 0.5f, 1.0f};
  glm::vec4 m_Kd{0.0f, 0.0f, 0.0f, 1.0f};
  glm::vec4 m_Ks{0.5f, 0.5f, 0.5f, 1.0f};
```

`glm::vec3 m_light{-0.05f, 1.0f, 0.0f};` armazena a posição da luz, que é fixa ao centro do tabuleiro.

`glm::vec3 m_pInicial{-0.8f, 0.075f, -0.8f};` armazena a posição inical utilizada para indicar aos cubos a posição que serão desenhadas no tabuleiro.

`glm::vec4 m_ColorList[64];` armazena todas as cores que serão utilizadas nos cubos. Serve de auxiliar para passar as cores de forma aleatória para os cubos.

#### Cube.cpp

`onSetup:` Cria o Program, VAO e VBO. Além de salvar a localização das variávies uniformes do programa de renderização.

`onCreate:` 

Cria a estrutura de cada cubo passando a posição que ele deve ter no tabuleiro utilizando um loop que utiliza o atributo de posição inicial somado a valor de i e j no loop. Além de inicar com o Status `on` para que o usuário possa memorizar no começo do jogo as cores, além de modificar o atributo `m_color` para cor neutra `black`.

```
  for (size_t i = 0; i < 8; i++) {
    for (size_t j = 0; j < 8; j++) {
      int p = i * 8 + j;
      float z = 0.2f * i;
      float x = 0.2f * j;
      m_cubes[p].m_position =
          glm::vec3((m_pInicial.x + x), m_pInicial.y, (m_pInicial.z + z));
      /*
        Inicializa o Status On para que o usuário posso memorizar no começo do
        jogo
      */
      m_cubes[p].m_status = StatusCube::on;
      // Inicializa com um cor neutra
      m_cubes[p].m_color = black;
    }
  }
```

Além disso, é feito de forma aleatória a seleção das cores que os cubos devem ter. Onde é percorrido o vetor m_ColorList[i], onde se tem um loop que é executado enquanto um cubo sorteado do vetor de Cubos (`m_cubes`) não tiver a cor `black`, quando encontrado um cubo com a cor neutra é alterado essa cor para a cor atual do m_ColorList. 

```
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
```

`onPaint:` Desenha os cubos, onde caso o estado do cubo seja off o cubo é colorido de cinza ou branco. Caso contrário é colorido com a própria cor do cubo:

```
if (m_cubes[i].m_status == StatusCube::off) {
      m_colorCurrent = ((i / 8) + i) % 2 == 0 ? gray : white;
      abcg::glUniform4fv(m_colorMatrixLocation, 1, &m_colorCurrent.x);
    } else {
      abcg::glUniform4fv(m_colorMatrixLocation, 1, &m_cubes[i].m_color.x);
    }
```

Além de ser atualizado os atributos dos Shaders:

```
    abcg::glUniform4fv(m_KaLocation, 1, &m_Ka.x);
    abcg::glUniform4fv(m_KdLocation, 1, &m_Kd.x);
    abcg::glUniform4fv(m_KsLocation, 1, &m_Ks.x);

    abcg::glUniform3f(m_lightPositionLocation, m_light.x, m_light.y, m_light.z);

    abcg::glUniform4fv(m_IaLocation, 1, &m_Ia.x);
    abcg::glUniform4fv(m_IdLocation, 1, &m_Id.x);
    abcg::glUniform4fv(m_IsLocation, 1, &m_Is.x);
```

`onUpdate:` Em cada atualização é esperado um tempo para que quando o cubo tenha sido selecionado é alterado a escala dando a impressão que o cubo está piscando (aumentando e diminuindo), caso contrário (estado on ou off) é mantido a escala padrão dos cubos.

```
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
```

`onState:` Altera para o estado off (oculta todos os cubos).

`onSelect:` Altera o estado do cubo passado para `select` caso o cubo tenha o estado diferente de off. Retorna `true`, caso tenha alterado o estado e `false` caso contrário.

`onDecision:` Faz a verificação se os 2 cubos selecionados são iguais. Após a verificação é alterado o status dos cubos de acordo com a descisão.

`onGameOver:` Verifica se o jogo acabou ou não. Para isso verifica se todos os cubos possuem o status `on`.

`loadObj:` Carrega os Vértices e Indíces do arquivo cubo.obj.

`createBuffers:` Cria o EBO e VBO.

`onDestroy:` Deleta o VAO, VBO, EBO e o Program.

`standardize:` Centraliza e faz a escala dos cubos, alterando o position.

`computeNormals:` Ajusta os Vértices e Indíces para a matriz normal.

### Select

#### Select.hpp

Como atributos que temos em destaque:

`enum class StatusSelect { moving, select };`No qual determinamos os possíveis estados que o Select podem ter, onde:
* `moving` o objeto está se movendo para selecionar um cubo.
* `select` um cubo foi selecionado.

`glm::vec3 m_positionCurrent{0.0f, 0.25f, 0.0f};` Armazena a posição atual da forma utilizada para seleção.

Destacamos também a estrutura `Selects` que armazena o status, cor e posição da forma de seleção.
```
  struct Selects {
    StatusSelect m_status;
    glm::vec4 m_color;
    glm::vec3 m_position;
  };
```

`glm::vec3 m_pInicial{0.0f, 0.30f, 0.0f};` Armazena a posição inicial (Utilizada na criação e reinicialização do jogo).

Em relação a propriedade dos materiais (Cubos) temos o objeto refletindo 3/4 da luz ambiente (Ka). Já o Kd é 0.0f para que não seja refletida a luz difusa. Por fim temos metade da luz especular refletida pelos cubos.
```
  glm::vec4 m_Ka{0.75f, 0.75f, 0.75f, 1.0f};
  glm::vec4 m_Kd{0.0f, 0.0f, 0.0f, 1.0f};
  glm::vec4 m_Ks{0.5f, 0.5f, 0.5f, 1.0f};
```

`glm::vec3 m_light{-0.05f, 1.0f, 0.0f};` armazena a posição da luz, que é fixa ao centro do tabuleiro.

#### Select.cpp

`onSetup:` Cria o Program, VAO e VBO. Além de salvar a localização das variávies uniformes do programa de renderização.

`onCreate:` Atualiza a cor, posição e status inicial do objeto.

`onPaint:` Desenha na tela a objeto de seleção, comportamento similar ao `Cube::onPaint`.

`onUpdate:` Quando o estado do select seja "moving", ou seja, se movendo para selecionar um cubo. O select é colorido de cinza ou branco fazendo com que o select fique piscando.

`onSelect:` Caso o status seja select (decidindo se os cubos escolhidas são iguais), é alterado o m_color do Selects para a cor amarela.

`onxMove:` É movimentado no eixo x / horizontal, dentro do intervalo de blocos, caso contrário não é feito nada.

`onzMove:` É movimentado no eixo y / vertical, dentro do intervalo de blocos, caso contrário não é feito nada.

`loadObj`, `createBuffers`, `standardize`, `computeNormals` e `onDestroy` são equivalentes ao citados acima no `cube.cpp`.

### Window

#### Window.hpp

Como atributos que temos em destaque:

`enum class GameStatus { start, playing, gameover };` Onde:
* `start` determina que o jogo está começando e deve exibir ao usuário os cubos com suas próprias cores, onde nenhum movimento é permitido.
* `playing` o usuário pode executar os movimentos do jogo.
* `gameover` o jogo está encerrado, sendo exibido uma mensagem para o usuário e reiniciado o jogo. 

`enum class PlayStatus { moving, select, decision };` Onde:
* `moving` o usuário consegue mover o objeto de seleção pelo tabuleiro.
* `select` no momento está sendo feito a seleção de um cubo.
* `decision` o jogo está parado aguardando ser verificado se os cubos selecionados são da mesma cor.

`glm::vec3 m_positionCurrent{0.0f, 0.25f, 0.0f};` Armazena a posição atual do objeto de seleção.

Criado a instância dos objetos que compõe o jogo.
```
Ground m_ground;
Background m_background;
Cube m_cube;
Select m_select;
PlayStatus m_play{PlayStatus::moving};
GameStatus m_game{GameStatus::playing};
```

`int tries{0};` Armazena a quantidade de tentativas feitas até finalizar o jogo.

#### Window.cpp

`onEvent:` Realiza a movimentação do objeto de seleção. Além de utilizar o `space` para selecionar os cubos.

`onCreate:` Limpa a tela e chama o `onCreate` das demais classe para criarem os objetos.

`onPaint:` Chama o `onPaint` das demais classe para desenharem os objetos.

`onUptade:` 
* Caso o GameStatus seja `gameover` não é feito nada;
* Caso o estado do game seja "start", após um intervalo de tempo, é oculto as cores dos cubos que estavam coloridos ealterado o estado do jogo para "playing", permitando que o usuário movimente e selecione os cubos;
* Caso o PlayStatus seja `select` é calculado a posição no vetor dado a posição do seletor e caso seja o 2° cubo selecionado é alterado o estado do jogo para "decision";
* Caso o PlayStatus seja `decision` função da `onDecision` da Classe Cube e limpo o vetor que armazena as posições selecionadas. Após isso é retornado o estado do game para "moving". Além de atualizar o contador de tentativas;
* Caso o PlayStatus seja `moving` é atualizado os cubos e o seletor.

`onPaintUI:` É criado o botão de restart, além de criar as mensagens que aparecem o usuário (Jogo Iniciando, Reiniciando, Fim do Jogo).

`onGame:` Verifica se o jogo acabou ou foi reiniciado utilizando o botão.

`onRestart:` Reinicia as variáveis e o jogo.

`onResize:` Atualiza as dimensões caso o tamanho da tela seja ajustado.

`onDestroy:` Chama as funções `onDestroy` das demais classes.

## Referência

Para execução do projeto foi utilizado a biblioteca [ABCG](https://github.com/hbatagelo/abcg).
