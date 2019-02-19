#ifndef _REENTRANT
#define _REENTRANT
#endif
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#define SHARED 1
#define ITERS 100
#define CONSUMERS 8

void *Producer();
void *Consumer();

sem_t empty, full;
int data;
int numIters;

int main(void){
	pthread_t pid, cid;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

	numIters = ITERS;
	sem_init(&empty, SHARED, 1); /* sem empty = 1 */
	sem_init(&full, SHARED, 0);  /* sem full = 0; */

	printf("main started\n");
	pthread_create(&pid, &attr, Producer, NULL);
	pthread_create(&cid, &attr, Consumer, NULL);
	pthread_join(pid, NULL);
	pthread_join(cid, NULL);
	printf("main done\n");

}

void *Producer(){
	int produced;
	printf("Momma bird created\n");
	while(true){
		sem_wait(&empty);
		data = produced;
		sem_post(&full);
	}
}

void *Consumer(){
	int total = 0, consumed;
	printf("Consumer created\n");
	for(consumed = 0; consumed < numIters; consumed++){
	sem_wait(&full);
	total = total + data;
	sem_post(&empty);
	}
	printf("for %d iterations, the total is %d\n", numIters, total);
}
