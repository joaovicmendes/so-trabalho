#ifndef MEMORIA_H
#define MEMORIA_H

// Implementação da função malloc(), que aloca tam bytes de memória e retorna
// um ponteiro para a posição inicial alocada.
void *aloca_mem(size_t tam);

// Implementação da função calloc(), que aloca espaço suficiente para qtd 
// objetos de tamanho tam.
void *aloca_quantos(size_t qtd, size_t tam);

// Implementação da função realloc(), que tenta mudar o tamanho da memória 
// apontada por ptr. Se não houver espaço para aumentar a memória de ptr,
// uma nova alocação é feita, e copia tanto quanto for possivel da informação
// existente no vetor anterior. Retorna um ponteiro para a nova posição de 
// memória.
void *realoca(void *ptr, size_t tam);

// Libera o espaço de memória apontado por ptr.
void libera(void *ptr);

#endif