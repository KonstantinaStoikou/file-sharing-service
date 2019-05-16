#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_SLEEP 10 /* Maximum sleeping time in seconds */
#define perror2(s, e) fprintf(stderr, "%s: %s\n", s, strerror(e))

void *sleeping(void *arg) {
    long sl = (long)arg;
    printf("thread %ld sleeping %ld seconds ...\n", pthread_self(), sl);
    sleep(sl); /* Sleep a number of seconds */
    printf("thread %ld waking up\n", pthread_self());
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    int n, i, err;
    long sl;
    pthread_t *tids;
    if (argc > 1)
        n = atoi(argv[1]); /* Make integer */
    else
        exit(0);
    if (n > 50) { /* Avoid too many threads */
        printf("Number of threads should be up to 50\n");
        exit(0);
    }

    if ((tids = malloc(n * sizeof(pthread_t))) == NULL) {
        perror("malloc");
        exit(1);
    }

    srandom((unsigned int)time(NULL)); /* Initialize generator */
    for (i = 0; i < n; i++) {
        sl = random() % MAX_SLEEP + 1; /* Sleeping time 1..MAX_SLEEP */
        if (err = pthread_create(tids + i, NULL, sleeping, (void *)sl)) {
            /* Create a thread */
            perror2("pthread_create", err);
            exit(1);
        }
    }

    for (i = 0; i < n; i++)
        if (err = pthread_join(*(tids + i), NULL)) {
            /* Wait for thread termination */
            perror2("pthread_join", err);
            exit(1);
        }
    printf("all %d threads have terminated\n", n);
}
