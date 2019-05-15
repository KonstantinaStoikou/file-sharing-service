#
# Makefile 
#
# Type  make    to compile all the programs
# in the chapter 
#

all: 	p14-create_a_thread \
	p16-detached_thread	\
	p18-random_sleeps \
	p21-print_single \
	p22-multi_hello \
	p24-incprint \
	p26-twordcount1 \
	p34-twordcount2 \
	p37-twordcount3 \
	p41-sync_by_mutex \
	p45-sum_of_squares \
	p56-mutex_condvar \
	p60-counter \
	p65-prod-cons

clean: 
	rm -f p14-create_a_thread \
	p16-detached_thread \
	p18-random_sleeps \
	p21-print_single \
	p22-multi_hello \
	p24-incprint \
	p26-twordcount1 \
	p34-twordcount2 \
	p37-twordcount3 \
	p41-sync_by_mutex \
	p45-sum_of_squares \
	p56-mutex_condvar \
	p60-counter \
	p65-prod-cons

p14-create_a_thread: p14-create_a_thread.c 
	gcc p14-create_a_thread.c -o p14-create_a_thread -lpthread

p16-detached_thread: p16-detached_thread.c 
	gcc p16-detached_thread.c -o p16-detached_thread -lpthread

p18-random_sleeps: p18-random_sleeps.c
	gcc p18-random_sleeps.c -o p18-random_sleeps -lpthread

p21-print_single: p21-print_single.c
	gcc p21-print_single.c -o p21-print_single -lpthread

p22-multi_hello: p22-multi_hello.c
	gcc p22-multi_hello.c  -o  p22-multi_hello -lpthread

p24-incprint: p24-incprint.c
	gcc p24-incprint.c -o p24-incprint -lpthread

p26-twordcount1: p26-twordcount1.c
	gcc p26-twordcount1.c -o p26-twordcount1 -lpthread

p34-twordcount2: p34-twordcount2.c
	gcc p34-twordcount2.c -o p34-twordcount2 -lpthread

p37-twordcount3: p37-twordcount3.c
	gcc p37-twordcount3.c -o p37-twordcount3 -lpthread

p41-sync_by_mutex: p41-sync_by_mutex.c
	gcc p41-sync_by_mutex.c -o p41-sync_by_mutex -lpthread

p45-sum_of_squares: p45-sum_of_squares.c
	gcc p45-sum_of_squares.c -o p45-sum_of_squares -lpthread

p56-mutex_condvar: p56-mutex_condvar.c
	gcc p56-mutex_condvar.c -o p56-mutex_condvar -lpthread

p60-counter: p60-counter.c
	gcc p60-counter.c -o p60-counter -lpthread

p65-prod-cons: p65-prod-cons.c
	gcc p65-prod-cons.c -o p65-prod-cons -lpthread
