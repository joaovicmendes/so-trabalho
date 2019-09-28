#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "./headers/auxiliares.h"

void interpreta(int argc, char **argv, Contexto *estado)
{
    if (strcmp(argv[0], "exit") == 0)
    {
        exit(0);
    }
    else if (strcmp(argv[0], "pwd") == 0)
    {
        printf("%s\n", estado->pwd);
    }
    else if (strcmp(argv[0], "cd") == 0)
    {

    }
    else if (strcmp(argv[0], "bg") == 0)
    {

    }
    else if (strcmp(argv[0], "fg") == 0)
    {

    }
    else if (strcmp(argv[0], "jobs") == 0)
    {

    }
    else
    {
    }
}
