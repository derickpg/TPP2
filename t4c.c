#include <stdio.h>
#include <math.h>
#include "mpi.h"


//Função de comparação QS
int compare(const void *a, const void *b)
{
	return (*(int *)a - *(int *)b);
}

main(int argc, char **argv)
{
    // Parametros
    int tam_vet = 50, nums_vet = 1000; // tamanho do vetor inicial e range dos numeros aleatorios de ex 100 = 0~99
    int tam_fixo = 25;

    // Ativa e Desativa o DEBUG
    int debug = 1;

    // Variaveis
    int id_proc, qtd_proc;
    int id_pai, id_filho_esq, id_filho_dir;
    int nivel[1];
    int i = 0;
    int tam_esq, tam_dir, tam_novo_vetor;
    int vetor[tam_vet];
    int recebido[tam_vet];
    int cont = 0;
    int i_esq = 0;
    int i_dir = 0;
    int espera = 0;
    int *vet_esq_env;
    int *vet_dir_env;
    int vet_aux_esq[tam_vet];
    int vet_aux_dir[tam_vet];

    // TAGS
    int t_vetor = 50;
    int t_morte = 99;
    int t_nivel = 1;
    int t_r_vetor = 25;


    MPI_Status status; /* Status de retorno */
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &id_proc);
    MPI_Comm_size(MPI_COMM_WORLD, &qtd_proc);

    if(debug == 1) printf("\nSou o Processo => %d!", id_proc);
    if (id_proc == 0) /* Raiz */ {

        nivel[0] = 1; //Raiz inicializa o nivel como um, e passa para os filhos
        for (i = 0; i < tam_vet; i++)
            vetor[i] = rand() % nums_vet + 1;

//array gerado embaralhado.
if(debug == 1) { int indice = 0; printf("\n Array Gerado na raiz: \n"); for (indice = 0; indice < tam_vet; indice++){ printf(" %d,", vetor[indice]); } printf("\n"); }

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
            MPI_Recv(recebido, (tam_vet/2), MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            if(status.MPI_TAG == t_r_vetor){
                    espera++;
                    if(status.MPI_SOURCE == id_filho_esq){
                        //vet_esq_env = recebido;
                        for(i = 0; i < tam_vet/2; i++)
                            vet_aux_esq[i] = recebido[i];
//aqui chega o vetor do filho esquerdo e ele esta chegando certinho, so que nao consegui copiar o conteudo usando memcpy nem for
if(debug == 1) { int indice = 0; printf("\n Array do filho ESQ voltou na raiz: \n"); for (indice = 0; indice < tam_vet/2; indice++){ printf(" %d,", recebido[indice]); } printf("\n"); }
// memcpy(vet_esq_env, recebido, sizeof(int)*(tam_vet/2));
// int indice = 0; for (indice = 0; indice < tam_vet/2; indice++) vet_esq_env[indice] = recebido[indice];
                        
                    }else{
                        //vet_dir_env = recebido;
                        for(i = 0; i < tam_vet/2; i++)
                            vet_aux_dir[i] = recebido[i];
//aqui chega o vetor do filho direito e ele esta chegando certinho, so que nao consegui copiar o conteudo usando memcpy nem for
if(debug == 1) { int indice = 0; printf("\n Array do filho DIR voltou na raiz: \n"); for (indice = 0; indice < tam_vet/2; indice++){ printf(" %d,", recebido[indice]); } printf("\n"); }
// memcpy(vet_dir_env, recebido, sizeof(int)*(tam_vet/2));
// int indice = 0; for (indice = 0; indice < tam_vet/2; indice++) vet_dir_env[indice] = recebido[indice];
                    }
                    if(espera == 2){
                        int vetor_final[tam_vet];
                        // ~~~ Processo de Interleaving ~~~
                        i_esq = 0;
                        i_dir = 0;
//aqui os dois estao apontados para a mesma memoria e o resultado eh os numeros duplicados
if(debug == 1) { int indice = 0; printf("\n Array do filho ESQ voltou na raiz: \n"); for (indice = 0; indice < tam_vet/2; indice++){ printf(" %d,", vet_aux_esq[indice]); } printf("\n"); }
if(debug == 1) { int indice = 0; printf("\n Array do filho DIR voltou na raiz: \n"); for (indice = 0; indice < tam_vet/2; indice++){ printf(" %d,", vet_aux_dir[indice]); } printf("\n"); }
                        for (i = 0; i < tam_vet; i++) {
                            if (((vet_aux_esq[i_esq] <= vet_aux_dir[i_dir]) && (i_esq < (tam_vet/2))) || (i_dir == (tam_vet/2)))
                                vetor_final[i] = vet_aux_esq[i_esq++];
                            else
                                vetor_final[i] = vet_aux_dir[i_dir++];
                        }
                        verificacao = 1;

// resultado final
if(debug == 1) { int indice = 0; printf("\n Array Retornou a raiz: \n"); for (indice = 0; indice < tam_vet; indice++){ printf(" %d,", vetor_final[indice]); } printf("\n"); }
                        


                        printf("\n\n ---------------- FIM -----------------------");
                    }
            }
        }
        if(debug == 1) printf("\n TUDO CERTO vou MATAR");
        // MSG da MORTE
        for(i = 0; i < qtd_proc; i ++){
            MPI_Send(0, 0, MPI_INT, i, t_morte, MPI_COMM_WORLD);
        }

    } else /* Nodos abaixo da raiz, varios niveis */ {
        int tam_vetor_n;
        int nivel_aux[1];
        int meu_nivel;
        int esperando_filho = 0;
        id_pai = (id_proc - 1) / 2;
        while(1){
            MPI_Recv(recebido, tam_vet, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            if(status.MPI_TAG == t_vetor){
                if(debug == 1) printf("\nChegou !!! %d e o tamanho do vetor => %d", id_proc, tam_vetor_n);
                // Verifica se deve ou nao ordenar 
                if(tam_vetor_n <= tam_fixo){ // entao ordena
                    if(debug == 1) printf("\n VOU ORDENAR! %d e mandar para o pai %d", id_proc, id_pai);
                    // Manda para o pai o vetor ordenado
                    int *vet_aux;
                    vet_aux = recebido;
                    qsort(vet_aux, tam_vetor_n, sizeof(int), compare);
if(debug == 1) { printf("\n NODO %d ORDENOU \n", id_proc); int indice = 0; for (indice = 0; indice < tam_vetor_n; indice++){ printf(" %d,", vet_aux[indice]); } }

                    MPI_Send(vet_aux, tam_vetor_n, MPI_INT, id_pai, t_r_vetor, MPI_COMM_WORLD);
                }else{// Manda para os filhos 
                    if(debug == 1) printf("\n Sou filho e vou mandar para os meus filhos!");
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
                if(debug == 1) printf("\n MORRI !!! %d", id_proc);
                MPI_Finalize();
                return 0;
            }else if(status.MPI_TAG == t_nivel){
                if(debug == 1) printf("RECEBIDO NO NIVEL = %d", recebido[0]);
                meu_nivel = recebido[0];
                tam_vetor_n = (tam_vet / (pow(2, (meu_nivel - 1))));
                if(debug == 1) printf("\n NIVEL CHEGOU !!! %d e tam %d", id_proc, tam_vetor_n);
            }else if(status.MPI_TAG == t_r_vetor){ // Aguardando o retorno do vetor
                esperando_filho++;
                if(status.MPI_SOURCE == id_filho_esq){
                    //vet_esq_env = recebido;
                    for(i = 0; i < tam_vetor_n; i++)
                        vet_aux_esq[i] = recebido[i];
                }else{
                    //vet_dir_env = recebido;
                    for(i = 0; i < tam_vetor_n; i++)
                        vet_aux_dir[i] = recebido[i];
                }
                // So se receber os dois faz o processo do interleaving e manda para o pai
                if(esperando_filho == 2){
                    int vetor_ord[tam_vetor_n];
                    // ~~~ Processo de Interleaving ~~~
                    i_esq = 0;
                    i_dir = 0;
                    for (i = 0; i < tam_vetor_n; i++)
                    {
                        if (((vet_aux_esq[i_esq] <= vet_aux_dir[i_dir]) && (i_esq < (tam_vetor_n/2))) || (i_dir == (tam_vetor_n/2)))
                            vetor_ord[i] = vet_aux_esq[i_esq++];
                        else
                            vetor_ord[i] = vet_aux_dir[i_dir++];
                    }
                    // ~~~ Fim do Interleaving ~~~
                    MPI_Send(vetor_ord, tam_vetor_n, MPI_INT, id_pai, t_r_vetor, MPI_COMM_WORLD);
                }
            }
        }
    }
    MPI_Finalize();
    
}
