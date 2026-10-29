/*
 * Producer-Consumer: Tandoori Roti Counter at a Wedding
 *
 * Producers : chefs who make rotis in the tandoor
 * Consumers : guests waiting at the counter
 * Buffer    : the counter plate, which holds only a few hot rotis
 *
 * Rules:
 *  - A chef must wait if the plate is full        (semaphore: empty)
 *  - A guest must wait if there is no roti ready  (semaphore: full)
 *  - Only one person touches the plate at a time  (mutex)
 *
 * Compile: gcc roti_counter.c -o roti_counter -pthread
 * Run    : ./roti_counter
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define PLATE_SIZE      5   /* max rotis on the counter          */
#define NUM_CHEFS       2
#define NUM_GUESTS      4
#define ROTIS_PER_CHEF  6   /* each chef makes 6 rotis           */
#define ROTIS_PER_GUEST 3   /* each guest eats 3 rotis           */
/* total made = 2*6 = 12, total eaten = 4*3 = 12 (so nobody waits forever) */

int plate[PLATE_SIZE];
int in = 0, out = 0, count = 0;
int roti_no = 0;            /* serial number of the next roti    */

sem_t empty_slots;          /* free spaces on the plate          */
sem_t full_slots;           /* hot rotis available               */
pthread_mutex_t plate_lock = PTHREAD_MUTEX_INITIALIZER;

void *chef(void *arg) {
    int id = *(int *)arg;
    for (int i = 0; i < ROTIS_PER_CHEF; i++) {
        usleep(200000 + rand() % 300000);     /* time to bake a roti */

        sem_wait(&empty_slots);               /* wait if plate is full */
        pthread_mutex_lock(&plate_lock);

        int r = ++roti_no;
        plate[in] = r;
        in = (in + 1) % PLATE_SIZE;
        count++;
        printf("Chef %d  placed roti #%-2d  | rotis on plate: %d\n", id, r, count);

        pthread_mutex_unlock(&plate_lock);
        sem_post(&full_slots);                /* one more roti ready */
    }
    printf("Chef %d  is done for the night.\n", id);
    return NULL;
}

void *guest(void *arg) {
    int id = *(int *)arg;
    for (int i = 0; i < ROTIS_PER_GUEST; i++) {
        sem_wait(&full_slots);                /* wait if no roti ready */
        pthread_mutex_lock(&plate_lock);

        int r = plate[out];
        out = (out + 1) % PLATE_SIZE;
        count--;
        printf("    Guest %d took roti #%-2d | rotis on plate: %d\n", id, r, count);

        pthread_mutex_unlock(&plate_lock);
        sem_post(&empty_slots);               /* a space is free now */

        usleep(300000 + rand() % 500000);     /* time to eat it */
    }
    printf("    Guest %d is full and leaves the counter.\n", id);
    return NULL;
}

int main(void) {
    pthread_t chefs[NUM_CHEFS], guests[NUM_GUESTS];
    int chef_id[NUM_CHEFS], guest_id[NUM_GUESTS];

    sem_init(&empty_slots, 0, PLATE_SIZE);    /* plate starts empty */
    sem_init(&full_slots, 0, 0);

    printf("Roti counter open! Plate holds %d rotis.\n\n", PLATE_SIZE);

    for (int i = 0; i < NUM_CHEFS; i++) {
        chef_id[i] = i + 1;
        pthread_create(&chefs[i], NULL, chef, &chef_id[i]);
    }
    for (int i = 0; i < NUM_GUESTS; i++) {
        guest_id[i] = i + 1;
        pthread_create(&guests[i], NULL, guest, &guest_id[i]);
    }

    for (int i = 0; i < NUM_CHEFS; i++)  pthread_join(chefs[i], NULL);
    for (int i = 0; i < NUM_GUESTS; i++) pthread_join(guests[i], NULL);

    printf("\nAll rotis served. Counter closed.\n");

    sem_destroy(&empty_slots);
    sem_destroy(&full_slots);
    return 0;
}
