#include <stdio.h> 		// from www.mario-konrad.ch
#include <pthread.h>
#include <unistd.h>

#define POOL_SIZE 6

typedef struct {
	int data[POOL_SIZE];
	int start;
	int end;
	int count;
} pool_t;

int num_of_items = 15;

pthread_mutex_t mtx;
pthread_cond_t cond_nonempty;
pthread_cond_t cond_nonfull;
pool_t pool;

void initialize(pool_t * pool) {
	pool->start = 0;
	pool->end = -1;
	pool->count = 0;
}

void place(pool_t * pool, int data) {
	pthread_mutex_lock(&mtx);
	while (pool->count >= POOL_SIZE) {
		printf(">> Found Buffer Full \n");
		pthread_cond_wait(&cond_nonfull, &mtx);
		}
	pool->end = (pool->end + 1) % POOL_SIZE;
	pool->data[pool->end] = data;
	pool->count++;
	pthread_mutex_unlock(&mtx);
}

int obtain(pool_t * pool) {
	int data = 0;
	pthread_mutex_lock(&mtx);
	while (pool->count <= 0) {
		printf(">> Found Buffer Empty \n");
		pthread_cond_wait(&cond_nonempty, &mtx);
		}
	data = pool->data[pool->start];
	pool->start = (pool->start + 1) % POOL_SIZE;
	pool->count--;
	pthread_mutex_unlock(&mtx);
	return data;
}

void * producer(void * ptr)
{
	while (num_of_items > 0) {
		place(&pool, num_of_items);
		printf("producer: %d\n", num_of_items);
		num_of_items--;
		pthread_cond_signal(&cond_nonempty);
		usleep(1000);
		}
	pthread_exit(0);
}

void * consumer(void * ptr)
{
	while (num_of_items > 0 || pool.count > 0) {
		printf("consumer: %d\n", obtain(&pool));
		pthread_cond_signal(&cond_nonfull);
		usleep(500000);
		}
	pthread_exit(0);
}

int main(int argc, char ** argv)
{
	pthread_t cons, prod;
	
	initialize(&pool);
	pthread_mutex_init(&mtx, 0);
	pthread_cond_init(&cond_nonempty, 0);
	pthread_cond_init(&cond_nonfull, 0);
	pthread_create(&cons, 0, consumer, 0);
	pthread_create(&prod, 0, producer, 0);
	pthread_join(prod, 0);
	pthread_join(cons, 0);
	pthread_cond_destroy(&cond_nonempty);
	pthread_cond_destroy(&cond_nonfull);
	pthread_mutex_destroy(&mtx);
	return 0;
}
