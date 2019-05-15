#include <stdio.h> 
#include <string.h>    /* For strerror */
#include <stdlib.h>    /* For exit     */
#include <pthread.h>   /* For threads  */
#include <unistd.h>

#define perror2(s,e) fprintf(stderr, "%s: %s\n", s, strerror(e))

void *thread_f(void *argp){ /* Thread function */
   printf("I am the newly created thread %ld\n", pthread_self());
   printf("Hanging on for a couple of secs!\n");
   sleep(2);
   printf("I am the newly created thread and about to exit! \n");
   pthread_exit((void *) 47);
}

int main(){ 
  pthread_t thr;
  int err, status;
  if (err = pthread_create(&thr, NULL, thread_f, NULL)) { /* New thread */
      perror2("pthread_create", err);
      exit(1); 
      }
  printf("I am original thread %ld and I created thread %ld\n", 
           pthread_self(), thr);
  if (err = pthread_join(thr, (void **) &status)) { /* Wait for thread */
      perror2("pthread_join", err); /* termination */
      exit(1); 
      }
  else printf("Just joinned the two threads ->one!\n");
  printf("Thread %ld exited with code %d\n", thr, status);
  printf("Thread %ld just before exiting (Original)\n", pthread_self());
  pthread_exit(NULL); 
}

