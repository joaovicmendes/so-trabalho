#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./headers/auxiliares.h"

void *malloc_safe(unsigned nbytes)
{
    void *p;
    p = malloc(nbytes);
    if (p == NULL)
    {
        printf("Não foi possível alocar memória\n");
        exit(EXIT_FAILURE);
    }
    return p;
}

void remove_espacos(char** texto)
{
    char *aux = malloc(sizeof(char) * (strlen(*texto) + 1));
    char *texto_corrigido = NULL;

    int tam_final = 0;
    for (int i = 0; i < strlen(*texto); i++)
    {
        if ((*texto)[i] != ' ')
            aux[tam_final++] = (*texto)[i];

        // Condições para copiar o espaço:
        // 1) Não está na 1a casa
        // 2) Não é precedido por um espaço
        // 3) Não é sucedido pelo caracter nulo (fim da frase)
        else if (i != 0 && (*texto)[i-1] != ' ' && (*texto)[i+1] != '\0' && (*texto)[i+1] != '\n')
            aux[tam_final++] = (*texto)[i];
    }
    // Se sobrou um ' ' na última posição antes do \n\0
    if (aux[strlen(aux) - 2] == ' ')
    {
        aux[strlen(aux) - 2] = '\n';
        aux[strlen(aux) - 1] = '\0';
        tam_final--;
    }
    
    texto_corrigido = (char *) malloc(tam_final * sizeof(char));
    strcpy(texto_corrigido, aux);

    free(aux);
    aux = NULL;
    free(*texto);
    (*texto) = texto_corrigido;
}
