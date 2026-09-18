// Disciplina: Sistemas Distribuiídos

// Problema: Produtor-Consumidor

// Integrantes: Davi Braga, Otávio Andrade e Vitório Marcos
// Centro Federal de Educação Tecnológica de Minas Gerais (CEFET-MG)
// Belo Horizonte, 2026

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <time.h>

int *buffer; // Buffer como ponteiro para simular o acesso compartilhado à memória
int N, Np, Nc; // N = tamanho do buffer; Np = Número de produtores; Nc = Número de consumidores
int in = 0, out = 0; // Controladores para acesso às posições do buffer, in para o produtor e out para o consumidor
int current_occupancy = 0; // Contador de ocupação total do buffer

int M = 100000; // Número de ítens a serem produzidos em cada execução. Ao atingir esse número, deve interromper a execução

// Contadores para geração dos relatórios
int items_produced = 0;
int items_consumed = 0;

// Semáforos e mutexes para controle de concorrência
pthread_mutex_t prod_lock;
pthread_mutex_t cons_lock;
sem_t empty_slots;
sem_t full_slots;
sem_t mutex;

// Variáveis para geração do relatório de ocupação do buffer durante a execução do algorítmo
int *occupancy_log;
int log_index = 0;

bool is_prime(int n) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;
    for (int i = 5; i * i <= n; i = i + 6)
        if (n % i == 0 || n % (i + 2) == 0)
            return false;
    return true;
}

void* producer(void* arg) {
    while (1) {
        pthread_mutex_lock(&prod_lock);
        if (items_produced >= M) {
            pthread_mutex_unlock(&prod_lock);
            break;
        }
        items_produced++;
        pthread_mutex_unlock(&prod_lock);

        int num = (rand() % 10000000) + 1;

        sem_wait(&empty_slots);
        sem_wait(&mutex);

        buffer[in] = num;
        in = (in + 1) % N;
        current_occupancy++;
        occupancy_log[log_index++] = current_occupancy; 

        sem_post(&mutex);
        sem_post(&full_slots);
    }
    return NULL;
}

void* consumer(void* arg) {
    while (1) {
        pthread_mutex_lock(&cons_lock);
        if (items_consumed >= M) {
            pthread_mutex_unlock(&cons_lock);
            break;
        }
        items_consumed++;
        pthread_mutex_unlock(&cons_lock);

        sem_wait(&full_slots);
        sem_wait(&mutex);

        int num = buffer[out];
        out = (out + 1) % N;
        current_occupancy--;
        occupancy_log[log_index++] = current_occupancy; 

        sem_post(&mutex);
        sem_post(&empty_slots);

        is_prime(num);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 4) return 1;

    Np = atoi(argv[1]);
    Nc = atoi(argv[2]);
    N = atoi(argv[3]);

    srand(time(NULL));

    buffer = malloc(N * sizeof(int));
    occupancy_log = malloc(2 * M * sizeof(int));

    sem_init(&empty_slots, 0, N);
    sem_init(&full_slots, 0, 0);
    sem_init(&mutex, 0, 1);
    pthread_mutex_init(&prod_lock, NULL);
    pthread_mutex_init(&cons_lock, NULL);

    pthread_t prods[Np];
    pthread_t cons[Nc];

    for (int i = 0; i < Np; i++) pthread_create(&prods[i], NULL, producer, NULL);
    for (int i = 0; i < Nc; i++) pthread_create(&cons[i], NULL, consumer, NULL);

    for (int i = 0; i < Np; i++) pthread_join(prods[i], NULL);
    for (int i = 0; i < Nc; i++) pthread_join(cons[i], NULL);

    // Salva o log de ocupação silenciosamente
    char filename[50];
    sprintf(filename, "logsOcupacao/logOcupacao_Np%d_Nc%d_N%d.csv", Np, Nc, N);
    FILE *f = fopen(filename, "w");
    if (f) {
        fprintf(f, "Operacao,Ocupacao\n");
        for (int i = 0; i < 2 * M; i++) {
            fprintf(f, "%d,%d\n", i, occupancy_log[i]);
        }
        fclose(f);
    }

    free(buffer);
    free(occupancy_log);
    sem_destroy(&empty_slots);
    sem_destroy(&full_slots);
    sem_destroy(&mutex);
    pthread_mutex_destroy(&prod_lock);
    pthread_mutex_destroy(&cons_lock);

    return 0;
}