#include <stdio.h> 
#include <string.h> 
#include <stdlib.h> 
#include <pthread.h> 
#define perror2(s, e) fprintf(stderr, "%s: %s\n", s, strerror(e))
#define LIMITUP 100

pthread_mutex_t mtx;            /* Mutex for synchronization */
int n, nthr, mtxfl;             /* Variables visible by thread function */
double sqsum;                   /* Sum of squares */
void *square_f(void *);         /* Forward declaration */

int main(int argc, char *argv[]){ 
    int err;
    long i;
    pthread_t *tids;
    if (argc > 3) {
        n = atoi(argv[1]);        /* Last integer to be squared */
	nthr = atoi(argv[2]);     /* Number of threads */
	mtxfl = atoi(argv[3]); }  /* with lock (1)? or without lock (0) */
    else exit(0);

    if (nthr > LIMITUP) { /* Avoid too many threads */
        printf("Number of threads should be up to 100\n"); exit(0); }
    if ((tids = malloc(nthr * sizeof(pthread_t))) == NULL) {
        perror("malloc"); exit(1); }

    sqsum = (double) 0.0; /* Initialize sum */
    pthread_mutex_init(&mtx, NULL); /* Initialize mutex */

    for (i=0 ; i<nthr ; i++) {
       if (err = pthread_create(tids+i, NULL, square_f, (void *) i)) {
       /* Create a thread */
       perror2("pthread_create", err); exit(1); } }

    for (i=0 ; i<nthr ; i++)
       if (err = pthread_join(*(tids+i), NULL)) {
       /* Wait for thread termination */
           perror2("pthread_join", err); exit(1); }

    if (err = pthread_mutex_destroy(&mtx)) { /* Destroy mutex */
           perror2("pthread_mutex_destroy", err); exit(1); }

    if (!mtxfl) printf("Without mutex\n");
    else printf("With mutex\n");

    printf("%2d threads: sum of squares up to %d is %12.9e\n",nthr,n,sqsum);

    sqsum = (double) 0.0; /* Compute sum with a single thread */
    for (i=0 ; i<n ; i++)
        sqsum += (double) (i+1) * (double) (i+1);
    printf("Single thread: sum of squares up to %d is %12.9e\n", n, sqsum);

    printf("Formula based: sum of squares up to %d is %12.9e\n",
             n, ((double) n)*(((double) n)+1)*(2*((double) n)+1)/6);
    pthread_exit(NULL); 
}

void *square_f(void *argp){ /* Thread function */
    int err;
    long i, thri;
    thri = (long) argp;

    for (i=thri ; i<n ; i+=nthr) {
        if (mtxfl) /* Is mutex used? */
            if (err = pthread_mutex_lock(&mtx)) { /* Lock mutex */
                perror2("pthread_mutex_lock", err); exit(1); }

        sqsum += (double) (i+1) * (double) (i+1);

        if (mtxfl) /* Is mutex used? */
            if (err = pthread_mutex_unlock(&mtx)) { /* Unlock mutex */
                perror2("pthread_mutex_unlock", err); exit(1); } 
        }
    pthread_exit(NULL); 
}





