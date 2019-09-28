#ifndef AUX_H
#define AUX_H

#define TAM_PWD 200

typedef struct contexto {
    char *pwd;
} Contexto;

// Implementação do malloc() que já faz verificação de memória insuficiente
void *malloc_safe(unsigned nbytes);

#endif