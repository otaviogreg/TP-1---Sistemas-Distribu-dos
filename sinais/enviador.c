#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    // Verifica se os parâmetros foram passados corretamente
    if (argc != 3) {
        printf("Uso: %s <PID> <SINAL>\n", argv[0]);
        return 1;
    }
    
    pid_t pid = atoi(argv[1]);
    int sig = atoi(argv[2]);

    // Verifica se o processo destino existe
    // Enviar o sinal 0 não afeta o processo, mas retorna erro se ele não existir
    if (kill(pid, 0) == -1) {
        if (errno == ESRCH) {
            printf("Erro: O processo %d não existe.\n", pid);
        } else {
            perror("Erro ao verificar o processo");
        }
        return 1;
    }

    // Envia o sinal indicado[cite: 2]
    if (kill(pid, sig) == 0) {
        printf("Sinal %d enviado com sucesso para o processo %d.\n", sig, pid);
    } else {
        perror("Erro ao enviar o sinal");
    }
    
    return 0;
}
