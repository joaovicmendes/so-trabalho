#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "./headers/memoria.h"


void *aloca_mem(size_t tam)
{
    if (tam == 0)
        return NULL;

    void *p = sbrk(0);
    void *request = sbrk(size);
    if (request == (void*) -1) 
        return NULL;
    else 
    {
        assert(p == request);
        return p;
    }
}