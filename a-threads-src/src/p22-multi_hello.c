#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define NUM 5

void main()
{	pthread_t t1, t2;

	void *print_mesg(void *);
	
	pthread_create(&t1, NULL, print_mesg, (void *)"hello ");
	pthread_create(&t2, NULL, print_mesg, (void *)"world\n");
	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
}

void *print_mesg(void *m)
{	char *cp = (char *)m;
	int i;

	for (i=0;i<NUM; i++){
		printf("%s", cp);
		fflush(stdout);
		sleep(2);
		}
	return NULL;
}
