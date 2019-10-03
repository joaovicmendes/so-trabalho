/*
    Universidade Federal de São Carlos – UFSCar
    Departamento de Computação

    Sistemas Operacionais
    Prof. Dr. Fredy Valente
    Prof. Dr. Hélio Crestana Guardia

    Aluno: João Victor Mendes Freire (758943)
    e Guilherme Locca Salomão (758569)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "./headers/auxiliares.h"
#include "./headers/interpretador.h"
#include "./headers/lista.h"
#include "./headers/sinal.h"

Contexto estado;

int main(int argc, char **argv)
{
    // Buffer de leitura e tamanho
    size_t buff_size;
    char *buffer  = NULL;
    char *comando = NULL;

    // Comando e lista de argumentos
    char **args = NULL;

    // Variáveis úteis
    int contador;
    estado.fg = getpid();
    estado.pgid = getpid();
    estado.num_processos = 0;
    estado.pwd = getcwd(NULL, TAM_PWD);
    inicializa_lista(&estado.processos);

    // Definindo como os sinais devem ser tratados
    signal(SIGINT,  SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGCHLD, sig_chld);

    // Loop até estar em foreground
    while (tcgetpgrp(STDIN_FILENO) != (estado.pgid = getpgrp()))
        kill(- estado.pgid, SIGTTIN);
    
    // Colocando o shell no seu próprio parent group
    if (setpgid(estado.pgid, estado.pgid) < 0)
    {
        perror ("Erro: Não conseguiu colocar o shell no seu próprio Parent Group\n");
        exit(1);
    }

    // Tomando controle do terminal
    tcsetpgrp(STDIN_FILENO, estado.pgid);

    for (;;)
    {
        // Imprimindo pwd + $
        printf("\033[1;31m[%s]$\033[0m ", estado.pwd);

        // Lendo linha de comando
        buffer = NULL;
        getline(&buffer, &buff_size, stdin);

        comando = remove_espacos(buffer);
        if (strcmp(comando, "\n") != 0)
        {
            contador = 2;
            for (int i = 0; i < strlen(comando); i++)
            {
                if (comando[i] == ' ')
                    contador++;
            }

            // Alocando vetor de argumentos
            args = (char **) malloc_safe(contador * sizeof(char *));
            
            args[0] = strtok(comando, " \n");
            for (int i = 1; i < contador; i++)
                args[i] = strtok(NULL, " \n");

            // Atualiza estado dos processos
            atualiza_processo(&estado);
            
            // Executa comando
            interpreta(contador, args, &estado);

            free(args), args = NULL;
        }

        if (estado.processos == NULL)
            estado.num_processos = 0;

        free(buffer), buffer = NULL;
        free(comando), comando = NULL;
    }

    free(estado.pwd), estado.pwd = NULL;

    return 0;
}
