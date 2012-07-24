#include <stdio.h>
#include <stdint.h>
#include<stdlib.h>
#include<time.h>

void backward(uint32_t *arrMuchBiggerThanL3);
void intercept(); //does nothing
void inner(uint32_t *arrMuchBiggerThanL3, int otherIterator);

int main()
{

/*
L3 is 6MB, which can fit 199608 uint32_ts

Declaring an array that is 64 times bigger than L3.

*/

uint32_t *arrMuchBiggerThanL3=NULL;
arrMuchBiggerThanL3 = (uint32_t*)malloc((64 * 196608) * sizeof(uint32_t));

//struct timeval start, end;

//'long long' is 8 bytes, cant use uint64_t as this needs to be signed for the loops to work
//printf("Size of long long is %d", sizeof(long long));


long long i, k=0;
//Initialize



for(i=0; i<(196608*64); i++)
	arrMuchBiggerThanL3[i] = ((++k) % (65535));

//
//Jump by 4k bytes = 128 ints
//
/*
gettimeofday(&start, NULL);

for(otherIterator=0; otherIterator<5000; otherIterator++){

	for(backIterator=otherIterator; backIterator<=1572864*8; backIterator+=128)
	{
        	readValue = arrMuchBiggerThanL3[backIterator];
        	p = readValue;
		p = p - 10;
	}

}
 gettimeofday(&end, NULL);

 double t4 =  (end.tv_sec + (end.tv_usec/1000000.0)) - (start.tv_sec + (start.tv_usec/1000000.0));
 printf("\nTime taken in secs (bigger than L3, sequential access): %lf", t4);
*/


//Call the function

//backward(arrMuchBiggerThanL3);





uint32_t readValue;
int p;
int otherIterator;
struct timeval start, end;
//long long backIterator, countAll;

//countAll=0;

gettimeofday(&start, NULL);
for(otherIterator=0; otherIterator<5000; otherIterator++){
	/*for(backIterator=((196608*64) - otherIterator); backIterator>=0; backIterator-=128)
	{
		countAll++;
	//	intercept();
		readValue = arrMuchBiggerThanL3[backIterator];
	//	intercept();
	        p = readValue;
		p = p + 10;
		//printf("\n %d", p);
	}*/

	inner(arrMuchBiggerThanL3, otherIterator);
}
 gettimeofday(&end, NULL);

 double t2 =  (end.tv_sec + (end.tv_usec/1000000.0)) - (start.tv_sec + (start.tv_usec/1000000.0));
 printf("\nTime taken in secs (bigger than L3, backward access): %lf\n", t2);
// printf("\nTotal count is: %llu \n", countAll);



//Free allocated memory
free(arrMuchBiggerThanL3);

return 0;

}


void backward(uint32_t *arrMuchBiggerThanL3){

uint32_t readValue;
int p;
int otherIterator;
struct timeval start, end;
//long long backIterator, countAll;

//countAll=0;

gettimeofday(&start, NULL);
for(otherIterator=0; otherIterator<5000; otherIterator++){
	/*for(backIterator=((196608*64) - otherIterator); backIterator>=0; backIterator-=128)
	{
		countAll++;
	//	intercept();
		readValue = arrMuchBiggerThanL3[backIterator];
	//	intercept();
	        p = readValue;
		p = p + 10;
		//printf("\n %d", p);
	}*/

	inner(arrMuchBiggerThanL3, otherIterator);
}
 gettimeofday(&end, NULL);

 double t2 =  (end.tv_sec + (end.tv_usec/1000000.0)) - (start.tv_sec + (start.tv_usec/1000000.0));
 printf("\nTime taken in secs (bigger than L3, backward access): %lf\n", t2);
// printf("\nTotal count is: %llu \n", countAll);
}


void inner(uint32_t *arrMuchBiggerThanL3, int otherIterator){


uint32_t readValue;
int p;
long long backIterator;

for(backIterator=((196608*64) - otherIterator); backIterator>=0; backIterator-=128)
	{
	//	countAll++;
	//	intercept();
		readValue = arrMuchBiggerThanL3[backIterator];
	//	intercept();
	        p = readValue;
		p = p + 10;
		//printf("\n %d", p);
	}
}

void intercept(){
printf("\nHello World");
}
