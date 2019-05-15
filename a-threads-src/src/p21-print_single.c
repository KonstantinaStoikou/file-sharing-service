#include <stdio.h>
#include <unistd.h>

#define	NUM 5

void print_mesg(char *);

int 	main(){
	print_mesg("hello");
	print_mesg("world\n");
}

void print_mesg(char *m){
int i;
for (i=0; i<NUM; i++){
	printf("%s", m);
	fflush(stdout);
	sleep(1);
	}
}


