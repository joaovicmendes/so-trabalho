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
    pid_t pid;
    id_t id = 0;
    siginfo_t *infop;

    if (waitid(P_ALL, id, infop, WEXITED | WSTOPPED) == 0)
    {
        pid = infop->si_pid;
        remove_pid_lista(&(estado.processos), pid);
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
