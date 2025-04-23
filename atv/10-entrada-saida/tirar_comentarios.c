/* POSIX headers */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

/* Standard C */
#include <stdio.h>



int main(int argc, char *argv[])
{   
    if(argc != 3){
        printf("Use o programa assim: ./tirar_comentarios caso1.c resp_caso1.c\n");
    }
    else{
    //O_RDONLY = so le
    int fd1 = open(argv[1], O_RDONLY);
    //O_WRONLY = abrir/escrita
    //O_CREAT = criar se nao existir
    //0700 = da permissao escrita leitura e execucao
    int fd2 = open(argv[2], O_WRONLY | O_CREAT, 0700);

    int ret;
    char x[2];

    // implemente aqui
    estado1: ret = read(fd1, x, 1);
            if(ret == 0) goto estado_fim;
            if(x[0] == '/') goto estado2;
            else write(fd2, x, 1);
            goto estado1;


    estado2: ret = read(fd1, x, 1);
            if(ret == 0) goto estado_fim;
            if(x[0] == '/') goto estado3;
            else if(x[0] == '*') goto estado4;
            else{
                write(fd2, "/", 1);
                write(fd2, x, 1);
                goto estado1;
            }

    estado3:ret = read(fd1, x, 1);
            if(ret == 0) goto estado_fim;
            if(x[0] == '\n'){
            write(fd2, "\n", 1);
            goto estado1;}
            else goto estado3;

    estado4: ret = read(fd1, x, 1);
            if(ret == 0) goto estado_fim;
            if(x[0] == '*') goto estado5;
            else goto estado4;


    estado5:ret = read(fd1, x, 1);
            if(ret == 0) goto estado_fim;
            if(x[0] == '/') goto estado1;
            else goto estado4;

    estado_fim:

    
    close(fd1);
    close(fd2);
    }
    
    return 0;
}
