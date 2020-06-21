#include <stdio.h>
#include <math.h>
#include "mpi.h"

main(int argc, char **argv)
{
    // Parametros
    int tam_vet = 100, nums_vet = 10000; // o que eh nums_vet ?
    int tam_fixo = 25;

    // Variaveis
    int id_proc, qtd_proc;
    int id_pai, id_filho_esq, id_filho_dir;
    int nivel[1];
    int i = 0;
    int tam_esq, tam_dir, tam_novo_vetor;
    int vetor[tam_vet];
    int recebido[tam_vet];
    int cont = 0;

    // TAGS
    int t_vetor = 50;
    int t_morte = 99;
    int t_nivel = 1;
    int t_r_vetor = 25;

    MPI_Status status; /* Status de retorno */
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &id_proc);
    MPI_Comm_size(MPI_COMM_WORLD, &qtd_proc);

    printf("\nSou o Processo => %d!", id_proc);
    if (id_proc == 0) /* Raiz */ {

        nivel[0] = 1; //Raiz inicializa o nivel como um, e passa para os filhos
        for (i = 0; i < tam_vet; i++)
            vetor[i] = rand() % nums_vet + 1;

        //Por ser o raiz pular algumas etapas tipo calcular lados do array
        
        int vet_esq[tam_vet/2]; // 1000 / 2 = 500 0..499 500...999
        int vet_dir[tam_vet/2];
        
        for (i = 0; i < tam_vet; ++i) {
            if(i < tam_vet/2)
                vet_esq[i] = vetor[i];
            else {
                vet_dir[cont] = vetor[i];
                cont++;
            }
        }
        id_filho_esq = id_proc * 2 + 1;
        id_filho_dir = id_proc * 2 + 2;
        nivel[0] = 2;
        MPI_Send(nivel, tam_vet, MPI_INT, id_filho_esq, t_nivel, MPI_COMM_WORLD);
        MPI_Send(nivel, tam_vet, MPI_INT, id_filho_dir, t_nivel, MPI_COMM_WORLD);

        MPI_Send(vet_esq, (tam_vet/2), MPI_INT, id_filho_esq, t_vetor, MPI_COMM_WORLD);
        MPI_Send(vet_dir, (tam_vet/2), MPI_INT, id_filho_dir, t_vetor, MPI_COMM_WORLD);
        int verificacao = 0;
        while(verificacao == 0){
            MPI_Recv(recebido, tam_vet, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            if(status.MPI_TAG == t_r_vetor)
                verificacao = 1;
        }
        printf("\n TUDO CERTO vou MATAR");
        // MSG da MORTE
        for(i = 0; i < qtd_proc; i ++){
            MPI_Send(0, 0, MPI_INT, i, t_morte, MPI_COMM_WORLD);
        }

    } else /* Nodos abaixo da raiz, varios niveis */ {
        int tam_vetor_n;
        int nivel_aux[1];
        int meu_nivel;
        id_pai = (id_proc - 1) / 2;
        while(1){
            MPI_Recv(recebido, tam_vet, MPI_INT, id_pai, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            if(status.MPI_TAG == t_vetor){
                printf("\n Chegou !!! %d e o tamanho do vetor => %d", id_proc, tam_vetor_n);
                // Verifica se deve ou nao ordenar 
                if(tam_vetor_n <= tam_fixo){ // entao ordena
                    printf("\n VOU ORDENAR! %d", id_proc);
                    // Manda para o pai o vetor ordenado
                    // para teste so esta mandando um ~ok~
                    MPI_Send(0, 0, MPI_INT, id_pai, t_r_vetor, MPI_COMM_WORLD);
                }else{// Manda para os filhos 
                    printf("\n Sou filho e vou mandar para os meus filhos!");
                    int vet_esq_n[tam_vetor_n/2]; // 1000 / 2 = 500 0..499 500...999
                    int vet_dir_n[tam_vetor_n/2];
                    id_filho_esq = id_proc * 2 + 1;
                    id_filho_dir = id_proc * 2 + 2;
                    cont = 0;
                    for (i = 0; i < tam_vetor_n; ++i) {
                        if(i < tam_vetor_n/2)
                            vet_esq_n[i] = recebido[i];
                        else {
                            vet_dir_n[cont] = recebido[i];
                            cont++;
                        }
                    }
                    nivel_aux[0] = meu_nivel + 1;
                    MPI_Send(nivel_aux, tam_vet, MPI_INT, id_filho_esq, t_nivel, MPI_COMM_WORLD);
                    MPI_Send(nivel_aux, tam_vet, MPI_INT, id_filho_dir, t_nivel, MPI_COMM_WORLD);


                    MPI_Send(vet_esq_n, (tam_vetor_n/2), MPI_INT, id_filho_esq, t_vetor, MPI_COMM_WORLD);
                    MPI_Send(vet_dir_n, (tam_vetor_n/2), MPI_INT, id_filho_dir, t_vetor, MPI_COMM_WORLD);

                }
            }else if(status.MPI_TAG == t_morte){
                printf("\n MORRI !!! %d", id_proc);
                MPI_Finalize();
                return 0;
            }else if(status.MPI_TAG == t_nivel){
                printf("RECEBIDO NO NIVEL = %d", recebido[0]);
                meu_nivel = recebido[0];
                tam_vetor_n = (tam_vet / (pow(2, (recebido[0] - 1))));
                printf("\n NIVEL CHEGOU !!! %d e tam %d", id_proc, tam_vetor_n);
            }else if(status.MPI_TAG == t_r_vetor){ // Aguardando o retorno do vetor
                // Apenas teste recebendo o ~ok~ do filho
                // Manda para o pai o retorno do ~ok~ 
                MPI_Send(0, 0, MPI_INT, id_pai, t_r_vetor, MPI_COMM_WORLD);

            }
        }
    }
    MPI_Finalize();
}