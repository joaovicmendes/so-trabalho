#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "./headers/auxiliares.h"
#include "./headers/interpretador.h"

int main(int argc, char **argv)
{
    // Buffer de leitura e tamanho
    size_t buff_size;
    char *buffer;

    // Comando e lista de argumentos
    char **args;

    // Variáveis úteis
    Contexto estado;
    int contador;

    estado.pwd = malloc_safe(sizeof(char) * TAM_PWD);
    estado.pwd = getcwd(estado.pwd, TAM_PWD);

    for (;;)
    {
        // Imprimindo caminho atual
        printf("[%s]$ ", estado.pwd);

        // Lendo linha de comando
        buffer = NULL;
        getline(&buffer, &buff_size, stdin);

        // Removendo espaços duplicados
        remove_espacos(&buffer);
        // Se o comando não é apenas "\n"
        if (strcmp(buffer, "\n") != 0)
        {
            contador = 1;
            for (int i = 0; i < strlen(buffer); i++)
            {
                if (buffer[i] == ' ')
                    contador++;
            }

            // Alocando vetor de argumentos e preenchendo (arg[0] é o nome do programa)
            args = (char **) malloc_safe(contador * sizeof(char *));
            
            args[0] = strtok(buffer, " \n");
            for (int i = 1; i < contador; i++)
                args[i] = strtok(NULL, " \n");

            interpreta(contador, args, &estado);

            free(args);
        }

        free(buffer);
    }

    return 0;
}
