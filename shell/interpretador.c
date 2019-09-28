#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include "./headers/auxiliares.h"
#include "./headers/lista.h"

void interpreta(int argc, char **argv, Contexto *estado)
{
    if (strcmp(argv[0], "exit") == 0)
    {
        // Encerrando os processos na lista de processos
        Node *aux = estado->processos;
        while (aux != NULL)
        {
            kill(aux->proc.pid, SIGKILL);
            aux = aux->prox;
        }
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
        if (argc < 3)
            printf("Argumentos insuficientes\n");
        else
        {
            unsigned int num_job;
            sscanf(argv[1], "%d ", &num_job);

            Node *aux = pesquisa_id_lista(&(estado->processos), num_job);
            if (aux != NULL)
            {
                waitpid(aux->proc.pid, NULL, 0);
                remove_pid_lista(&(estado->processos), aux->proc.pid);
                        
                printf("Processo %d finalizado.\n", aux->proc.pid);   
            }
        }
    }
    else if (strcmp(argv[0], "jobs") == 0)
    {
        Node *aux = estado->processos;
        while (aux != NULL)
        {
            printf(" [%d] %s %d\n", aux->proc.id, aux->proc.nome, aux->proc.pid);
            aux = aux->prox;
        }
    }
    else // prog
    {
        Processo novo_processo;
        pid_t pid;

        estado->num_processos++;
        novo_processo.id = estado->num_processos;
        strcpy(novo_processo.nome, argv[0]);

        pid = fork();
        if (pid == 0) // Filho
        {
            // Se execve() retornar, algo de errado ocorreu
            if (execve(argv[0], argv, NULL) == -1)
                printf("Problemas ao executar %s\n", argv[0]);
        }
        else // Pai
        {
            novo_processo.pid = pid;
            insere_lista(&(estado->processos), novo_processo);

            // Se precisa ficar em background
            if (strcmp(argv[argc - 2], "&") != 0)
            {
                estado->fg = pid;
                waitpid(pid, NULL, 0);
                estado->fg = getpid();
                remove_pid_lista(&(estado->processos), pid);

                printf("Processo %d finalizado.\n", pid);
            }
        }
    }
}
