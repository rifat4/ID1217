/* matrix summation using pthreads

   features: uses a barrier; the Worker[0] computes
             the total sum from partial sums computed by Workers
             and prints the total sum to the standard output

   usage under Linux:
     gcc matrixSum.c -lpthread
     a.out size numWorkers

*/
#ifndef _REENTRANT 
#define _REENTRANT 
#endif 
#include <limits.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#define MAXSIZE 10000  /* maximum matrix size */
#define MAXWORKERS 10   /* maximum number of workers */

pthread_mutex_t barrier;  /* mutex lock for the barrier */
pthread_cond_t go;        /* condition variable for leaving */
int numWorkers;           /* number of workers */ 
int numArrived = 0;       /* number who have arrived */
int counter = 0;            /* used for bag of tasks */
int realTotal = 0;
int realMin = INT_MAX;
int realMax = INT_MIN;
int realMinY, realMinX, realMaxY, realMaxX;
pthread_mutex_t maxLock;
pthread_mutex_t minLock;
pthread_mutex_t counterLock;  //TODO
pthread_mutex_t sumLock;

/* a reusable counter barrier */
void Barrier() {
  pthread_mutex_lock(&barrier);
  numArrived++;
  if (numArrived == numWorkers) {
    numArrived = 0;
    pthread_cond_broadcast(&go);
  } else
    pthread_cond_wait(&go, &barrier);
  pthread_mutex_unlock(&barrier);
}

/* timer */
double read_timer() {
    static bool initialized = false;
    static struct timeval start;
    struct timeval end;
    if( !initialized )
    {
        gettimeofday( &start, NULL );
        initialized = true;
    }
    gettimeofday( &end, NULL );
    return (end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec);
}

double start_time, end_time; /* start and end times */
int size, stripSize;  /* assume size is multiple of numWorkers */
int sums[MAXWORKERS]; /* partial sums */
int matrix[MAXSIZE][MAXSIZE]; /* matrix */
int maxValues[MAXWORKERS]; //array for maxvalues to be removed
int minValues[MAXWORKERS]; //array for minvalues to be removed


void *Worker(void *);

/* read command line, initialize, and create threads */
int main(int argc, char *argv[]) {
  int i, j;
  long l; /* use long in case of a 64-bit system */
  pthread_attr_t attr;
  pthread_t workerid[MAXWORKERS];

  /* set global thread attributes */
  pthread_attr_init(&attr);
  pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

  /* initialize mutex and condition variable */
  pthread_mutex_init(&barrier, NULL);
  pthread_cond_init(&go, NULL);

  /* initialize minmax, counter and sumLock variable mutex*/
  if(pthread_mutex_init(&maxLock, NULL) != 0){
	printf("\nfailed to initialize minmax mutex\n");
  }
  if(pthread_mutex_init(&minLock, NULL) != 0){
	printf("\nfailed to initialize minmax mutex\n");
  }
  if(pthread_mutex_init(&counterLock, NULL) != 0){
	printf("\nfailed to initialize counter mutex\n");
  }
  if(pthread_mutex_init(&sumLock, NULL) != 0){
	printf("\nfailed to initialize sumLock mutex\n");
  }

  /* read command line args if any */
  size = (argc > 1)? atoi(argv[1]) : MAXSIZE;
  numWorkers = (argc > 2)? atoi(argv[2]) : MAXWORKERS;
  if (size > MAXSIZE) size = MAXSIZE;
  if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;
  stripSize = size/numWorkers;

  /* initialize the matrix */
  for (i = 0; i < size; i++) {
	  for (j = 0; j < size; j++) {
         //matrix[i][j] = 1;
         matrix [i][j] = rand() % 99;//%99;
	  }
  }

  /* print the matrix */
#ifdef DEBUG
  for (i = 0; i < size; i++) {
	  printf("[ ");
	  for (j = 0; j < size; j++) {
	    printf(" %d", matrix[i][j]);
	  }
	  printf(" ]\n");
  }
#endif

  	/* do the parallel work: create the workers */
  	start_time = read_timer();
  	for (l = 0; l < numWorkers; l++){
    	pthread_create(&workerid[l], &attr, Worker, (void *) l);
	}
	for(int i = 0; i < numWorkers; i++){
		pthread_join(workerid[i], NULL);
	}
  	//printf("move that dump %ld\n", myid);
  	// have to implement some kind of wait for thread 0.
  	// has to wait for all other threads to finisht their work.
  	// option is to let last thread to the remaining work
  	// unless we assume that thread 0 is the main thread.
    //max = maxValues[0];
    //min = minValues[0];
    //for (i = 0; i < numWorkers; i++){
      //	total += sums[i];
      //	if(max < maxValues[i])max = maxValues[i];
      //	if(min > minValues[i])min = minValues[i];
    //  }
    /* get end time */
    end_time = read_timer();
    /* print results */
    printf("The minValue = %d\n", realMin);
    printf("Found at position [%d, %d]\n", realMinX, realMinY);
    printf("The maxValue = %d\n", realMax);
    printf("Found at position [%d, %d]\n", realMaxX, realMaxY);
    printf("The total is %d\n", realTotal);
    printf("The execution time is %g sec\n", end_time - start_time);

}

