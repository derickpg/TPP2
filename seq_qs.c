#include <stdio.h>
#include "mpi.h"


int compare(const void *a, const void *b)
{
	return (*(int *)a - *(int *)b);
}

main(int argc, char** argv)
{
    int my_rank;  /* Identificador do processo */
    int proc_n;   /* Número de processos */
    int source;   /* Identificador do proc.origem */
    int dest;     /* Identificador do proc. destino */
    int tag = 50; /* Tag para as mensagens */
    int tam_vet = 50, nums_vet = 1000; // tamanho do vetor inicial e range dos numeros aleatorios de ex 100 = 0~99
    int vetor[tam_vet];
    int i;
    double ti,tf;

    MPI_Status status; /* Status de retorno */

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &proc_n);

    if (my_rank == 0){  
        for (i = 0; i < tam_vet; i++)
            vetor[i] = rand() % nums_vet + 1;
        ti = MPI_Wtime();
        qsort(vetor, tam_vet, sizeof(int), compare);
		tf = MPI_Wtime();
		double total_time;
		total_time = tf - ti;
		printf("\n TEMPO TOTAL = %f \n", total_time);
    }
    MPI_Finalize();
    return 0;
}