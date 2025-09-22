// ex4_prodcons.c — producteur/consommateur (sémaphores + mutex)
#define _GNU_SOURCE
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifndef BUF_SIZE
#define BUF_SIZE 16
#endif
typedef int item_t;
typedef struct{
    item_t buf[BUF_SIZE]; int head, tail;
    sem_t empty, full; pthread_mutex_t mtx;
} BQ; static BQ q;
void bq_init(){ q.head=q.tail=0; sem_init(&q.empty,0,BUF_SIZE); sem_init(&q.full,0,0); pthread_mutex_init(&q.mtx,NULL); }
void bq_destroy(){ sem_destroy(&q.empty); sem_destroy(&q.full); pthread_mutex_destroy(&q.mtx); }
void push(item_t x){ sem_wait(&q.empty); pthread_mutex_lock(&q.mtx); q.buf[q.tail]=x; q.tail=(q.tail+1)%BUF_SIZE; pthread_mutex_unlock(&q.mtx); sem_post(&q.full); }
item_t pop(){ sem_wait(&q.full); pthread_mutex_lock(&q.mtx); item_t x=q.buf[q.head]; q.head=(q.head+1)%BUF_SIZE; pthread_mutex_unlock(&q.mtx); sem_post(&q.empty); return x; }
typedef struct{int id, items;} ProdArg; typedef struct{int id, items;} ConsArg;
void* producer(void* p){ ProdArg*a=(ProdArg*)p; for(int i=1;i<=a->items;++i){ int v=a->id*100000+i; push(v); fprintf(stderr,"[PROD %d] -> %d\n",a->id,v);} return NULL; }
void* consumer(void* p){ ConsArg*a=(ConsArg*)p; for(int i=0;i<a->items;++i){ int v=pop(); fprintf(stderr,"             [CONS %d] <- %d\n",a->id,v);} return NULL; }
int main(int argc,char**argv){
    int NP=(argc>1)?atoi(argv[1]):2, NC=(argc>2)?atoi(argv[2]):2, ITEMS=(argc>3)?atoi(argv[3]):50;
    bq_init(); pthread_t *tp=malloc(sizeof(pthread_t)*NP), *tc=malloc(sizeof(pthread_t)*NC);
    ProdArg* pa=malloc(sizeof(ProdArg)*NP); ConsArg* ca=malloc(sizeof(ConsArg)*NC);
    int total=NP*ITEMS, base=total/NC, rest=total%NC;
    for(int i=0;i<NP;++i){ pa[i].id=i; pa[i].items=ITEMS; pthread_create(&tp[i],NULL,producer,&pa[i]); }
    for(int i=0;i<NC;++i){ ca[i].id=i; ca[i].items=base+(i==NC-1?rest:0); pthread_create(&tc[i],NULL,consumer,&ca[i]); }
    for(int i=0;i<NP;++i) pthread_join(tp[i],NULL); for(int i=0;i<NC;++i) pthread_join(tc[i],NULL);
    bq_destroy(); fprintf(stderr,"OK Produit=%d Consommé=%d\n", total,total); return 0;
}
