#ifndef AUX_H
#define AUX_H

#define TAM_PWD 250

typedef struct Processo {
    pid_t pid;
    unsigned int id;
} Processo;

typedef struct node {
    struct node *prox;
    Processo proc;
} Node;

typedef struct contexto {
    char *pwd;
    unsigned int num_processos;
    Node *processos;
} Contexto;

// Implementação do malloc() que já faz verificação de memória insuficiente
void *malloc_safe(unsigned nbytes);

// A função recebe uma string e remove desta string espaços desnecessários.
char *remove_espacos(char* texto);

#endif
