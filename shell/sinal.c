#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "./headers/auxiliares.h"
#include "./headers/interpretador.h"
#include "./headers/lista.h"
#include "./headers/sinal.h"

extern Contexto estado;

void sig_chld(int sig)
{
    pid_t pid_term = getpid();
    pid_t pid;
    id_t id = 0;
    siginfo_t *infop;

    if (waitid(P_ALL, id, infop, WEXITED | WSTOPPED | WCONTINUED) == 0)
    {
        pid = infop->si_pid;

        if (infop->si_code == CLD_KILLED)
        {
            remove_pid_lista(&(estado.processos), pid);
            if (estado.fg == pid)
            {
                estado.fg = pid_term;    
                tcsetpgrp(STDIN_FILENO, estado.pgid);
            }    
        }
        else if (infop->si_code == CLD_STOPPED)
        {
            Node *aux = pesquisa_pid_lista(&(estado.processos), pid);
            if (aux != NULL)
                aux->proc.stopped = 1;

            if (estado.fg == pid)
            {
                estado.fg = pid_term;
                tcsetpgrp(STDIN_FILENO, estado.pgid);
            }
        }
        else if (infop->si_code == CLD_CONTINUED)
        {
            Node *aux = pesquisa_pid_lista(&(estado.processos), pid);
            if (aux != NULL)
                aux->proc.stopped = 0;
        }
    }
}

void sig_int(int sig)
{
    pid_t pid_term = getpid();
    if (estado.fg != pid_term)
    {
        kill(estado.fg, sig);
        remove_pid_lista(&(estado.processos), estado.fg);
        estado.fg = pid_term;
    }
}

void sig_tstp(int sig)
{
    pid_t pid_term = getpid();
    if (estado.fg != pid_term)
    {
        Node *aux = pesquisa_pid_lista(&(estado.processos), estado.fg);
        if (aux != NULL)
            aux->proc.stopped = 1;
        kill(estado.fg, sig);
        estado.fg = pid_term;
    }
}
