#ifndef _REENTRANT
#define _REENTRANT
#endif

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#define SHARED 1
#define BEES 10
#define MAXHONEY 10

void *Producer();
void Consumer();

sem_t empty, full, mutex;
int honey = 0;
int count = 0;

void main(void){
	pthread_t cid;
	pthread_t pid[BEES];
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

	sem_init(&empty, SHARED, 0);
	sem_init(&full, SHARED, 0);
	sem_init(&mutex, SHARED, 1);

	printf("main started\n");

	for(long i = 0; i < BEES; i++) pthread_create(&pid[i], &attr, Producer, (void *) i);
	Consumer(); //bear
	printf("Should not be written.");
}

void *Producer(int pid){
	for(;;){
		sem_wait(&mutex);
		honey++;
		printf("Bee %d adding honey to pot. Pot: %d\n", pid, honey);
		if(honey >= MAXHONEY){
			printf("Pot is full, bee %d is waking up Mr.Bears\n", pid);
			sem_post(&full);
			sem_wait(&empty);
		}
		sem_post(&mutex);
		//sleep(rand()% 10);
	}
}

void Consumer(){
	for(;;){
		sem_wait(&full);
		printf("************************************************************\n");
		printf("Mr.Bears: OMNOMNOMNOMNOM THIS IS SOME GOOD SH*T\n");
		printf("Mr.Bears: GOING BACK TO SLEEP NOW... ALSO RELEASE THE HOUNDS\n");
		printf("Mr.Bears: I HAVE NOW EATEN %d POTS OF HONEY\n", ++count);
		printf("************************************************************\n");
		honey = 0;
		sem_post(&empty);
	}
}
