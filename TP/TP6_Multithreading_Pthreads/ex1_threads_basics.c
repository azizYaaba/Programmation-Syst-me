// ex1_threads_basics.c
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct { int id; int work; } Arg;

void* worker(void* p){
    Arg* a=(Arg*)p; long sum=0;
    for(int i=0;i<a->work;++i) sum += (a->id+1);
    printf("[thread %d] sum=%ld\n", a->id, sum);
    return (void*)(intptr_t)sum;
}

int main(int argc,char**argv){
    int n=(argc>1)?atoi(argv[1]):4;
    int work=(argc>2)?atoi(argv[2]):1000000;
    pthread_t* th=malloc(sizeof(pthread_t)*n);
    Arg* args=malloc(sizeof(Arg)*n);
    for(int i=0;i<n;++i){ args[i].id=i; args[i].work=work; pthread_create(&th[i],NULL,worker,&args[i]); }
    long total=0; for(int i=0;i<n;++i){ void* ret; pthread_join(th[i],&ret); total+=(long)(intptr_t)ret; }
    printf("[main] total=%ld\n", total);
    free(th); free(args); return 0;
}
