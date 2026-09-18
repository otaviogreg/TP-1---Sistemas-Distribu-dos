#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

// Variável global para controle de fluxo
volatile sig_atomic_t executando = 1;

// Define signal handlers para capturar e reagir a sinais[cite: 2]
void handler_sigusr1(int sig) {
    printf("\n[Handler] Sinal SIGUSR1 (%d) recebido! Executando rotina 1...\n", sig);
}

void handler_sigusr2(int sig) {
    printf("\n[Handler] Sinal SIGUSR2 (%d) recebido! Executando rotina 2...\n", sig);
}

void handler_sigterm(int sig) {
    printf("\n[Handler] Sinal SIGTERM (%d) recebido! Encerrando o processo...\n", sig);
    // A signal handler deve terminar o processo[cite: 2]
    exit(0); 
}

int main(int argc, char *argv[]) {
    // Recebe a forma de esperar como parâmetro[cite: 2]
    if (argc != 2 || (strcmp(argv[1], "busy") != 0 && strcmp(argv[1], "blocking") != 0)) {
        printf("Uso: %s <busy|blocking>\n", argv[0]);
        return 1;
    }

    printf("Processo Recebedor iniciado. PID: %d\n", getpid());
    printf("Modo de espera configurado: %s\n", argv[1]);
    printf("Aguardando sinais... (Envie 10 para SIGUSR1, 12 para SIGUSR2, 15 para SIGTERM)\n");

    // Registro dos handlers
    signal(SIGUSR1, handler_sigusr1);
    signal(SIGUSR2, handler_sigusr2);
    signal(SIGTERM, handler_sigterm); 

    if (strcmp(argv[1], "busy") == 0) {
        // Busy wait: a thread "gira" (executa) sem parar
        printf("Entrando em loop de espera ativa (CPU em uso)...\n");
        while (executando) {
            // Loop vazio intencional consumindo ciclos de CPU
        }
    } else {
        // Blocking wait: processo aguarda sem consumir CPU
        printf("Entrando em espera bloqueante (CPU ociosa)...\n");
        while (executando) {
            pause(); // Suspenso até a chegada de um sinal
        }
    }

    return 0;
}
