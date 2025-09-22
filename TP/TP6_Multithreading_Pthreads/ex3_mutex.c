// ex3_mutex.c — section critique protégée par mutex
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
static long long counter=0;
static pthread_mutex_t mtx=PTHREAD_MUTEX_INITIALIZER;
typedef struct{int id; long long iters;} Arg;
void* worker(void* p){ Arg*a=(Arg*)p; for(long long i=0;i<a->iters;++i){ pthread_mutex_lock(&mtx); counter++; pthread_mutex_unlock(&mtx);} return NULL; }
static double now(){ struct timespec ts; clock_gettime(CLOCK_MONOTONIC,&ts); return ts.tv_sec+ts.tv_nsec/1e9; }
int main(int argc,char**argv){
    int n=(argc>1)?atoi(argv[1]):8; long long iters=(argc>2)?atoll(argv[2]):2000000;
    pthread_t* th=malloc(sizeof(pthread_t)*n); Arg* args=malloc(sizeof(Arg)*n);
    double t0=now(); for(int i=0;i<n;++i){ args[i].id=i; args[i].iters=iters; pthread_create(&th[i],NULL,worker,&args[i]); }
    for(int i=0;i<n;++i) pthread_join(th[i],NULL); double t1=now();
    long long expected=(long long)n*iters; printf("expected=%lld obtained=%lld time=%.3fs\n", expected, counter, t1-t0); return 0;
}
