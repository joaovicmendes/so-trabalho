#ifndef MEMORIA_H
#define MEMORIA_H

/* 
    Retirado de: 
    https://wiki-prog.infoprepa.epita.fr/images/0/04/Malloc_tutorial.pdf 
*/

struct s_block
{
    size_t size;
    struct s_block *next;
    struct s_block *prev;
    int free;
    void *ptr;
    char data[1];
};
typedef struct s_block *t_block;

#define BLOCK_SIZE 40

#define align4(x) (((((x) - 1) >> 2) << 2) + 4)

static void *base = NULL;

// Implementação da função malloc(), que aloca tam bytes de memória e retorna
// um ponteiro para a posição inicial alocada.
void *aloca_mem(size_t size);

// Libera o espaço de memória apontado por ptr.
void libera(void *p);

// Funções internas

// Implementa a estratégia first-fit, buscando um bloco já alocado com espaço
// suficiente.
static t_block find_block (t_block *last , size_t size);

// Separa o bloco de memória b de acordo com s. O bloco b deve exitir.
static void split_block(t_block b, size_t s);

// Adiciona um novo block à heap, retornando NULL se não foi possível.
static t_block extend_heap(t_block last, size_t s);

// Retorna o bloco do que contém *p.
static t_block get_block(void *p);

// Valida o endereço para o libera().
static int valid_addr(void *p);

// Junta um bloco com seu sucessor.
static t_block fusion(t_block b);



#endif