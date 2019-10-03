#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include "./headers/auxiliares.h"
#include "./headers/lista.h"

void static prog(int argc, char **argv, Contexto *estado);

void interpreta(int argc, char **argv, Contexto *estado)
{
    if (strcmp(argv[0], "exit") == 0)
    {
        pid_t pid;
        // Encerrando os processos na lista de processos do shell
        Node *aux = estado->processos;
        while (aux != NULL)
        {
            pid = aux->proc.pid;
            aux = aux->prox; 
            kill(pid, SIGINT);
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
            printf("shell: diretório '%s' não encontrado\n", argv[1]);
    }
    else if (strcmp(argv[0], "bg") == 0)
    {
        // Verificando se o número de argumentos está correto
        if (argc != 3)
            printf("shell: %d argumentos, esperava 1\n", argc - 2);
        else
        {
            unsigned int num_job;
            if (sscanf(argv[1], "%d ", &num_job) <= 0)
            {
                printf("shell: processo '%s' não é um número\n", argv[1]);
                return;
            }

            // Pesquisando se processo requisitado está em execução
            Node *aux = pesquisa_id_lista(&(estado->processos), num_job);
            if (aux != NULL)
            {
                aux->proc.stopped = 0;
                kill(aux->proc.pid, SIGCONT);
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
            if (sscanf(argv[1], "%d ", &num_job) <= 0)
            {
                printf("shell: processo '%s' não é um número\n", argv[1]);
                return;
            }

            // Pesquisando se processo requisitado está em execução
            Node *aux = pesquisa_id_lista(&(estado->processos), num_job);
            if (aux != NULL)
            {
                estado->fg = aux->proc.pid;
                aux->proc.stopped = 0;
                tcsetpgrp(STDIN_FILENO, aux->proc.pid);

                kill(aux->proc.pid, SIGCONT);
                espera_processo(aux->proc.pid, estado);

                // Devolvendo o controle do terminal para o shell
                tcsetpgrp(STDIN_FILENO, estado->pgid);
                estado->fg = estado->pgid;
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
                printf(" [%d] Stopped   %s  (%d)\n", aux->proc.id, aux->proc.nome, aux->proc.pid);
            else if (estado->fg == aux->proc.pid)
                printf(" [%d] Running   %s  (%d)\n", aux->proc.id, aux->proc.nome, aux->proc.pid);
            else
                printf(" [%d] Running   %s  (%d) &\n", aux->proc.id, aux->proc.nome, aux->proc.pid);
        
            aux = aux->prox;
        }
    }
    else // Um programa externo
    {
        prog(argc, argv, estado);
    }
}

void static prog(int argc, char **argv, Contexto *estado)
{
    Processo novo_processo;

    pid_t pid;

    int fd;
    int contador;

    char *path = NULL;
    char **args = NULL;

    // Flags
    char achou = 0;
    char esta_em_bin = 0;

    if (argv[0][0] != '.')
    {
        path = malloc_safe(sizeof(char) * (strlen("/bin/") + strlen(argv[0]) + 1));
        strcpy(path, "/bin/");
        strcat(path, argv[0]);
        argv[0] = path;
        esta_em_bin = 1;
    }


    // Se o arquivo existente em argv[0] não existir
    fd = open(argv[0], O_RDONLY);
    if (fd == -1)
    {
        printf("Problemas ao encontrar '%s'\n", argv[0]);
        return;
    }
    close(fd);

    // Se o arquivo existe, basta duplicar o processo e execve()
    pid = fork();

    if (pid == 0)
    {
        pid = getpid(); // Pegando o PID do filho

        // Atualizando parent group
        setpgid(pid, pid);

        // Assumindo o controle do terminal, se estiver em foreground
        if (strcmp(argv[argc - 2], "&") != 0)
            tcsetpgrp(STDIN_FILENO, pid);

        // Desabilitanto o tratamento de sinais para este processos
        signal(SIGINT,  SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        signal(SIGTTIN, SIG_DFL);
        signal(SIGTTOU, SIG_DFL);
        signal(SIGCHLD, SIG_DFL);

        // Tratando de programas que usam redirecionamento de entrada, saída e saída de erro
        for (int i = 0; i < argc - 1; i++)
        {
            char deu_problema = 0;

            // Redirecionamento de entrada
            if (strcmp(argv[i], "<") == 0)
            {    
                fd = open(argv[i + 1], O_RDONLY | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
                if (dup2(fd, STDIN_FILENO) < 0)
                    deu_problema = 1;
                close(fd);
            }
            // Redirecionamento de saida truncando arquivo
            else if(strcmp(argv[i], ">") == 0)
            {
                fd = open(argv[i + 1], O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);
                if (dup2(fd, STDOUT_FILENO) < 0)
                    deu_problema = 1;                
                close(fd);
            }
            // Redirecionamento de saida adicionando ao arquivo
            else if(strcmp(argv[i], ">>") == 0)
            {
                fd = open(argv[i + 1], O_WRONLY | O_CREAT | O_APPEND, S_IRWXU | S_IRWXG | S_IRWXO);
                if (dup2(fd, STDOUT_FILENO) < 0)
                    deu_problema = 1;                
                close(fd);
            }
            // Redirecionamento de saida de erro truncando arquivo
            else if(strcmp(argv[i], "2>") == 0)
            {
                fd = open(argv[i + 1], O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);
                if (dup2(fd, STDERR_FILENO) < 0)
                    deu_problema = 1;                
                close(fd);
            }
            // Redirecionamento de saida de erro adicionando ao arquivo
            else if(strcmp(argv[i], "2>>") == 0)
            {
                fd = open(argv[i + 1], O_WRONLY | O_CREAT | O_APPEND, S_IRWXU | S_IRWXG | S_IRWXO);
                if (dup2(fd, STDOUT_FILENO) < 0)
                    deu_problema = 1;
                close(fd);
            }

            if (deu_problema)
            {
                printf("shell: problemas ao criar file descriptor\n");
                exit(1);
            }
        }

        // Recortando lista de argumentos até encontrar redirecionamento ou 
        // background. Adiciona NULL na última posição
        contador = 2;
        args = NULL;
        achou = 0;

        for (int i = 1; !achou && i < argc - 1; i++)
        {
            if (strcmp(argv[i], ">") == 0 || strcmp(argv[i], "<" ) == 0 || strcmp(argv[i], "&"  ) == 0
            || strcmp(argv[i], ">>") == 0 || strcmp(argv[i], "2>") == 0 || strcmp(argv[i], "2>>") == 0)
                achou = 1;
            else
                contador++;
        }

        args = malloc_safe(sizeof(char *) * contador);

        for (int i = 0; i < contador - 1; i++)
            args[i] = argv[i];
        args[contador - 1] = NULL;

        // Se execve() retornar, algo de errado ocorreu
        if (execve(argv[0], args, NULL) == -1)
        {
            printf("shell: não foi possível executar '%s'\n", argv[0]);
            sleep(1);
            remove_pid_lista(&(estado->processos), pid);
            exit(1);
        }
    }
    else
    {
        setpgid(pid, pid);

        // Atualiza estado e cria processo a ser adicionado à lista do shell
        estado->num_processos++;

        novo_processo.pid = pid;
        novo_processo.id = estado->num_processos;
        novo_processo.stopped = 0;
        strcpy(novo_processo.nome, argv[0]);
        
        insere_lista(&(estado->processos), novo_processo);

        // Se precisa ficar em foreground
        if (strcmp(argv[argc - 2], "&") != 0)
        {
            estado->fg = pid;
            tcsetpgrp(STDIN_FILENO, pid);
            espera_processo(pid, estado);
            
            // Devolvendo o controle do terminal para o shell
            tcsetpgrp(STDIN_FILENO, estado->pgid);
        }
        else
            printf(" [%d] Running   %s  (%d) &\n", novo_processo.id, novo_processo.nome, novo_processo.pid);
    }

    if (esta_em_bin)
        free(path);
}
