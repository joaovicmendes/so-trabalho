#include <stdio.h>
#include <stdlib.h>
#include "./headers/auxiliares.h"

void *malloc_safe(unsigned nbytes)
{
    void *p;
    p = malloc(nbytes);
    if (p == NULL)
    {
        printf("Não foi possível alocar memória\n");
        exit(EXIT_FAILURE);
    }
    return p;
}
