#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_LUGARES 100
#define NUM_INGRESSOS 80
#define NUM_PESSOAS 30
#define MAX_ING_POR_TRAN 3

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int ingressos_disponiveis = NUM_INGRESSOS;
int lugares_disp = MAX_LUGARES;

void *compra_ingresso(void *thread_id) {
    int tid = *((int *)thread_id);

    while(lugares_disp >0){
    pthread_mutex_lock(&mutex);

    int qnt_ing_desej = (rand() % MAX_ING_POR_TRAN ) + 1;

    if (ingressos_disponiveis >= qnt_ing_desej) {
        printf("Pessoa %d comprou %d ingresso(s).\n", tid, qnt_ing_desej);
        ingressos_disponiveis -= qnt_ing_desej;
        lugares_disp -= qnt_ing_desej;
    } else if(lugares_disp-qnt_ing_desej <0){
        printf("Pessoa %d: Desculpe, não há mais lugares disponíveis.\n", tid);
    } else{
        pthread_cond_wait(&cond,&mutex);
        if(qnt_ing_desej>ingressos_disponiveis){
            printf("Qnt de ingressos desejados muito alta/Nao ha mais lugares.\n");
        } else{
            printf("Pessoa %d comprou %d ingresso(s).\n", tid, qnt_ing_desej);
            ingressos_disponiveis -= qnt_ing_desej;
            lugares_disp -= qnt_ing_desej;
        }
    }

    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
    }
}

void *gera_ingressos(void *arg){
    while(lugares_disp > 0 && lugares_disp - ingressos_disponiveis >= 0){
        sleep(rand()%3);

        pthread_mutex_lock(&mutex);
        int qnt = 1;
        if (lugares_disp-qnt >=0){
            ingressos_disponiveis += qnt;
            printf("Disponibilizados %d ingressos\n",qnt);
            pthread_cond_broadcast(&cond);
        }
        pthread_mutex_unlock(&mutex);
    }
}

int main() {
    pthread_t threads[NUM_PESSOAS];
    int thread_ids[NUM_PESSOAS];
    pthread_t produtor;

    for (int i = 0; i < NUM_PESSOAS; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, compra_ingresso, (void *)&thread_ids[i]);
    }

    pthread_create(&produtor, NULL , gera_ingressos, NULL);

    for (int i = 0; i < NUM_PESSOAS; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_join(produtor,NULL);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return 0;
}
