#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

#define BUFFER_SIZE 20

// Função simples para verificar se um número é primo
int eh_primo(int n) {
    if (n <= 1) return 0;
    if (n <= 3) return 1;
    if (n % 2 == 0 || n % 3 == 0) return 0;
    for (int i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) return 0;
    }
    return 1;
}

void produtor(int write_fd, int total_numeros) {
    int n_atual = 1;
    char buffer[BUFFER_SIZE];

    srand(time(NULL) ^ getpid()); // Semente para números aleatórios

    for (int i = 0; i < total_numeros; i++) {
        // Envia o número atual formatado em uma string de tamanho fixo (20 bytes)
        snprintf(buffer, BUFFER_SIZE, "%d", n_atual);
        write(write_fd, buffer, BUFFER_SIZE);

        // Se for o primeiro número (1), o próximo ganha um delta aleatório [1, 100]
        int delta = (rand() % 100) + 1;
        n_atual += delta;
    }

    // Após gerar todos, envia 0 para sinalizar o fim ao consumidor
    snprintf(buffer, BUFFER_SIZE, "%d", 0);
    write(write_fd, buffer, BUFFER_SIZE);

    close(write_fd);
    exit(0);
}

void consumidor(int read_fd) {
    char buffer[BUFFER_SIZE];
    
    while (1) {
        // Lê exatamente 20 bytes do pipe
        ssize_t bytes_lidos = read(read_fd, buffer, BUFFER_SIZE);
        if (bytes_lidos <= 0) break;

        int numero = atoi(buffer);

        // O consumidor deve terminar quando receber o número 0
        if (numero == 0) {
            printf("[Consumidor] Sinal de término (0) recebido. Encerrando...\n");
            break;
        }

        // Verifica se é primo e imprime o resultado
        if (eh_primo(numero)) {
            printf("[Consumidor] Número %d é PRIMO.\n", numero);
        } else {
            printf("[Consumidor] Número %d NÃO é primo.\n", numero);
        }
    }

    close(read_fd);
    exit(0);
}

int main(int argc, char *argv[]) {
    // Redireciona a saída do terminal para um arquivo chamado "execucao.log"
    freopen("execucao.log", "w", stdout);

    if (argc < 2) {
        printf("Uso: %s <quantidade_de_numeros>\n", argv[0]);
        return 1;
    }

    int total_numeros = atoi(argv[1]);
    int pipe_fd[2];

    // 1. Criação do Pipe antes do fork()
    if (pipe(pipe_fd) == -1) {
        perror("Erro ao criar o pipe");
        return 1;
    }

    // 2. Duplicação do processo com fork()
    pid_t pid = fork();

    if (pid < 0) {
        perror("Erro no fork()");
        return 1;
    }

    if (pid == 0) {
        // --- PROCESSO FILHO (Consumidor) ---
        close(pipe_fd[1]); // Fecha a ponta de escrita que não será usada
        consumidor(pipe_fd[0]);
    } else {
        // --- PROCESSO PAI (Produtor) ---
        close(pipe_fd[0]); // Fecha a ponta de leitura que não será usada
        produtor(pipe_fd[1], total_numeros);
    }

    return 0;
}