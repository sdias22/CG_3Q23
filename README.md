# Projeto de Computação Gráfica

### Nome: Gabriel de Sousa Dias
### RA: 11201720272

## Descrição

O projeto consiste em renderizar formas selecionadas, que podem ser por pontos, linhas, triângulos ou quadrados, que, quando renderizados consecutivamente, formam o desenho selecionado, podendo ser um círculo, triângulo ou quadrado. As formas podem ser preenchidas com uma cor única ou um degradê de cores.

No centro da aplicação, as formas são renderizadas em um intervalo de milissegundos, que pode ser configurado pelo usuário. Ao alterar a forma de desenho ou a forma utilizada para preencher o desenho, a tela é limpa para permitir a renderização da nova forma.

Este projeto foi implementado com base em projetos das notas de aula, principalmente o Triângulo de Sierpinski e Triângulos Coloridos.

## Arquivos

O arquivo `main.cpp` contém o código para configurar a tela com as dimensões de 600 x 600 pixels e chamar a aplicação.

Os arquivos `window.hpp` e `window.cpp` são responsáveis por controlar comportamento da janela da aplicação, nele se encontram as funções e variáveis citadas abaixo. 

### Variáveis

No código, você encontrará as seguintes variáveis:

- `uniqueColors`: Um booleano que, se for verdadeiro, indica o uso de uma única cor para desenhar as formas; caso contrário, um degradê de cores é usado.

- `m_viewportSize`: Armazena o tamanho da tela.

- `m_timer`: Armazena o tempo em milissegundos utilizado como atraso entre a exibição das formas.

- `m_formaDesenho`: Armazena a forma de desenho selecionada.

- `m_forma`: Armazena a forma usada para preencher o desenho.

- `m_colors`: Contém as cores para criar o degradê nas formas.

- `m_uniqueColor`: Contém a cor única utilizada nas formas.

- `angle` e `radius`: Utilizados para o desenho de círculos.

- `m_P`: Identifica a posição onde as formas serão desenhadas.

Além disso, existem outras variáveis usadas para a renderização.

### Funções

- A função `onCreate`, onde shaders são configurados, o gerador randômico é inicializado e a tela é limpa com a cor preta.

- A função `onPaint` cria o viewport, introduz atrasos entre as formas exibidas e inicia o programa que renderiza os shaders. De acordo com a forma selecionada, os vértices são passados para renderizar a forma.

- A função `onPaintUI` constrói menus que incluem controles deslizantes para ajustar o atraso e um botão "Limpar" para apagar a tela. Também é possível selecionar a forma de desenho e a forma de preenchimento usando um ComboBox. A escolha de uma cor única ou um degradê é controlada por meio de uma caixa de seleção, e as opções de cores utiliza `ColorEdit3` que são alocadas em `m_colors`.

- A função `SetupModel` calcula, a cada quadro, a posição onde a forma selecionada será renderizada e envia vértices e cores para os buffers.

- A função `onResize` redefine o ângulo para o desenho de círculos e o tamanho de `viewportSize`, além de limpar a tela quando a tela for redimensionada.

- A função `onDestroy` exclui os buffers utilizados na renderização quando a aplicação é encerrada.

## Referência

Para execução do projeto foi utilizado a biblioteca [ABCG](https://github.com/hbatagelo/abcg).
