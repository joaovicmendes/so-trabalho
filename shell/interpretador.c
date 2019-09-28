#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "./headers/auxiliares.h"
#include "./headers/lista.h"

void interpreta(int argc, char **argv, Contexto *estado)
{
    if (strcmp(argv[0], "exit") == 0)
    {
        exit(0);
    }
    else if (strcmp(argv[0], "pwd") == 0)
    {
        printf("%s\n", estado->pwd);
    }
    else if (strcmp(argv[0], "cd") == 0)
    {
        if (chdir(argv[1]) == -1)
            printf("Diretório %s não encontrado\n", argv[1]);
    }
    else if (strcmp(argv[0], "bg") == 0)
    {
        // Não implementado
    }
    else if (strcmp(argv[0], "fg") == 0)
    {
        // Não implementado
    }
    else if (strcmp(argv[0], "jobs") == 0)
    {
    
    }
    else
    {
        pid_t pid;
        if (strcmp(argv[argc - 2], "&") == 0)
        {
            pid = fork();
            if (pid == 0) // Filho
                execve(argv[0], argv, NULL);
            else // Pai
            {
                // Colocar na lista de jobs
            }
        }
        else
        {
            pid = fork();
            if (pid == 0) // Filho
                execve(argv[0], argv, NULL);
            else // Pai
            {
                // Colocar na lista de jobs
                waitpid(pid, NULL, 0);
                printf("Processo %d finalizado.\n", pid);
            }
        }
    }
}
