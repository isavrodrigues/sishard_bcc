
//sempre usar o include de thread
#include <pthread.h>
#include <stdio.h>

//entra um ponteiro e sai um ponteiro 
void *minha_thread(void *arg) {
    printf("Hello thread!\n");
    return NULL;
}


int main() {
    pthread_t tid;
    
    int error = pthread_create(&tid, NULL, minha_thread, NULL);

    printf("Hello main\n");
    
    pthread_join(tid, NULL);
    
    return 0;
}
