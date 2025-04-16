#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_REGION_SIZE 200

int main() {
    int x; 
    int n; 
    int k; 
    int *array; 
    int region_size; 

    printf("Qual o elemento a ser buscado? ");
    scanf("%d", &x);

    printf("Qual o tamanho do array? ");
    scanf("%d", &n);

    array = (int *)malloc(n * sizeof(int));
    if (array == NULL) {
        perror("Falhou em alocar memoria");
        return 1;
    }

    printf("Digite os %d elementos do array:\n", n);
    for (int i = 0; i < n; i++) {
        scanf("%d", &array[i]);
    }

    printf("Qual a quantidade de processos? ");
    scanf("%d", &k);

    region_size = n / k;

    pid_t child_pids[k];
    int child_results[k];
    for (int i = 0; i < k; i++) {
        child_results[i] = -1;
    }

    for (int i = 0; i < k; i++) {
        pid_t pid = fork();
        
        if (pid == -1) {
            perror("fork falhou");
            exit(1);
        } else if (pid == 0) {
            int start = i * region_size;
            int end = start + region_size;
            
            printf("Criou filho %d com PID=%d, r=[%d,%d)\n", i, getpid(), start, end);
           
            sleep(5);

            int result = -1;
            for (int j = start; j < end; j++) {
                if (array[j] == x) {
                    result = j - start;
                    break;
                }
            }
            
            free(array);
            
            if (result >= 0) {
                exit(result);
            } else {
                exit(255); 
            }
        } else {
            child_pids[i] = pid;
        }
    }

    printf("Processo pai esperando os filhos finalizarem...\n");
    
    int found_index = -1;
    int found_process = -1;

    for (int i = 0; i < k; i++) {
        int status;
        pid_t child_pid = wait(&status);
        
        int process_index = -1;
        for (int j = 0; j < k; j++) {
            if (child_pids[j] == child_pid) {
                process_index = j;
                break;
            }
        }
        
        if (process_index == -1) {
            continue; 
        }
        
        if (WIFEXITED(status)) {
            int return_value = WEXITSTATUS(status);
            
            if (return_value == 255) {
                child_results[process_index] = -1;
            } else {
                child_results[process_index] = process_index * region_size + return_value;
            }
            
            printf("Processo %d com PID=%d finalizou retornando %d\n", 
                   process_index, child_pid, child_results[process_index]);
           
                   
            //parte em que o chatgpt me ajudou a escolher o primeiro indice que ocorre o valor esperado
            if (child_results[process_index] != -1 && 
                (found_index == -1 || child_results[process_index] < found_index)) {
                found_index = child_results[process_index];
                found_process = process_index;
            }
            //fim da parte que o chatgpt gerou o codigo
        }
    }
    
    if (found_index != -1) {
        printf("Elemento %d encontrado pelo processo %d no indice %d!\n", 
               x, found_process, found_index);
    } else {
        printf("Elemento não encontrado!\n");
    }
    
    free(array);
    return 0;
}