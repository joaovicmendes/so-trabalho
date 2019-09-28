#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./headers/auxiliares.h"
#include "./headers/lista.h"

void inicializa_lista(Node **L)
{
	*L = NULL;
}

void insere_lista(Node **L, Processo P)
{
	Node *nova = malloc_safe(sizeof(Node));
	
	nova->proc = P;
	nova->prox = *L;
	*L = nova;

	printf("    item inserido com ID %d e PID %d\n", nova->proc.id, nova->proc.pid);
}

Node *pesquisa_pid_lista(Node **L, pid_t pid) 
{
	Node *aux = *L;
	while(aux != NULL && aux->proc.pid != pid) 
        aux = aux->prox;
            
	return aux;
}

Node *pesquisa_id_lista(Node **L, unsigned int id)
{
	Node *aux = *L;
	while(aux != NULL && aux->proc.id != id) 
        aux = aux->prox;
            
	return aux;
}

void remove_pid_lista(Node **L, pid_t pid)
{
    fflush(stdout);
    Node *antes_morta = *L;
    Node *morta = NULL;

    // Se a lista estiver vazia
    if (antes_morta == NULL)
        return;
    // Se o que deve remover for o primeiro
    else if (antes_morta->proc.pid == pid)
    {
        *L = antes_morta->prox;
        printf("    removendo id %d pid %d", antes_morta->proc.id, antes_morta->proc.pid);
        free(antes_morta);
    }
    // Se não for vazia e não for o primeiro
    else
    {
        // Até que se chegue no fim ou encontre
        while (antes_morta != NULL && antes_morta->prox->proc.pid != pid) 
            antes_morta = antes_morta->prox;

        if (antes_morta != NULL)
        {
            morta = antes_morta->prox;
            antes_morta->prox = morta->prox;
            printf("    removendo id %d pid %d", morta->proc.id, morta->proc.pid);
            free(morta);
        }
    }
}

void remove_id_lista(Node **L, unsigned int id)
{
    Node *antes_morta = *L;
    Node *morta = NULL;

    // Se a lista estiver vazia
    if (antes_morta == NULL)
        return;
    // Se o que deve remover for o primeiro
    else if (antes_morta->proc.id == id)
    {
        *L = antes_morta->prox;
        printf("    removendo id %d pid %d", antes_morta->proc.id, antes_morta->proc.pid);
        free(antes_morta);
    }
    // Se não for vazia e não for o primeiro
    else
    {
        // Até que se chegue no fim ou encontre
        while (antes_morta != NULL && antes_morta->prox->proc.id != id) 
            antes_morta = antes_morta->prox;

        if (antes_morta != NULL)
        {
            morta = antes_morta->prox;
            antes_morta->prox = morta->prox;
            printf("    removendo id %d pid %d", morta->proc.id, morta->proc.pid);            
            free(morta);
        }
    }
}

void imprime_lista(Node **L)
{
	Node *aux = *L;
    while (aux != NULL)
    {
        printf("  ID: %d, PID: %d\n", aux->proc.id, aux->proc.pid);
        aux = aux->prox;
    }
}
