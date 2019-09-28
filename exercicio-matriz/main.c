#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Argumentos insuficientes, adicione o tamanho da matriz\n");
        exit(1);
    }

    srand(time(NULL));

    int tam = atoi(argv[1]);
    int status;

    struct timespec inic, fim;
    float etime;

    int **A = malloc(sizeof(int *) * tam);
    int **B = malloc(sizeof(int *) * tam);
    int **C = malloc(sizeof(int *) * tam);

    for (int i = 0; i < tam; i ++)
    {
        A[i] = malloc(sizeof(int) * tam);
        B[i] = malloc(sizeof(int) * tam);
        C[i] = malloc(sizeof(int) * tam);
    }

    for (int i = 0; i < tam; i++)
    {
        for (int j = 0; j < tam; j++)
        {
            A[i][j] = 1;//rand() % 100;
            B[i][j] = 1;//rand() % 100;
        }
    }

    if (clock_gettime(CLOCK_REALTIME, &inic) < 0)
        printf("Erro em clock_gettime");

    // Processos P1 e P3
    if (fork() > 0) 
    {
        // Processo P1
        if (fork() > 0)
        {
            for (int i = 0; i < tam/2; i++)
            {
                for (int j = 0; j < tam/2; j++)
                {
                    int soma = 0;
                    for (int k = 0; k < tam; k++)
                    {
                        soma += A[i][k] * B[k][j];
                    }
                    C[i][j] = soma;
                }
            }
            wait(&status);
            // Verifica o instante atual logo depois do bloco de código sendo medido
            if (clock_gettime(CLOCK_REALTIME, &fim) < 0)
            printf("Erro em clock_gettime");

            // tempo decorrido: elapsed time
            etime = (fim.tv_sec + fim.tv_nsec/1000000000.) - (inic.tv_sec + inic.tv_nsec/1000000000.) ;
            printf("Tempo de execução: %lf\n", etime);
        }
        // Processo P3
        else
        {
            for (int i = tam/2; i < tam; i++)
            {
                for (int j = 0; j < tam/2; j++)
                {
                    int soma = 0;
                    for (int k = 0; k < tam; k++)
                    {
                        soma += A[i][k] * B[k][j];
                    }
                    C[i][j] = soma;
                }
            }
        }
    }
    // Processos P2 e P4
    else
    {
        // Processo P2
        if (fork() > 0)
        {
            for (int i = 0; i < tam/2; i++)
            {
                for (int j = tam/2; j < tam; j++)
                {
                    int soma = 0;
                    for (int k = 0; k < tam; k++)
                    {
                        soma += A[i][k] * B[k][j];
                    }
                    C[i][j] = soma;
                }
            }
            wait(&status);
        }
        // Processo P4
        else
        {
            for (int i = tam/2; i < tam; i++)
            {
                for (int j = tam/2; j < tam; j++)
                {
                    int soma = 0;
                    for (int k = 0; k < tam; k++)
                    {
                        soma += A[i][k] * B[k][j];
                    }
                    C[i][j] = soma;
                }
            }
        }
    }

    return 0;
}