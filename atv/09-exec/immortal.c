/* POSIX headers */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
/* Standard C */
#include <stdio.h>

int main(int argc, char *argv[]){
    if(argc != 2){
        printf("Use o programa assim: $ ./immortal ./path/para/algum/programa\n");
    }
    else{


    //eu passo o programa que eu quero chamar
        char *args[] = {argv[1], NULL};
        
        if (execvp(args[0], args) == -1) {
            perror("execvp failed!");
            exit(EXIT_FAILURE);
        }
    }
}
