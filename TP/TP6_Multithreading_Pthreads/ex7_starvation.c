// ex7_starvation.c — démonstration d'inanition (starvation) approximative
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static volatile int running = 1;

void* hogger(void* _){ // thread "gourmand"
    while(running){
        pthread_mutex_lock(&mtx);
        // Section critique très courte mais prise ultra fréquemment
        // (simulateur: dors quasi pas)
        pthread_mutex_unlock(&mtx);
        // pas de yield => monopolise
    }
    return NULL;
}

void* victim(void* _){ // thread "victime"
    long successes=0, fails=0;
    for(int i=0;i<500000; ++i){
        if(pthread_mutex_trylock(&mtx)==0){
            successes++;
            pthread_mutex_unlock(&mtx);
        }else{
            fails++;
            // dormir légèrement pour accentuer l'injustice
            usleep(50);
        }
    }
    printf("[victime] successes=%ld fails=%ld (taux reussite ~%.2f%%)\n", successes, fails, 100.0*successes/(successes+fails));
    return NULL;
}

int main(int argc,char**argv){
    int hoggers=(argc>1)?atoi(argv[1]):4; // nb de threads gourmands
    int victims=(argc>2)?atoi(argv[2]):2; // nb de victimes
    pthread_t th[64]; int idx=0;

    for(int i=0;i<hoggers;i++) pthread_create(&th[idx++],NULL, hogger, NULL);
    for(int i=0;i<victims;i++) pthread_create(&th[idx++],NULL, victim, NULL);

    sleep(2); running=0; // arrêter les hoggers
    for(int i=0;i<idx;i++) pthread_join(th[i],NULL);

    puts("Suggestion de correction : insérer des yields (sched_yield), backoff, ou utiliser des condvars.");
    return 0;
}
