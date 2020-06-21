#include <stdio.h>
#include "mpi.h"

main(int argc, char **argv)
{
    int id_proc, qtd_proc;
    int id_pai, id_filho_esq, id_filho_dir;
    int nivel[1];
    int i = 0;
    int tam_vet = 100, nums_vet = 10000; // o que eh nums_vet ?
    int tam_esq, tam_dir, tam_novo_vetor;
    int vetor[tam_vet];
    // TAGS
    int t_vetor = 50;
    int t_morte = 99;
    int t_nivel = 1;

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
        
        int cont = 0, i;
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


        // MSG da MORTE
        for(i = 0; i < qtd_proc; i ++){
            MPI_Send(0, 0, MPI_INT, i, t_morte, MPI_COMM_WORLD);
        }

    } else /* Nodos abaixo da raiz, varios niveis */ {
        id_pai = (id_proc - 1) / 2;
        while(1){
            MPI_Recv(vetor, 500, MPI_INT, id_pai, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            if(status.MPI_TAG == t_vetor){
                printf("\n Chegou !!! %d", id_proc);

            }else if(status.MPI_TAG == t_morte){
                printf("\n MORRI !!! %d", id_proc);
                MPI_Finalize();
                return 0;
            }else if(status.MPI_TAG == t_nivel){
                printf("\n NIVEL CHEGOU !!! %d", id_proc);
            }
        }
    }
    MPI_Finalize();
}