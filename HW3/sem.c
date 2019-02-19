#ifndef _REENTRANT
#define _REENTRANT
#endif
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#define SHARED 1
#define ITERS 100
#define CONSUMERS 10

void Producer();
void *Consumer();

sem_t empty, full, mutex;
int data;
int numIters;

int main(void){
	pthread_t pid;
	pthread_t cid[CONSUMERS];
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

	numIters = ITERS;
	sem_init(&empty, SHARED, 1); /* sem empty = 1 */
	sem_init(&full, SHARED, 0);  /* sem full = 0; */
	sem_init(&mutex, SHARED, 1); /* sem empty = 1 */


	printf("main started\n");
	//pthread_create(&pid, &attr, Producer, NULL);
	for(long i = 0; i < CONSUMERS; i++) pthread_create(&cid[i], &attr, Consumer,(void *) i);
	//pthread_join(pid, NULL);
	//pthread_join(cid, NULL);
	Producer();
	printf("main done\n");

}

void Producer(){
	int produced;
	printf("Momma bird created\n");
	for(;;){
		sem_wait(&empty);
		printf("Bowl empty, momma getting more worms.\n");
		data = (rand() % 10) + 1;
		printf("Added %d worms to the bowl.\n", data);
		sem_post(&full);
		/*
		for(int i = 0; i < data; i++){
			sem_post(&full);
		}
		*/
		//sem_post(&mutex);
	}
}

void *Consumer(int pid){
	int total = 0, consumed, timer;
	printf("Baby birted\n");
	for(;;){
		sem_wait(&mutex); //mutex because of hazard, can get negative (data) otherwise;
		sem_wait(&full);
		if(data <= 0){
			printf("%d: FEED ME\n", pid);
			sem_post(&empty);
			sem_wait(&full);
		}
		data--;
		printf("Bird %d got a worm. Worms left: %d\n", pid, data);
		sem_post(&full);
		sem_post(&mutex);
		timer = rand() % 10;
		sleep(timer);
	}
	//printf("for %d iterations, the total is %d\n", numIters, total);
}
