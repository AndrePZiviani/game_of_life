 The Game of Life

 * uma célula nasce se tem examente 3 vizinhos vivos 
 * uma célula morre de solidão se tiver menos de 2 vizinhos vivos
 * uma célula morre de overcrowding, se tem mais de 3 vizinhos vivos 
 * uma célula continua viva se não morre de solidão ou de overcrowding

Compilação:
    $ make

Usage:
    $ gol <linhas> <colunas> <iterações> <output file> [semente]

    O jogo gera uma matriz inicial aleatória com as dimensões especificadas nos
    parâmetros e a semente especificada, se houver. Executa o número de
    iterações especificado e a matriz final é impressa em <output_file>, com
    "x" nas células vivas e " " nas células mortas.


** run-inputs.sh
    Executa o programa com 6 matrizes de dimensões e sementes predefinidas,
    imprime as matrizes finais nos arquivos teste-{0-5}.out e na tela os stats
    de tempo de cada execução.

Implementação:
    2 matrizes 2D são usadas, uma para o estado anterior e outra para o atual.
    Uma célula contendo 1 está viva, uma contendo 0 está morta.
