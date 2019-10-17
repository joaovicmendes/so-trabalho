#include <stdio.h>
#include <unistd.h>
#include "./headers/memoria.h"

void *aloca_mem(size_t size)
{
    t_block b, last;
    size_t s = align4(size);

    if (base) 
    {
        // Procura um bloco
        last = base;
        b = find_block(&last, s);
        if (b) 
        {
            // Se pode ser dividido
            if ((b->size - s) >= (BLOCK_SIZE + 4))
                split_block(b,s);

            b->free = 0;
        } 
        else 
        {
            // Se não tem bloco que caiba, extende a heap
            b = extend_heap (last, s);
            if (!b)
                return NULL;
        }
    } 
    else 
    {
        // Na primeira chamada
        b = extend_heap(NULL, s);
        if (!b)
            return NULL;
        base = b;
    }

    return b->data;
}

void libera(void *p)
{
    t_block b;

    if (valid_addr(p))
    {
        b = get_block(p);
        b->free = 1;

        // Junta com espaço anterior, se possível
        if (b->prev && b->prev->free)
            b = fusion(b->prev);

        // Junta com o próximo, se possível
        if (b->next)
            fusion(b);
        else
        {
            // Libera o fim da heap
            if (b->prev)
            b->prev->next = NULL;
            else
                base = NULL;
            
            brk(b);
        }
    }
}

static t_block find_block(t_block *last, size_t size)
{
    t_block b = base;

    while (b && !(b->free && b->size >= size)) 
    {
        *last = b;
        b = b->next;
    }

    return (b);
}

static void split_block(t_block b, size_t s)
{
    t_block new;
    new = (t_block)(b->data + s);
    new->size = b->size - s - BLOCK_SIZE;
    new->next = b->next;
    new->prev = b;
    new->free = 1;
    new->ptr = new->data;

    b->size = s;
    b->next = new;
    if (new->next)
        new->next->prev = new;
}

static t_block extend_heap(t_block last, size_t s)
{
    int sb;
    t_block b = sbrk(0);

    sb = (int)sbrk(BLOCK_SIZE + s);

    if (sb < 0)
        return (NULL);
    
    b->size = s;
    b->next = NULL;
    b->prev = last;
    b->ptr = b->data;
    if (last)
        last->next = b;
    b->free = 0;
    
    return (b);
}

static t_block get_block(void *p)
{
    char *tmp;
    tmp = p;
    return (p = tmp -= BLOCK_SIZE);
}

static int valid_addr(void *p)
{
    if (base)
    {
        if ( p > base && p < sbrk(0))
        {
            return (p == (get_block(p))->ptr);
        }
    }

    return 0;
}

static t_block fusion(t_block b)
{
    if (b->next && b->next->free)
    {
        b->size += BLOCK_SIZE + b->next->size;
        b->next = b->next->next;

        if (b->next)
            b->next ->prev = b;
    }
    return b;
}
