#include "auxiliares.h"

#ifndef LISTA_H
#define LISTA_H

// Inicializa lista encadeada de processos
void inicializa_lista(Node **L);

// Insere dado processo P na lista L
void insere_lista(Node **L, Processo P);

// Retorna o vértice da lista que possui o pid passado
Node *pesquisa_pid_lista(Node **L, pid_t pid);

// Retorna o vértice da lista que possui o id passado
Node *pesquisa_id_lista(Node **L, unsigned int id);

// Remove da lista o vértice que possui o pid passado
void remove_pid_lista(Node **L, pid_t pid);

// Remove da lista o vértice que possui o pid passado
void remove_id_lista(Node **L, unsigned int id);

// Imprime os conteúdos da lista L
void imprime_lista(Node **L);

#endif
