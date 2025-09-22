// ex5_barrier.c — barrière (phases)
#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
static pthread_barrier_t barrier;
static void msleep(int ms){ struct timespec ts={ms/1000,(ms%1000)*1000000L}; nanosleep(&ts,NULL); }
void* worker(void* p){ long id=(long)p; for(int ph=1; ph<=3; ++ph){ printf("[thread %ld] phase %d: travail\n", id, ph); msleep(50 + rand()%150); printf("[thread %ld] phase %d: attente\n", id, ph); pthread_barrier_wait(&barrier);} return NULL; }
int main(int argc,char**argv){ int n=(argc>1)?atoi(argv[1]):4; pthread_t* th=malloc(sizeof(pthread_t)*n); pthread_barrier_init(&barrier,NULL,n); for(long i=0;i<n;++i) pthread_create(&th[i],NULL,worker,(void*)i); for(int i=0;i<n;++i) pthread_join(th[i],NULL); pthread_barrier_destroy(&barrier); free(th); return 0; }
