#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include "./headers/auxiliares.h"
#include "./headers/lista.h"
#include "./headers/sinal.h"

void interpreta(int argc, char **argv, Contexto *estado)
{
    if (strcmp(argv[0], "exit") == 0)
    {
        // Encerrando os processos na lista de processos do shell
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
        // Verificando se o número de argumentos está correto
        if (argc != 3)
            printf("%d argumentos, esperava 1\n", argc - 2);
        else
        {
            unsigned int num_job;
            sscanf(argv[1], "%d ", &num_job);

            // Pesquisando se processo requisitado está em execução
            Node *aux = pesquisa_id_lista(&(estado->processos), num_job);
            if (aux != NULL)
            {
                if (aux->proc.stopped)
                {
                    aux->proc.stopped = 0;
                    kill(aux->proc.pid, SIGCONT);
                }
            }
        }
    }
    else if (strcmp(argv[0], "fg") == 0)
    {
        // Verificando se o número de argumentos está correto
        if (argc != 3)
            printf("%d argumentos, esperava 1\n", argc - 2);
        else
        {
            unsigned int num_job;
            sscanf(argv[1], "%d ", &num_job);

            // Pesquisando se processo requisitado está em execução
            Node *aux = pesquisa_id_lista(&(estado->processos), num_job);
            if (aux != NULL)
            {
                if (aux->proc.stopped)
                {
                    aux->proc.stopped = 0;
                    kill(aux->proc.pid, SIGCONT);
                }
                espera_processo(aux->proc.pid, estado);
            }
        }
    }
    else if (strcmp(argv[0], "jobs") == 0)
    {
        // Imprimindo lista de processos
        Node *aux = estado->processos;
        while (aux != NULL)
        {
            if (aux->proc.stopped)
                printf(" [%d] Stopped    %s    (%d)\n", aux->proc.id, aux->proc.nome, aux->proc.pid);
            else if (estado->fg == aux->proc.pid)
                printf(" [%d] Running    %s    (%d)\n", aux->proc.id, aux->proc.nome, aux->proc.pid);
            else
                printf(" [%d] Running    %s &  (%d)\n", aux->proc.id, aux->proc.nome, aux->proc.pid);
            
            aux = aux->prox;
        }
    }
    else // Um programa externo
    {
        Processo novo_processo;
        pid_t pid;
        

        //@cleanup encontrar uma forma de testar se o executável existe 
        // antes de chamar o fork()
        pid = fork();
        if (pid == 0) // Filho
        {
            pid = getpid();
            
            

            // Se execve() retornar, algo de errado ocorreu
            if (execve(argv[0], argv, NULL) == -1)
            {
                printf("Problemas ao executar %s\n", argv[0]);
                sleep(1); // Para garantir que processo foi devidamente adicionado à lista do shell

                remove_pid_lista(&(estado->processos), pid);
                exit(0);
            }
        }
        else // Pai
        {
            // Atualizando estado e criando dados do processo a ser adicionado
            // à lista do shell
            estado->num_processos++;
            novo_processo.pid = pid;
            novo_processo.id = estado->num_processos;
            novo_processo.stopped = 0;
            strcpy(novo_processo.nome, argv[0]);
            
            insere_lista(&(estado->processos), novo_processo);

            // Verificando se precisa ficar em background
            if (strcmp(argv[argc - 2], "&") != 0)
                espera_processo(pid, estado);
        }
    }
}

