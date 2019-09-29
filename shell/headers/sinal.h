#ifndef SIGNAL_H
#define SIGNAL_H

// Tratamento do sinal SIGCHLD
void sig_chld(int sig);

// Tratamento do sinal SIGINT
void sig_int(int sig);

// Tratamento do sinal SIGTSTP
void sig_tstp(int sig);

#endif
