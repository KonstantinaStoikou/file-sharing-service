#include <stdio.h> 
#include <string.h> 
#include <stdlib.h> 
#include <pthread.h> 
#define perror2(s, e) fprintf(stderr, "%s: %s\n", s, strerror(e))

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t cvar;           /* Condition variable */
char buf[25];                  /* Message to communicate */
void *thread_f(void *);        /* Forward declaration */

int main(){ 
    pthread_t thr; int err;
    pthread_cond_init(&cvar, NULL); /* Initialize condition variable */

    if (err = pthread_mutex_lock(&mtx)) { /* Lock mutex */
        perror2("pthread_mutex_lock", err); exit(1); }
    printf("Thread %ld: Locked the mutex\n", pthread_self());

    if (err = pthread_create(&thr, NULL, thread_f, NULL)) { /* New thread */
        perror2("pthread_create", err); exit(1); }
    printf("Thread %ld: Created thread %ld\n", pthread_self(), thr);

    printf("Thread %ld: Waiting for signal\n", pthread_self());

    pthread_cond_wait(&cvar, &mtx); /* Wait for signal */
    printf("Thread %ld: Woke up\n", pthread_self());
    printf("Thread %ld: Read message \"%s\"\n", pthread_self(), buf);

    if (err = pthread_mutex_unlock(&mtx)) { /* Unlock mutex */
        perror2("pthread_mutex_unlock", err); exit(1); }
    printf("Thread %ld: Unlocked the mutex\n", pthread_self());

    if (err = pthread_join(thr, NULL)) { /* Wait for thread */
        perror2("pthread_join", err); exit(1); } /* termination */
    printf("Thread %ld: Thread %ld exited\n", pthread_self(), thr);

    if (err = pthread_cond_destroy(&cvar)) {
        /* Destroy condition variable */
        perror2("pthread_cond_destroy", err); exit(1); }
    pthread_exit(NULL); 
}

void *thread_f(void *argp){ /* Thread function */
    int err;

    printf("Thread %ld: Just started \n", pthread_self());
    printf("Thread %ld: Trying to lock the mutex\n", pthread_self());

    if (err = pthread_mutex_lock(&mtx)) { /* Lock mutex */
        perror2("pthread_mutex_lock", err); exit(1); }
    printf("Thread %ld: Locked the mutex\n", pthread_self());

    strcpy(buf, "This is a test message");

    printf("Thread %ld: Wrote message \"%s\"\n", pthread_self(), buf);
    pthread_cond_signal(&cvar); /* Awake other thread */
    printf("Thread %ld: Sent signal\n", pthread_self());

    if (err = pthread_mutex_unlock(&mtx)) { /* Unlock mutex */
        perror2("pthread_mutex_unlock", err); exit(1); }
    printf("Thread %ld: Unlocked the mutex\n", pthread_self());

    pthread_exit(NULL); 
}

