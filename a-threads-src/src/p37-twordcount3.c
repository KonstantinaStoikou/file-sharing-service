#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <ctype.h>
#define EXIT_FAILURE 1

struct arg_set{
	char *fname;
	int  count;
	};

int	main(int ac, char *av[]) {	
        pthread_t t1, t2;
	struct arg_set args1, args2;
	void *count_words(void *);

	if ( ac != 3 ) {
           printf("usage: %s file1 file2 \n", av[0]); exit (EXIT_FAILURE); }	

	args1.fname = av[1]; args1.count = 0;
	pthread_create(&t1, NULL, count_words, (void *) &args1);

	args2.fname = av[2]; args2.count = 0;
	pthread_create(&t2, NULL, count_words, (void *) &args2);

	pthread_join(t1, NULL); pthread_join(t2, NULL);

	printf("In file  %-10s there are %5d words\n", av[1], args1.count);
	printf("In file  %-10s there are %5d words\n", av[2], args2.count);
	printf("Main thread %ld reporting %5d total words\n", 
                pthread_self(), args1.count+args2.count);
}

void *count_words(void *a) {
     struct arg_set *args = a;
     FILE *fp; int c, prevc = '\0';
     printf("Working within Thread with ID %ld and counting\n",pthread_self());

     if ( (fp=fopen(args->fname,"r")) != NULL ){
		while ( ( c = getc(fp) )!= EOF ){
			if ( !isalnum(c) && isalnum(prevc) ){
				args->count++;
				}
			prevc = c;
			}
		fclose(fp);
      } else perror(args->fname);
      return NULL;
}
