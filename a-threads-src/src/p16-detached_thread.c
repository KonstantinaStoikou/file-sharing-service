#include <stdio.h> 
#include <string.h> 
#include <stdlib.h>       
#include <pthread.h>
#include <unistd.h>

#define perror2(s,e) fprintf(stderr,"%s: %s\n",s,strerror(e))

void *thread_f(void *argp){ /* Thread function */
   int err;
   if (err = pthread_detach(pthread_self())) {/* Detach thread */
       perror2("pthread_detach", err);
       exit(1); 
       }
   printf("I am thread %ld and I was called with argument %d \n", 
		pthread_self(), *(int *)argp);
   printf("I will wait for some time in detached now.. 10 secs\n");
   sleep(12);
   printf("About to leave detached thread now.. %ld \n",pthread_self());
   pthread_exit(NULL); 
}

int main(){ 
   pthread_t thr;
   int err, arg = 29;

   if (err = pthread_create(&thr,NULL,thread_f,(void *) &arg)){/* New thread */
       perror2("pthread_create", err);
       exit(1); 
       }
    sleep(2);
    printf("I am original thread %ld and I created thread %ld\n",
             pthread_self(), thr);
    printf("I am the original and I am done! \n");
    pthread_exit(NULL); 
}
