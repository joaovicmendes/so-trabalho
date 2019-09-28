// Universidade Federal de São Carlos
// Setembro de 2019
// Alunos: João Victor Mendes Freire e Guilherme Locca Salomão
// 
// Os limites parecem ser definidos no software, mas obviamente limitados pelo 
// hardware. Pesquisando um pouco, encontramos que o número máximo de processos
// suportado por um computador com a arquitetura x86 é 32.767 e para a x86_64 é
// 4.194.303, sendo limitado também pela quantidade de memória ram disponível.
//
// Os seguintes comandos foram executados em uma Máquina Virtual rodando Linux
// Manjaro, com 2GB de ram e apenas um processador (x86_64). Como o limite de 
// hardware é superior aos valores retornados, inferimos que este número é uma
// limitação imposta pelo SO
//
// cat /proc/sys/kernel/pid_max
//   32.768
// cat /proc/sys/kernel/threads-max
//   15.811

// Para executar o programa a seguir basta compilar com
//   gcc main.c
// Depois executar
//   ./a.out 1000
// Os tempos obtidos foram de aproximadamente 0.15s para criar 1000 processos
// e 0.02s para criar 1000 threads.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <pthread.h>

float tempo_processos(int qtd);

float tempo_threads(int qtd);

void *func_thread();

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Argumentos insuficientes\n");
        exit(1);
    }

    int qtd = atoi(argv[1]);
    float t_processo = tempo_processos(qtd);
    float t_thread = tempo_threads(qtd);

    printf("Tempo gasto para criar %d processos: %f\n", qtd, t_processo);
    printf("Tempo gasto para criar %d threads: %f\n", qtd, t_thread);

    return 0;
}

float tempo_processos(int qtd)
{
    struct timespec inic, fim;
    float etime;

    // Marcando horário inicial
    if (clock_gettime(CLOCK_REALTIME, &inic) < 0)
       printf("Erro em clock_gettime");

    // Criando os 'qtd' processos
    pid_t pid;
    for (int i = 0; i < qtd; i++)
    {
        // Se for o processo pai, continua no laço
        if ((pid = fork()) > 0);
        // Se não, encerra o processo
        else
            exit(0);
    }

    // Pai esperando que todos os filhos se encerrem
    for (int i = 0; i < qtd; i++)
    {
        wait(NULL);
    }
   
    // Marcando horário final
    if (clock_gettime(CLOCK_REALTIME, &fim) < 0)
       printf("Erro em clock_gettime");

   // Retornando tempo gasto na operação
    return etime = (fim.tv_sec + fim.tv_nsec/1000000000.) - (inic.tv_sec + inic.tv_nsec/1000000000.);
}

float tempo_threads(int qtd)
{
    struct timespec inic, fim;
    float etime;

    // Marcando horário inicial
    if (clock_gettime(CLOCK_REALTIME, &inic) < 0)
       printf("Erro em clock_gettime");

    pthread_t threads[qtd];
    // Criando as threads
    for (int i = 0; i < qtd; i++)
    {
        pthread_create(&threads[i], NULL, func_thread, NULL);
    }
    // Esperando as threads filhas se encerrarem
    for (int i = 0; i < qtd; i++)
    {
        pthread_join(threads[i], NULL);
    }
   
    // Marcando horário final
    if (clock_gettime(CLOCK_REALTIME, &fim) < 0)
       printf("Erro em clock_gettime");

   // Retornando tempo gasto na operação
    return etime = (fim.tv_sec + fim.tv_nsec/1000000000.) - (inic.tv_sec + inic.tv_nsec/1000000000.);
}

// Função na qual as threads irão começar
void *func_thread()
{
    pthread_exit(0);
}