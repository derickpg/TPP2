#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"

#define TAM 1000
#define VALOR_FIXO 300

//Função de comparação QS
int compare(const void *a, const void *b)
{
	return (*(int *)a - *(int *)b);
}

int *divideVetor(int vetor[], int ini, int fim)
{
	int tam_aux = fim - ini;
	int *retorno = malloc((tam_aux) * sizeof(int));

	int cont = 0, i;
	for (i = ini; i < fim; ++i)
	{
		retorno[cont] = vetor[i];
		cont++;
	}

	return retorno;
}

int *interleaving(int a[], int b[], int tam_a, int tam_b)
{
	int *vetor_auxiliar = malloc(((tam_a + tam_b)) * sizeof(int));
	int i1, i2, i_aux;

	i1 = 0;
	i2 = 0;

	for (i_aux = 0; i_aux < (tam_a + tam_b); i_aux++)
	{
		if (((a[i1] <= b[i2]) && (i1 < tam_a)) || (i2 == tam_b))
			vetor_auxiliar[i_aux] = a[i1++];
		else
			vetor_auxiliar[i_aux] = b[i2++];
	}

	return vetor_auxiliar;
}

int main(int argc, char *argv[])
{
	printf("=-=-= COMECOU O ALG DIVISAO E CONQUISTA =-=-=");
	int id, n, message, aux;
	int i,j,tam_vector;
	int nivel = 1; // Nivel da Arvore
	double ti, tf; // Tempos
	//Dados da Matriz
	//int coluna = 1000; // Quantidade de Vetores
	//int linha  = 100000; // Quantidade de Elementos
	int ord = 1; // 1 == QS ; 0 == BS
	printf("\n ♥♥♥ 1");
	MPI_Status status;
	printf("\n ♥♥♥ 2");
	MPI_Init(&argc, &argv);				// funcao que inicializa o MPI, todo o codigo paralelo estah abaixo
	MPI_Comm_rank(MPI_COMM_WORLD, &id); // pega pega o numero do processo atual (rank)
	MPI_Comm_size(MPI_COMM_WORLD, &n);	// pega informacao do numero de processos (quantidade total)
	printf("\n ♥♥♥ 3 %d", id);
	if (id == 0)
	{ // RAIZ
		ord = 1;
		printf("\n Começo da Ordenação! Ordenação do tipo: %d", ord);

		int *vetor = malloc(TAM * sizeof(int));
		// Iniciando o VETOR NÃO DEVE CONTAR COMO TEMPO DE ORDENAÇÃO
		for (i = 0; i < TAM; i++)
		{
			aux = rand() % 100;
			vetor[i] = aux;
		}
		ti = MPI_Wtime();

		if (TAM <= VALOR_FIXO)
		{ // Ordena
			qsort(vetor, TAM, sizeof(int), compare);
		}
		else
		{ // DIVIDE
			int *vetor_esq = divideVetor(vetor, 0, TAM / 2);
			int *vetor_dir = divideVetor(vetor, TAM / 2, TAM);

			nivel = nivel + 1;
			// Envio Mensagem para os Filhos
			MPI_Send(nivel, 1, MPI_INT, (id * 2 + 1), 2, MPI_COMM_WORLD);			  // FILHO ESQUERDA
			MPI_Send(nivel, 1, MPI_INT, (id * 2 + 2), 2, MPI_COMM_WORLD);			  // FILHO DIREITA
			MPI_Send(vetor_esq, (TAM / 2), MPI_INT, (id * 2 + 1), 1, MPI_COMM_WORLD); // FILHO ESQUERDA
			MPI_Send(vetor_dir, (TAM / 2), MPI_INT, (id * 2 + 2), 1, MPI_COMM_WORLD); // FILHO DIREITA

			// Recebimento das respostas
			MPI_Recv(vetor_esq, (TAM / 2), MPI_INT, (id * 2 + 1), 1, MPI_COMM_WORLD, &status); // FILHO ESQUERDA
			MPI_Recv(vetor_dir, (TAM / 2), MPI_INT, (id * 2 + 2), 1, MPI_COMM_WORLD, &status); // FILHO DIREITA

			// UNIAO DOS VETORES...

			vetor = interleaving(vetor_esq, vetor_dir, (TAM / 2), (TAM / 2));
		}
	}
	else
	{ // FILHOS

		int pai = ((id - 1) / 2);
		MPI_Recv(nivel, 1, MPI_INT, pai, 2, MPI_COMM_WORLD, &status); // Recebe do PAI o NIVEL
		int tam_novo_vetor = (TAM / (pow(2, (nivel - 1))));
		int *novo_vetor;
		MPI_Recv(novo_vetor, tam_novo_vetor, MPI_INT, pai, 1, MPI_COMM_WORLD, &status); // Recebe do PAI o VETOR
		if (tam_novo_vetor <= VALOR_FIXO)
		{
			// ORDENA
			qsort(novo_vetor, tam_novo_vetor, sizeof(int), compare);
			// Devolve para o PAI
			MPI_Send(novo_vetor, tam_novo_vetor, MPI_INT, pai, 1, MPI_COMM_WORLD);
		}
		else
		{ // DIVIDE

			int *vetor_esq = divideVetor(novo_vetor, 0, tam_novo_vetor / 2);
			int *vetor_dir = divideVetor(novo_vetor, tam_novo_vetor / 2, tam_novo_vetor);

			nivel = nivel + 1;
			// Envio Mensagem para os Filhos
			MPI_Send(nivel, 1, MPI_INT, (id * 2 + 1), 2, MPI_COMM_WORLD);						 // FILHO ESQUERDA
			MPI_Send(nivel, 1, MPI_INT, (id * 2 + 2), 2, MPI_COMM_WORLD);						 // FILHO DIREITA
			MPI_Send(vetor_esq, (tam_novo_vetor / 2), MPI_INT, (id * 2 + 1), 1, MPI_COMM_WORLD); // FILHO ESQUERDA
			MPI_Send(vetor_dir, (tam_novo_vetor / 2), MPI_INT, (id * 2 + 2), 1, MPI_COMM_WORLD); // FILHO DIREITA

			// Recebimento das respostas
			MPI_Recv(vetor_esq, (tam_novo_vetor / 2), MPI_INT, (id * 2 + 1), 1, MPI_COMM_WORLD, &status); // FILHO ESQUERDA
			MPI_Recv(vetor_dir, (tam_novo_vetor / 2), MPI_INT, (id * 2 + 2), 1, MPI_COMM_WORLD, &status); // FILHO DIREITA

			// UNIAO DOS VETORES...

			novo_vetor = interleaving(vetor_esq, vetor_dir, tam_novo_vetor / 2, tam_novo_vetor / 2);

			// Devolve para o PAI
			MPI_Send(novo_vetor, tam_novo_vetor, MPI_INT, pai, 1, MPI_COMM_WORLD);
		}
	}
	printf("\n ♥♥♥ 4");
	if (id == 0)
	{
		tf = MPI_Wtime();
		double total_time;
		total_time = tf - ti;
		printf("\n TEMPO TOTAL = %f \n", total_time);
	}
	printf("\n ♥♥♥ 5");
	MPI_Finalize();

	printf("=-=-= TERMINOU O ALG DIVISAO E CONQUISTA =-=-=");
	return 0;
}