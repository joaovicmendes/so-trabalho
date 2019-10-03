#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "./headers/auxiliares.h"
#include "./headers/lista.h"

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

char *remove_espacos(char* texto)
{
    char *aux = malloc(sizeof(char) * (strlen(texto) + 1));
    char *texto_corrigido = NULL;

    int tam_final = 0;
    for (int i = 0; i < strlen(texto); i++)
    {
        if (texto[i] != ' ')
            aux[tam_final++] = texto[i];

        // Condições para copiar o espaço:
        // 1) Não está na 1a casa
        // 2) Não é precedido por um espaço
        // 3) Não é sucedido pelo caracter nulo (fim da frase)
        else if (i != 0 && texto[i-1] != ' ' && texto[i+1] != '\0' && texto[i+1] != '\n')
            aux[tam_final++] = texto[i];
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
    return texto_corrigido;
}

void espera_processo(pid_t pid, Contexto *estado)
{
    int status;

    waitpid(pid, &status, WUNTRACED);

    Node *aux = pesquisa_pid_lista(&(estado->processos), pid);
    if (aux != NULL)
    {
        if (WIFEXITED(status) || WIFSIGNALED(status))
        {
            printf(" [%d]+ Done       %s  (%d)\n", aux->proc.id, aux->proc.nome, aux->proc.pid);
            remove_pid_lista(&(estado->processos), pid);
        }
        else if (WIFSTOPPED(status))
        {
            aux->proc.stopped = 1;
            printf(" [%d]+ Stopped    %s  (%d)\n", aux->proc.id, aux->proc.nome, aux->proc.pid);
        }
        if (estado->fg == pid)
        {
            estado->fg = getpid();    
            tcsetpgrp(STDIN_FILENO, estado->pgid);
        }   
    }
}

void atualiza_processo(Contexto *estado)
{
    pid_t pid_term = getpid();
    pid_t pid;
    id_t id = 0;
    siginfo_t infop;
    infop.si_pid = 0;

    if (waitid(P_ALL, id, &infop, WNOHANG) == 0)
    {
        if (infop.si_pid != 0)
        {
            pid = infop.si_pid;
            if (infop.si_code == CLD_KILLED || infop.si_code == CLD_EXITED)
            {
                Node *aux = pesquisa_pid_lista(&(estado->processos), pid);
                if (aux != NULL)
                    printf(" [%d]+ Done       %s  (%d)\n", aux->proc.id, aux->proc.nome, aux->proc.pid);
                
                remove_pid_lista(&(estado->processos), pid);
                if (estado->fg == pid)
                {
                    estado->fg = pid_term;    
                    tcsetpgrp(STDIN_FILENO, estado->pgid);
                }    
            }
            else if (infop.si_code == CLD_STOPPED)
            {
                Node *aux = pesquisa_pid_lista(&(estado->processos), pid);
                if (aux != NULL)
                    aux->proc.stopped = 1;

                if (estado->fg == pid)
                {
                    estado->fg = pid_term;
                    tcsetpgrp(STDIN_FILENO, estado->pgid);
                }
            }
            else if (infop.si_code == CLD_CONTINUED)
            {
                Node *aux = pesquisa_pid_lista(&(estado->processos), pid);
                if (aux != NULL)
                    aux->proc.stopped = 0;
            }
        }
    }
}

