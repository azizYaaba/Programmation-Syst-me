// ex6_deadlock.c — démonstration d'interblocage (NE PAS FAIRE EN PRODUCTION)
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

pthread_mutex_t A = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t B = PTHREAD_MUTEX_INITIALIZER;

void* t1(void* _){
    pthread_mutex_lock(&A);
    fprintf(stderr,"[T1] A verrouillé\n");
    usleep(100000); // 100 ms
    fprintf(stderr,"[T1] tente de verrouiller B...\n");
    pthread_mutex_lock(&B);
    fprintf(stderr,"[T1] B verrouillé\n");
    pthread_mutex_unlock(&B);
    pthread_mutex_unlock(&A);
    return NULL;
}

void* t2(void* _){
    pthread_mutex_lock(&B);
    fprintf(stderr,"[T2] B verrouillé\n");
    usleep(100000);
    fprintf(stderr,"[T2] tente de verrouiller A...\n");
    pthread_mutex_lock(&A);
    fprintf(stderr,"[T2] A verrouillé\n");
    pthread_mutex_unlock(&A);
    pthread_mutex_unlock(&B);
    return NULL;
}

int main(){
    pthread_t x,y;
    pthread_create(&x,NULL,t1,NULL);
    pthread_create(&y,NULL,t2,NULL);
    pthread_join(x,NULL);
    pthread_join(y,NULL);
    return 0;
}
/*
Attendu : le programme se bloque (deadlock) après les messages "tente de verrouiller ...".
Correction : imposer un ORDRE GLOBAL, ex: toujours verrouiller A puis B dans tous les threads.
Ou utiliser trylock/timedlock et rollback.
*/
