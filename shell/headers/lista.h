#include "auxiliares.h"

#ifndef LISTA_H
#define LISTA_H

void inicializa_lista(Node **L);

void insere_lista(Node **L, Processo P);

Node *pesquisa_pid_lista(Node **L, pid_t pid);

Node *pesquisa_id_lista(Node **L, unsigned int id);

void remove_pid_lista(Node **L, pid_t pid);

void remove_id_lista(Node **L, unsigned int id);

void imprime_lista(Node **L);

#endif
