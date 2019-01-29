#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <stdbool.h>
#include <math.h>

double read_timer() {
	static bool initialized = false;
	static struct timeval start;
	struct timeval end;
	if( !initialized ){
		gettimeofday( &start, NULL );
		initialized = true;
	}
	gettimeofday( &end, NULL );
	return (end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec);
}
double start_time, end_time;

void main(void){
	start_time = read_timer();
	int sum = 0;
	for(int i = 0; i < (10000 * 10000);i++){
		sum = sum + rand() % 99;
	}
	end_time = read_timer();
	printf("The total is: %d\n", sum);
	printf("total execution time is: %g sec\n", end_time - start_time);
}
