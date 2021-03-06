#include <stdio.h>
#include <time.h>

int compare(const void *a, const void *b)
{
	return (*(int *)a - *(int *)b);
}

int main(int argc, char** argv)
{

    int tam_vet = 2000;
    int vetor[tam_vet];
    int i,j;

    for (i=0 ; i<tam_vet; i++)
        vetor[i] = tam_vet-i;
    
    clock_t begin = clock();

    qsort(vetor, tam_vet, sizeof(int), compare);
        
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

    //for (i=0 ; i<tam_vet; i++)              //verifica se ta certo
    //    printf("%d,", vetor[i]);

    printf("\n TEMPO TOTAL = %f \n", time_spent);

    return 0;
}