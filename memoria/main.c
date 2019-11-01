/*
    Universidade Federal de São Carlos – UFSCar
    Departamento de Computação

    Sistemas Operacionais
    Prof. Dr. Fredy Valente
    Prof. Dr. Hélio Crestana Guardia

    Aluno: João Victor Mendes Freire (758943)
    e Guilherme Locca Salomão (758569)
    e Renan Dantas Pasquantonio (760549)
*/

#include <stdio.h>
#include "./headers/memoria.h"

#define TAM_VETOR 10

int main(int argc, char **argv)
{
    int *ptr     = NULL;
    int *ptr2    = NULL;
    int **matriz = NULL;

    printf("1. Alocações comuns\n");
    /* Variável */
    printf("Alocando variável\n");
    printf("- Imprimindo ptr sem alocar, NULL esperado:\n");
    printf(" ptr = %d\n", ptr);
    ptr = aloca_mem(sizeof(int));
    *ptr = 5;
    printf("- Imprimindo ptr alocado, 5 esperado:\n");
    printf(" ptr = %d\n", *ptr);
    printf("- Liberando ptr\n\n");
    libera(ptr);

    /* Vetor */
    printf("Alocando vetor\n");
    printf("- Imprimindo ptr sem alocar, lixo esperado:\n");
    printf(" ptr = %d\n", *ptr);
    ptr = aloca_mem(sizeof(int) * TAM_VETOR);
    printf("- Imprimindo vetor alocado, [0..9] esperado:\n");
    printf(" ptr =");
    for (int i = 0; i < TAM_VETOR; i++)
    {
        ptr[i] = i;
        printf(" %d", ptr[i]);
    }
    printf("\n");
    printf("- Liberando ptr\n\n");
    libera(ptr);


    /* Matriz */
    printf("Alocando matriz\n");
    printf("- Imprimindo matriz sem alocar, NULL esperado:\n");
    printf(" matriz = %d\n", matriz);
    matriz = aloca_mem(sizeof(int *) * TAM_VETOR);
    printf("- Imprimindo matriz alocada, matriz identidade esperada:\n");
    printf(" matriz =\n");
    for (int i = 0; i < TAM_VETOR; i++)
    {
        matriz[i] = aloca_mem(sizeof(int) * TAM_VETOR);
        for (int j = 0; j < TAM_VETOR; j++)
        {
            matriz[i][j] = (i == j) ? 1 : 0;
            printf(" %d", matriz[i][j]);
        }
        printf("\n");
    }
    printf("- Liberando matriz\n\n");
    libera(matriz);

    /* Reutilização de espaço */
    printf("2. Reutilização de espaço\n");
    printf("- Aloca um vetor *ptr de tamanho TAM_MAX e preenche com 0s.\n");
    printf("- Libera a memória, e aloca um novo ponteiro *ptr2 de tamanho TAM_MAX - 1, mas preenche com 1s.\n");
    printf("- Por fim, imprime *ptr, e mostra que seu espaço foi reutilizado por *ptr2.\n\n");

    ptr = (int *) aloca_mem(sizeof(int) * TAM_VETOR);
    for (int i = 0; i < TAM_VETOR; i++)
        ptr[i] = 0;

    printf(" Esperado: 0s\n");
    for (int i = 0; i < TAM_VETOR; i++)    
        printf("  Valor em  ptr(%p) (alocado): %d\n", ptr + i, ptr[i]);

    libera(ptr);

    ptr2 = (int *) aloca_mem(sizeof(int) * (TAM_VETOR - 1));
    for (int i = 0; i < TAM_VETOR - 1; i++)
        ptr2[i] = 1;

    printf(" Esperado: 1s\n");    
    for (int i = 0; i < TAM_VETOR - 1; i++)    
        printf("  Valor em ptr2(%p) (alocado): %d\n", ptr2 + i, ptr2[i]);

    printf(" Esperado: 1s e lixo na última\n");
    for (int i = 0; i < TAM_VETOR; i++)    
        printf("  Valor em  ptr(%p) (não alocado): %d\n", ptr + i, ptr[i]);

    return 0;
}
