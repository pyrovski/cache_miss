#include <stdio.h>
#include <stdint.h>
#include<stdlib.h>
#include<time.h>

int main()
{

uint32_t *arrFitsInL3=NULL;
arrFitsInL3 = (uint32_t*)malloc(1572862 * sizeof(uint32_t));


uint32_t *arrExactlyL3=NULL;
arrExactlyL3 = (uint32_t*)malloc(1572864 * sizeof(uint32_t));


uint32_t *arrLittleBiggerThanL3=NULL;
arrLittleBiggerThanL3 = (uint32_t*)malloc(1572900 * sizeof(uint32_t));

uint32_t readValue;
int k =0;
int p;

long long i, backIterator;
//Initialize

struct timeval start, end;


for(i=0; i<1572862; i++)
        arrFitsInL3[i] = k++;

//Jump backward by 4k +5
//

gettimeofday(&start, NULL);
for(backIterator=0; backIterator<=1572861; backIterator++)
{
        readValue = arrFitsInL3[backIterator];
        p = readValue;
	p = p - 10;
//      printf("\nreadValue: %ld", readValue); 
}

gettimeofday(&end, NULL);

 double t5 =  (end.tv_sec + (end.tv_usec/1000000.0)) - (start.tv_sec + (start.tv_usec/1000000.0));
 printf("\nTime taken in secs (fits in L3, sequential access): %lf", t5);


gettimeofday(&start, NULL);
for(backIterator=1572861; backIterator>=0; backIterator--)
{
        readValue = arrFitsInL3[backIterator];
//        readValue = backIterator % 1000;
        p = readValue;
	p = p - 10;
//      printf("\nreadValue: %ld", readValue); 
}

gettimeofday(&end, NULL);

 double t6 =  (end.tv_sec + (end.tv_usec/1000000.0)) - (start.tv_sec + (start.tv_usec/1000000.0));
 printf("\nTime taken in secs (fits in L3, backward access): %lf", t6);



k=0;
for(i=0; i<1572864; i++)
        arrExactlyL3[i] = k++;

//Jump backward by 4k +5
//

gettimeofday(&start, NULL);
for(backIterator=0; backIterator<=1572863; backIterator++)
{
        readValue = arrExactlyL3[backIterator];
        //readValue = backIterator + 1000;
        p = readValue;
	p = p - 10;
//      printf("\nreadValue: %ld", readValue); 
}

gettimeofday(&end, NULL);

 double t3 =  (end.tv_sec + (end.tv_usec/1000000.0)) - (start.tv_sec + (start.tv_usec/1000000.0));
 printf("\nTime taken in secs (exactly L3, sequential access): %lf", t3);


gettimeofday(&start, NULL);
for(backIterator=1572863; backIterator>=0; backIterator--)
{
        readValue = arrExactlyL3[backIterator];
       // readValue = backIterator % 1000;
        p = readValue;
	p = p - 10;
//	printf("\nreadValue: %ld", readValue); 
}

gettimeofday(&end, NULL);

 double t1 =  (end.tv_sec + (end.tv_usec/1000000.0)) - (start.tv_sec + (start.tv_usec/1000000.0));
 printf("\nTime taken in secs (exactly L3, backward access): %lf", t1);

k=0;
for(i=0; i<1572900; i++)
	arrLittleBiggerThanL3[i] = k++;

//Jump backward by 4k +5
//


gettimeofday(&start, NULL);
for(backIterator=32; backIterator<=1572899; backIterator++)
{
        readValue = arrLittleBiggerThanL3[backIterator];
//        readValue = backIterator + 1000;
        p = readValue;
	p = p - 10;
//      printf("\nreadValue: %ld", readValue); 
}

 gettimeofday(&end, NULL);

 double t4 =  (end.tv_sec + (end.tv_usec/1000000.0)) - (start.tv_sec + (start.tv_usec/1000000.0));
 printf("\nTime taken in secs (bigger than L3, sequential access): %lf", t4);


gettimeofday(&start, NULL);
for(backIterator=1572899; backIterator>=32; backIterator--)
{
	readValue = arrLittleBiggerThanL3[backIterator];
	//readValue = backIterator + 1000;
        p = readValue;
	p = p - 10;
//	printf("\nreadValue: %ld", readValue); 
}

 gettimeofday(&end, NULL);

 double t2 =  (end.tv_sec + (end.tv_usec/1000000.0)) - (start.tv_sec + (start.tv_usec/1000000.0));
 printf("\nTime taken in secs (bigger than L3, backward access): %lf\n", t2);

printf("Size of uint32_t is: %d\n", sizeof(uint32_t));
printf("Size of long long is: %d\n", sizeof(long long));


return 0;

}