/* Each worker sums the values in one strip of the matrix.
   After a barrier, worker(0) computes and prints the total */
void *Worker(void *arg) {
  long myid = (long) arg;
  int i, j, first, last;

#ifdef DEBUG
  printf("worker %d (pthread id %d) has started\n", myid, pthread_self());
#endif

  /* determine first and last rows of my strip */
  first = myid*stripSize;
  last = (myid == numWorkers - 1) ? (size - 1) : (first + stripSize - 1);
  /* sum values in my strip */
  int max = INT_MIN;
  int min = INT_MAX;
  int minX, minY;
  int maxX, maxY;
	while(true){
		int localCounter;
		int total = 0;
		pthread_mutex_lock(&counterLock);
		localCounter = counter++;
		pthread_mutex_unlock(&counterLock);
		if(counter > size)return 0;

	    for (j = 0; j < size; j++){
	    	total += matrix[localCounter][j];
	    	if(max < matrix[localCounter][j]){
	    		max = matrix[localCounter][j];
				maxY = j;
				maxX = localCounter;
			}
	  		if(min > matrix[localCounter][j]){
	  			min = matrix[localCounter][j];
				minY = j;
				minX = localCounter;
			}
      	}

		/* This should possibly be guarded by one lock, because of reasons*/
		pthread_mutex_lock(&sumLock);
		realTotal = realTotal + total;
		pthread_mutex_unlock(&sumLock);

		if(max > realMax){
			pthread_mutex_lock(&maxLock);
			if(max > realMax){
				//should really change this to a struct
				printf("Writing over oldMax: %d, With NewMax: %d\n", realMax, max);
				printf("Old pos: [%d, %d], new pos: [%d, %d]\n", realMaxX, realMaxY, maxX, maxY);
				realMax = max;
				realMaxX = maxX;
				realMaxY = maxY;
			}
			pthread_mutex_unlock(&maxLock);
		}
		if(min < realMin){
			pthread_mutex_lock(&minLock);
			if(min < realMin){
				//should really change this to a struct
				realMin = min;
				realMinX = minX;
				realMinY = minY;
			}
			pthread_mutex_unlock(&minLock);
		}
		/* This should possibly be guarded by one lock, because of reasons*/
		//to reduce chance of deadlocks and overhead
		//not really possible to get deadlock in this specific situation
		//overhead is minimal because of how the access rate of min and max lock 
		//is reduced semi-logarithmically the more we use it. (worst case is linear)
		//shouldn't be a issue in this program, but something to consider in the future
		//adding the code for pos of min/max added about 70% more time
		//should think about refactoring that..
	}
}
