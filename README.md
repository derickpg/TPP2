# TPP2 - Divisao e Conquista
## Trabalho 4 da Cadeira de Programação Paralela e Distribuida ( Turma 590 )
### Graduação em Ciência da Computação
### Alunos: Derick Garcez e Vinicius Azevedo


O objetivo do trabalho é implementar, usando a biblioteca MPI, uma versão paralela seguindo o modelo divisão e conquista, de um programa que ordena um grande vetor usando o algortimo Bubble Sort (o programa sequencial está no final desta especificação) e qsort. Após implementado, o programa deve ser executado no cluster GRAD com até 2 nós para realização das medições de desempenho para um vetor de 1.000.000 de elementos . Deve ser feita também uma versão com um melhor balanceamento de carga (ver artigo no Moodle). Cada grupo (de no máximo dois integrantes) deve entregar um relatório em .pdf de uma página com a análise dos resultados e uma página com o código (seguir modelo proposto).


-> Código fonte executável da Versão Divisão e Conquista: CodigoFinal.c \n
-> Código Fonte dos Executáveis Sequenciais:
                      - sq_qs.c
                      - bubble_sort.c
-> Demais Códigos são exeplos, ou versões anteriores do código final, inclusive o código t4c.c é o código final mas sem os comentários e DEBUG de implementação.
