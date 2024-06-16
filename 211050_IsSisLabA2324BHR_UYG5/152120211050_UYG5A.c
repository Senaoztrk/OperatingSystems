#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

enum body { HEAD, UBDY, LEG_, FOOT };
#define NUMOFITEMS 11

int count = 0;
pthread_mutex_t count_mutex;
sem_t sem_gomlek, sem_kravat, sem_sapka, sem_cuzdan;

void dressItem(int part, const char *name) {
    static const char *BODY_STRING[] = { "kafa ", "govde", "bacak", "ayak " };

    pthread_mutex_lock(&count_mutex);
    if (count == 0) {
        printf("%2d. \t\t\t\t\t--> Haydi baslayalim...\n", count);
    }

    count++;
    printf("%2d. (%s) %s ", count, BODY_STRING[part], name);

    int remaining_items = NUMOFITEMS - count;
    if (remaining_items > 0) {
        printf("\t--> Kalan oge sayisi: %2d\n", remaining_items);
    } else if (remaining_items == 0) {
        printf("\t--> Goreve hazirim!\n");
    } else {
        printf("\n");
    }
    
    pthread_mutex_unlock(&count_mutex);
}

void *funcHEAD(void *arg) {
    dressItem(HEAD, "Gozluk                   ");
    sem_wait(&sem_sapka); // Wait until 'Mont' is done
    dressItem(HEAD, "Sapka                    ");
    return NULL;
}

void *funcUBDY(void *arg) {
    sem_wait(&sem_gomlek); // Wait until 'Pantolon' is done
    dressItem(UBDY, "Gomlek                   ");
    sem_post(&sem_kravat); // Signal after 'Gomlek' is done
    dressItem(UBDY, "Kravat                   ");
    dressItem(UBDY, "Ceket                    ");
    dressItem(UBDY, "Mont                     ");
    sem_post(&sem_sapka); // Signal after 'Mont' is done
    return NULL;
}

void *funcLEG_(void *arg) {
    dressItem(LEG_, "Pantolon                 ");
    sem_post(&sem_gomlek); // Signal after 'Pantolon' is done
    sem_wait(&sem_kravat); // Wait until 'Kravat' is done
    dressItem(LEG_, "Kemer                    ");
    dressItem(LEG_, "Cuzdan                   ");
    sem_post(&sem_cuzdan); // Signal after 'Cuzdan' is done
    return NULL;
}

void *funcFOOT(void *arg) {
    dressItem(FOOT, "Corap                    ");
    sem_wait(&sem_cuzdan); // Wait until 'Cuzdan' is done
    dressItem(FOOT, "Ayakkabi                 ");
    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t thread_head, thread_ubdy, thread_leg_, thread_foot;

    pthread_mutex_init(&count_mutex, NULL);
    sem_init(&sem_gomlek, 0, 0);
    sem_init(&sem_kravat, 0, 0);
    sem_init(&sem_sapka, 0, 0);
    sem_init(&sem_cuzdan, 0, 0);

    pthread_create(&thread_head, NULL, funcHEAD, NULL);
    pthread_create(&thread_ubdy, NULL, funcUBDY, NULL);
    pthread_create(&thread_leg_, NULL, funcLEG_, NULL);
    pthread_create(&thread_foot, NULL, funcFOOT, NULL);

    pthread_join(thread_head, NULL);
    pthread_join(thread_ubdy, NULL);
    pthread_join(thread_leg_, NULL);
    pthread_join(thread_foot, NULL);

    pthread_mutex_destroy(&count_mutex);
    sem_destroy(&sem_gomlek);
    sem_destroy(&sem_kravat);
    sem_destroy(&sem_sapka);
    sem_destroy(&sem_cuzdan);

    return 0;
}
