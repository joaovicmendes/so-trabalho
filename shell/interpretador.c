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
            if (sscanf(argv[1], "%d ", &num_job) <= 0)
            {
                printf("Processo [%s] não é um número\n", argv[1]);
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
                printf("Processo [%s] não é um número\n", argv[1]);
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
                printf(" [%d]  Stopped    %s  (%d)\n", aux->proc.id, aux->proc.nome, aux->proc.pid);
            else if (estado->fg == aux->proc.pid)
                printf(" [%d]+ Running    %s  (%d)\n", aux->proc.id, aux->proc.nome, aux->proc.pid);
            else
                printf(" [%d]  Running    %s &(%d)\n", aux->proc.id, aux->proc.nome, aux->proc.pid);
            
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
    int esta_em_bin = 0;
    int contador;
    char *path = NULL;
    char **args = NULL;
    char achou;
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
    if (pid == 0) // Filho
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

        // Tratando de programas que usam > e < para redirecionar as entrada e saída padrão
        for (int i = 0; i < argc - 1;i++)
        {
            if (strcmp(argv[i], "<") == 0)  // Redirecionamento de entrada
            {    
                char* pwd_in = malloc_safe(sizeof(char) * (strlen(estado->pwd) + strlen(argv[i + 1]) + 2));
                strncpy(pwd_in, estado->pwd, strlen(estado->pwd));
                // adiciona / antes de receber o nome do arquivo
                strcat(pwd_in, "/");
                // adiciona nome do arquivo que vai abrir
                strcat(pwd_in, argv[i + 1]);
                // abre o arquivo e armazena o file descriptor dele
                int fildes1 = open(pwd_in, O_RDONLY | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
                
                if (dup2(fildes1, 0) < 0)
                    printf("Problemas ao criar file descriptor\n");

                close(fildes1);
                free(pwd_in);
            }
            else if(strcmp(argv[i], ">") == 0) // Redirecionamento de saida truncando arquivo
            {
                char* pwd_out = malloc_safe(sizeof(char) * (strlen(estado->pwd) + strlen(argv[i + 1]) + 2));

                strncpy(pwd_out, estado->pwd, strlen(estado->pwd));
                // adiciona / antes de receber o nome do arquivo que vai receber o output
                strcat(pwd_out, "/");
                // adiciona nome do arquivo de output
                strcat(pwd_out, argv[i + 1]);

                // abre o aquivo e armazena o file descriptor dele
                int fildes2 = open(pwd_out, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);
                // tenta "redirecionar" o output, onde o fildes2(arquivo output) recebe o que esta no 1(stdout)
                if (dup2(fildes2, 1) < 0)
                    printf("Problemas ao criar file descriptor\n");
                close(fildes2);
                free(pwd_out);
            }
            else if(strcmp(argv[i], ">>") == 0) // Redirecionamento de saida adicionando ao arquivo
            {
                char* pwd_out = malloc_safe(sizeof(char) * (strlen(estado->pwd) + strlen(argv[i + 1]) + 2));

                strncpy(pwd_out, estado->pwd, strlen(estado->pwd));
                // adiciona / antes de receber o nome do arquivo que vai receber o output
                strcat(pwd_out, "/");
                // adiciona nome do arquivo de output
                strcat(pwd_out, argv[i + 1]);

                // abre o aquivo e armazena o file descriptor dele
                int fildes2 = open(pwd_out, O_WRONLY | O_CREAT | O_APPEND, S_IRWXU | S_IRWXG | S_IRWXO);
                // tenta "redirecionar" o output, onde o fildes2(arquivo output) recebe o que esta no 1(stdout)
                if (dup2(fildes2, 1) < 0)
                    printf("Problemas ao criar file descriptor\n");
                close(fildes2);
                free(pwd_out);
            }
            else if(strcmp(argv[i], "2>") == 0) // Redirecionamento de arquivo de erro truncando
            {
                char* pwd_err = malloc_safe(sizeof(char) * (strlen(estado->pwd) + strlen(argv[i + 1]) + 2));

                strncpy(pwd_err, estado->pwd, strlen(estado->pwd));
                // adiciona / antes de receber o nome do arquivo que vai receber o output
                strcat(pwd_err, "/");
                // adiciona nome do arquivo de output
                strcat(pwd_err, argv[i + 1]);

                // abre o aquivo e armazena o file descriptor dele
                int fildes3 = open(pwd_err, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);
                // tenta "redirecionar" o output, onde o fildes3(arquivo output) recebe o que esta no 1(stdout)
                if (dup2(fildes3, 2) < 0)
                    printf("Problemas ao criar file descriptor\n");
                close(fildes3);
                free(pwd_err);
            }
            else if(strcmp(argv[i], "2>>") == 0) // Redirecionamento de arquivo de erro adicionando
            {
                char* pwd_err = malloc_safe(sizeof(char) * (strlen(estado->pwd) + strlen(argv[i + 1]) + 2));

                strncpy(pwd_err, estado->pwd, strlen(estado->pwd));
                // adiciona / antes de receber o nome do arquivo que vai receber o output
                strcat(pwd_err, "/");
                // adiciona nome do arquivo de output
                strcat(pwd_err, argv[i + 1]);

                // abre o aquivo e armazena o file descriptor dele
                int fildes3 = open(pwd_err, O_WRONLY | O_CREAT | O_APPEND, S_IRWXU | S_IRWXG | S_IRWXO);
                // tenta "redirecionar" o output, onde o fildes3(arquivo output) recebe o que esta no 1(stdout)
                if (dup2(fildes3, 2) < 0)
                    printf("Problemas ao criar file descriptor\n");
                close(fildes3);
                free(pwd_err);
            }
        }

        // Limpando a lista de argumentos, ou seja, passando os argumentos
        // até encontrar um dentre <, > e &.
        // contador começa em dois para guardar argv[0]: nomeprograma e 
        // argv[n]: NULL.
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
            printf("Problemas ao executar '%s'\n", argv[0]);
            sleep(1);
            remove_pid_lista(&(estado->processos), pid);
            exit(1);
        }
    }
    else // Pai
    {
        // Atualizando parent group
        setpgid(pid, pid);

        // Atualizando estado e criando dados do processo a ser adicionado
        // à lista do shell
        estado->num_processos++;
        novo_processo.pid = pid;
        novo_processo.id = estado->num_processos;
        novo_processo.stopped = 0;
        strcpy(novo_processo.nome, argv[0]);
        
        insere_lista(&(estado->processos), novo_processo);

        // Verificando se precisa ficar em foreground
        if (strcmp(argv[argc - 2], "&") != 0)
        {
            estado->fg = pid;
            tcsetpgrp(STDIN_FILENO, pid);
            espera_processo(pid, estado);
            
            // Devolvendo o controle do terminal para o shell
            tcsetpgrp(STDIN_FILENO, estado->pgid);
        }
    }

    if (esta_em_bin)
        free(path);
}
