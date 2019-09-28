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
    char *buffer = NULL;
    char *comando = NULL;

    // Comando e lista de argumentos
    char **args = NULL;

    // Variáveis úteis
    Contexto estado;
    int contador;

    estado.pwd = malloc_safe(sizeof(char) * TAM_PWD);

    for (;;)
    {
        // Imprimindo caminho atual
        estado.pwd = getcwd(estado.pwd, TAM_PWD);
        printf("[%s]$ ", estado.pwd);

        // Lendo linha de comando
        buffer = NULL;
        getline(&buffer, &buff_size, stdin);

        // Removendo espaços duplicados
        comando = remove_espacos(buffer);
        // Se o comando não é apenas "\n"
        if (strcmp(comando, "\n") != 0)
        {
            // Começa com dois para guardar em args[0] o nome do programa
            // e em args[n] um NULL (usado no execv())
            contador = 2;
            for (int i = 0; i < strlen(comando); i++)
            {
                if (comando[i] == ' ')
                    contador++;
            }

            // Alocando vetor de argumentos
            args = (char **) malloc_safe(contador * sizeof(char *));
            
            args[0] = strtok(comando, " \n");
            for (int i = 1; i < contador; i++)
                args[i] = strtok(NULL, " \n");

            interpreta(contador, args, &estado);

            free(args);
            args = NULL;
        }

        free(buffer);
        buffer = NULL;
        free(comando);
        comando = NULL;
    }

    return 0;
}
