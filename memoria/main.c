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
    int *ptr;
    int *ptr2;

    /*
        Caso de teste (reutilização de espaço): 
        Aloca um vetor *ptr de tamanho TAM_MAX e preenche com 0s.
        Libera a memória, e aloca um novo ponteiro *ptr2 de tamanho TAM_MAX + 1,
        mas preenche com 1s.
        Por fim, imprime *ptr, e mostra q seu espaço foi reutilizado por *ptr2.
    */
   printf("%d", sizeof(t_block));
    printf("\n1. Reutilização de espaço\n");
    printf(" 1.1 Aloca um vetor *ptr de tamanho TAM_MAX e preenche com 0s.\n");
    printf(" 1.2 Libera a memória, e aloca um novo ponteiro *ptr2 de tamanho TAM_MAX + 1,\n");
    printf("     mas preenche com 1s.\n");
    printf(" 1.4 Por fim, imprime *ptr, e mostra que seu espaço foi reutilizado por *ptr2.\n\n");

    ptr = (int *) aloca_mem(sizeof(int) * TAM_VETOR);
    for (int i = 0; i < TAM_VETOR; i++)
        ptr[i] = 0;

    printf(" Esperado: 0s\n");    
    for (int i = 0; i < TAM_VETOR; i++)    
        printf("  Valor em ptr(%p) (alocado): %d\n", ptr+i , ptr[i]);
   
    libera(ptr);
   
    ptr2 = (int *) aloca_mem(sizeof(int) * (TAM_VETOR - 1));
    for (int i = 0; i < TAM_VETOR-1; i++)
        ptr2[i] = 1;

    printf(" Esperado: 1s\n");    
    for (int i = 0; i < TAM_VETOR - 1; i++)    
        printf("  Valor em ptr2(%p) (alocado): %d\n", ptr2 + i, ptr2[i]);

    printf(" Esperado: 1s\n");
    for (int i = 0; i < TAM_VETOR; i++)    
        printf("  Valor em ptr(%p) (não alocado): %d\n", ptr + i, ptr[i]);

    libera(ptr2);

    return 0;
}