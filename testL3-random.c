#include <stdio.h>
#include <stdint.h>
#include<stdlib.h>
#include<time.h>

#define MULT 256
#define INTS_IN_L3 1572864
#define NTIMES 20000

void intercept();

int main()
{

/*
L3 is 6MB, which can fit INTS_IN_L3 uint32_ts

Declaring an array that is 8 times bigger than L3.

*/

uint32_t *arrMuchBiggerThanL3=NULL;
long long max = (MULT * INTS_IN_L3);
arrMuchBiggerThanL3 = (uint32_t*)malloc((max) * sizeof(uint32_t));

uint32_t readValue, p;
//Step by 64k -- 64k = 2048 ints
uint32_t delta = 8193;
long long i, ntimes, k=0;
int randIdx;
struct timeval start, end;

//Initialize
for(i=0; i<max; i++)
	arrMuchBiggerThanL3[i] = ((++k) % (65535));


printf("\nMemory used: %1f MB", ((double) (max * sizeof(uint32_t)))/(1024.0 * 1024.0));

//Loop through

srand(time(NULL));
gettimeofday(&start, NULL);


for(ntimes=0; ntimes<NTIMES; ntimes++){

	for(i=0; i<max; i+=delta){
		//Access a[max - i], followed by a[i]
		 randIdx = ((unsigned long)rand()) % max;
		intercept();
		readValue = arrMuchBiggerThanL3[randIdx];
		intercept();
	//	p = arrMuchBiggerThanL3[i];
		p = readValue + 10;
		
	}
}

gettimeofday(&end, NULL);

double t2 =  (end.tv_sec + (end.tv_usec/1000000.0)) - (start.tv_sec + (start.tv_usec/1000000.0));
printf("\nTime taken: %lf secs\n", t2);
return 0;

}

void intercept(){
}
