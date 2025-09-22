// ex8_condvar.c — variables de condition pour remplacer le polling
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  cv  = PTHREAD_COND_INITIALIZER;
static int ready = 0;
static int data  = 0;

void* worker(void* _){
    // Travail simulé
    sleep(1);
    pthread_mutex_lock(&mtx);
    data = 42;
    ready = 1;
    pthread_cond_signal(&cv); // réveiller un attendant
    pthread_mutex_unlock(&mtx);
    return NULL;
}

int main(){
    pthread_t t; pthread_create(&t,NULL,worker,NULL);

    pthread_mutex_lock(&mtx);
    while(!ready){                // éviter "spurious wakeups"
        pthread_cond_wait(&cv,&mtx);
    }
    printf("Donnée prête = %d\n", data);
    pthread_mutex_unlock(&mtx);

    pthread_join(t,NULL);
    return 0;
}
