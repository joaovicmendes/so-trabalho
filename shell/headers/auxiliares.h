#ifndef AUX_H
#define AUX_H

#define TAM_PWD 250

// Struct das informações que o shell mantém de cada processo
typedef struct Processo {
    unsigned int id;
    char nome[100];
    pid_t pid;
} Processo;

// Struct da lista encadada de processos
typedef struct node {
    Processo proc;
    struct node *prox;
} Node;

// Struct das informações sobre sí mesma que o shell mantém
typedef struct contexto {
    char *pwd;                  // Path of working directory
    pid_t fg;                   // PID do processo em foreground
    Node *processos;            // Lista de processos filhos
    unsigned int num_processos; // ID para criação de novos processos
} Contexto;

// Implementação do malloc() que já faz verificação de memória insuficiente
void *malloc_safe(unsigned nbytes);

// A função recebe uma string e remove espaços desnecessários.
char *remove_espacos(char* texto);

#endif
