#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "./headers/auxiliares.h"
#include "./headers/interpretador.h"
#include "./headers/lista.h"
#include "./headers/signal.h"

extern Contexto estado;

void sig_chld()
{
    int status;
    pid_t pid;

    pid = wait3(&status, WNOHANG, NULL);
    remove_pid_lista(&(estado.processos), pid);
}
