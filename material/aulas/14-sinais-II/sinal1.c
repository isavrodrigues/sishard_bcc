#include <stdio.h>
#include <unistd.h>
#include <signal.h>


volatile int cont = 0;

void sig_handler(int num) {
    cont++;
    printf("Chamou Ctrl+C (%d)\n", cont);
    if(cont==3) exit(0);
}

int main() {
    /* TODO: registre a função sig_handler
     * como handler do sinal SIGINT
     */
    /* Dentro da main, uma das primeiras coisas que fazemos é
    registrar nosso handler */
    struct sigaction s;
    s.sa_handler = sig_handler; // aqui vai a função a ser executada
    sigemptyset(&s.sa_mask);
    s.sa_flags = 0;

    sigaction(SIGINT, &s, NULL);
    printf("Meu pid: %d\n", getpid());

    while(1) {
        sleep(1);
    }
    return 0;
}

// tem que armazenar pra dar kill, tem que apagar os arquivos que ficaram pendentes