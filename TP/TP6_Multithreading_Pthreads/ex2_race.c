// ex2_race.c — condition de course volontaire
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
static long long counter=0;
typedef struct{int id; long long iters;} Arg;
void* worker(void* p){ Arg*a=(Arg*)p; for(long long i=0;i<a->iters;++i){ counter++; } return NULL; }
int main(int argc,char**argv){
    int n=(argc>1)?atoi(argv[1]):8; long long iters=(argc>2)?atoll(argv[2]):2000000;
    pthread_t* th=malloc(sizeof(pthread_t)*n); Arg* args=malloc(sizeof(Arg)*n);
    for(int i=0;i<n;++i){ args[i].id=i; args[i].iters=iters; pthread_create(&th[i],NULL,worker,&args[i]); }
    for(int i=0;i<n;++i) pthread_join(th[i],NULL);
    long long expected=(long long)n*iters;
    printf("expected=%lld obtained=%lld%s\n", expected, counter, (counter!=expected)?" (RACE!)":"");
    return 0;
}
