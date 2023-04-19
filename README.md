# Implementação de uma memória cache
Este é um projeto desenvolvido para a disciplina de Arquitetura e Organização de Computadores II pelo grupo composto por Marcelo Rezende e Ricardo Cordeiro, ambos da turma M2, do curso de Ciência da Computação da Universidade Federal de Pelotas (UFPel). A implementação consiste em um simulador de cache, desenvolvido em linguagem de programação C.

Execução

Para executar a aplicação via terminal, basta passar o nome da aplicação, número de conjuntos, tamanho do bloco, nível de associatividade, política de substituição (no caso, a única disponível é a Random), flag de saída e o arquivo de entrada. O arquivo de entrada deve estar na mesma pasta que o executável do programa.

cache_simulator <nsets> <bsize> <assoc> <substituição> <flag_saida> arquivo_de_entrada
Para compilar o código corretamente, é necessário adicionar o argumento "-lm" para o comando do gcc ( isso é necessário por causa da biblioteca "math.h" ):

gcc -Wall -Wextra -g -o cache_simulator cache_simulator.c -lm

Formato de saída

A saída do simulador será um relatório de estatísticas com o número total de acessos, número total hits e misses (os misses deverão ser classificados em compulsórios, capacidade e conflito), que poderá ser exibido em dois formatos, de acordo com o valor do campo flag_saida:

flag = 0;
![image](https://user-images.githubusercontent.com/90624765/232940389-775c78a9-fd4a-4390-9d62-861d82d9ef0a.png)

flag = 1.
![image](https://user-images.githubusercontent.com/90624765/232940444-3419e9a3-9651-49d3-ae61-77a326e566bc.png)

Bibliotecas utilizadas

Foram utilizadas as seguintes bibliotecas:

stdio.h: biblioteca padrão do C que fornece funções para entrada e saída de dados através de streams (fluxos) padrão, como o teclado e a tela;
stdlib.h: biblioteca padrão do C que fornece funções para alocação de memória dinâmica, conversão de tipos, manipulação de strings, controle de processos e outras funcionalidades relacionadas à gerenciamento de memória e ao ambiente de execução do programa;
math.h: utilizada para cálculo de potência e logaritmos;
time.h: utilizada para gerar números aleatórios.

Adicionalmente, foi utilizado a biblioteca byteswap.h. No programa, a linha que inclui essa biblioteca no programa está comentada. Este programa foi escrito em uma máquina linux, e por causa da "glibc" ( que inclui o "byteswap.h" ), não houve problemas ao compilar o código. Porém, em uma máquina windows, o compilador "mingw" não inclui essa biblioteca. O programa inclui soluções para este problema ( basicamente pegando a parte da biblioteca que lida com a inversão de bytes de um endereço de 32 bits especificamente ) e deve ser capaz de compilar em qualquer máquina. Porém, se o programa for compilado em uma máquina linux, utilizar a biblioteca pode ser mais conveniente.

Para usar esta biblioteca ( apenas linux ): descomentar a linha 5 e 130 e comentar a linha 133
