#ifndef SIGNAL_H
#define SIGNAL_H

// Tratamento do sinal SIGCHLD
void sig_chld();

// Transferencia do sinal sig para o processo em fg
void sig_int(int sig);

#endif
