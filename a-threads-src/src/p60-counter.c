#include <stdio.h> 
#include <stdlib.h> 
#include <pthread.h> 
#include <string.h>

#define perror2(s, e) fprintf(stderr, "%s: %s\n", s, strerror(e))

#define COUNT_PER_THREAD 8       /* Count increments by each thread */
#define THRESHOLD 21             /* Count value to wake up thread */

int count = 0;                    /* The counter */
int thread_ids[4] = {0, 1, 2, 3}; /* My thread ids */

pthread_mutex_t mtx;              /* mutex */
pthread_cond_t cv;                /* the condition variable */

void *incr(void *argp){
    int i, j, *id = argp;
    int err;

    for (i=0 ; i<COUNT_PER_THREAD ; i++) {
        if (err = pthread_mutex_lock(&mtx)) { /* Lock mutex */
            perror2("pthread_mutex_lock", err); exit(1); }
        count++; /* Increment counter */
        if (count == THRESHOLD) { /* Check for threshold */
            pthread_cond_signal(&cv); /* Signal suspended thread */
            printf("incr: thread %d, count = %d, threshold reached\n",*id,count); 
            }
        printf("incr: thread %d, count = %d\n", *id, count);
        if (err = pthread_mutex_unlock(&mtx)) { /* Unlock mutex */
            perror2("pthread_mutex_unlock", err); exit(1); }
     } /* For threads to alternate */
    /* on mutex lock */
    pthread_exit(NULL); 
}

void *susp(void *argp){
    int err, *id = argp;
    printf("susp: thread %d started\n", *id);
    printf("susp: thread %d about to suspend\n", *id);

    if (err = pthread_mutex_lock(&mtx)) { /* Lock mutex */
        perror2("pthread_mutex_lock", err); exit(1); 
        }
    if (count < THRESHOLD) { /* If threshold not reached */
        pthread_cond_wait(&cv, &mtx); /* suspend */
        printf("susp: thread %d, signal received\n", *id); 
        }
    if (err = pthread_mutex_unlock(&mtx)) { /* Unlock mutex */
        perror2("pthread_mutex_unlock", err); exit(1); 
        }
    pthread_exit(NULL); 
}

int main() { 
    int i, err;
    pthread_t threads[4];

    pthread_mutex_init(&mtx, NULL); /* Initialize mutex */
    pthread_cond_init(&cv, NULL); /* and condition variable */

    for (i=0 ; i<3 ; i++)
        if (err=pthread_create(&threads[i],NULL,incr,(void *)&thread_ids[i])) { 
            /* Create threads 0, 1, 2 */
            perror2("pthread_create", err); exit(1); 
            }

    if (err=pthread_create(&threads[3], NULL, susp, (void *) &thread_ids[3])) { 
        /* Create thread 3 */
        perror2("pthread_create", err); exit(1); }

    for (i=0 ; i<4 ; i++)
        if (err = pthread_join(threads[i], NULL)) {
            perror2("pthread_join", err); exit(1); 
            };
    /* Wait for threads termination */

    printf("main: all threads terminated\n");
    /* Destroy mutex and condition variable */
    if (err = pthread_mutex_destroy(&mtx)) {
        perror2("pthread_mutex_destroy", err); exit(1); }
    if (err = pthread_cond_destroy(&cv)) {
        perror2("pthread_cond_destroy", err); exit(1); }
    pthread_exit(NULL); 
}
