#include <stdio.h>
#include <stdint.h>
#include<stdlib.h>
#include<time.h>
#include <papi.h>

#define MULT 20
#define INTS_IN_L3 (1024*1024*20/sizeof(int))

uint32_t *arrMuchBiggerThanL3=NULL;
int *indices = 0;
long long max = (MULT * INTS_IN_L3);

uint32_t readValue, p;
//Step by 64k -- 64k = 2048 ints
uint32_t delta = 8192;

volatile long long i, n;
struct timeval start, end;


void test_error(char * file, int line, char * message, int val){
  fprintf(stderr, "%s: %d; %s: %d\n", 
	  file, line, message, val);
}

void test_fail(char * file, int line, char * message, int val){
  test_error(file, line, message, val);
  exit(1);  
}

void access_pattern(){
  for(n=0; n<delta; n++){
    for(i=n; i<max; i+=delta){
      //Access a[max - i], followed by a[i]
      readValue = arrMuchBiggerThanL3[max-i];
      p = arrMuchBiggerThanL3[i];
      p += readValue;
      arrMuchBiggerThanL3[i] = p;
    }
  }
}

void access_random(){
  int index1, index2;
  for(i=0; i<max; i++){
    //Access a[max - i], followed by a[i]
    index1 = indices[i];
    index2 = indices[max-i];
    readValue = arrMuchBiggerThanL3[index2];
    p = arrMuchBiggerThanL3[index1];
    p += readValue;
    arrMuchBiggerThanL3[index1] = p;
  }
}

void access_linear(){
  for(i=0; i<max; i++){
    //Access a[max - i], followed by a[i]
    readValue = arrMuchBiggerThanL3[max-i];
    p = arrMuchBiggerThanL3[i];
    p += readValue;
    arrMuchBiggerThanL3[i] = p;
  }
}

int main()
{
  
  arrMuchBiggerThanL3 = (uint32_t*)malloc((max) * sizeof(uint32_t));
  indices = (int*)malloc((max) * sizeof(int));

  long long k=0;
  
  printf("RAND_MAX: %ld\n", RAND_MAX);

  //Initialize
  for(i=0; i<max; i++){
    arrMuchBiggerThanL3[i] = ((++k) % (65535));
    indices[i] = ((unsigned long)rand()) % max;
  }

  printf("Memory used: %1f MB\n", ((double) (max * sizeof(uint32_t)))/(1024.0 * 1024.0));
  
  long long value;
  int EventSet = PAPI_NULL;
  int event;
  int retval;
  char descr[PAPI_MAX_STR_LEN];
  PAPI_event_info_t evinfo;
  
  int eventlist[] = {
    PAPI_L1_DCA,
    PAPI_L1_DCM,
    PAPI_L1_DCH,
    PAPI_L2_DCA,
    PAPI_L2_DCM,
    PAPI_L2_DCH,
    PAPI_L3_DCA,
    PAPI_L3_TCA,
    PAPI_L3_TCM,
    PAPI_TOT_INS,
    PAPI_STL_ICY,
    
#if 0
    PAPI_L1_LDM,
    PAPI_L1_STM,
    PAPI_L1_DCR,
    PAPI_L1_DCW,
    PAPI_L1_ICM,
    PAPI_L1_TCM,
    PAPI_LD_INS,
    PAPI_SR_INS,
    PAPI_LST_INS,
    PAPI_L2_DCR,
    PAPI_L2_DCW,
    PAPI_CSR_TOT,
    PAPI_MEM_SCY,
    PAPI_MEM_RCY,
    PAPI_MEM_WCY,
    PAPI_L1_ICH,
    PAPI_L1_ICA,
    PAPI_L1_ICR,
    PAPI_L1_ICW,
    PAPI_L1_TCH,
    PAPI_L1_TCA,
    PAPI_L1_TCR,
    PAPI_L1_TCW,
    PAPI_L2_DCM,
    PAPI_L2_ICM,
    PAPI_L2_TCM,
    PAPI_L2_LDM,
    PAPI_L2_STM,
    PAPI_L2_DCH,
    PAPI_L2_DCA,
    PAPI_L2_DCR,
    PAPI_L2_DCW,
    PAPI_L2_ICH,
    PAPI_L2_ICA,
    PAPI_L2_ICR,
    PAPI_L2_ICW,
    PAPI_L2_TCH,
    PAPI_L2_TCA,
    PAPI_L2_TCR,
    PAPI_L2_TCW,
#endif
    0
  };
  
  char *native_names[] = {
    "MEM_LOAD_LLC_HIT_RETIRED",
    //    "MEM_TRANS_RETIRED:L3_HIT",
    //    "MEM_TRANS_RETIRED",
    "MEM_LOAD_RETIRED",
    //"OFFCORE_RESPONSE_0:PF_DATA_RD:ANY_RESPONSE",
    "HW_PRE_REQ",
    "HW_PRE_REQ:L1D_MISS"
    "ix86arch::LLC_REFERENCES",
    "ix86arch::LLC_MISSES",
    "perf::PERF_COUNT_HW_CACHE_L1D:READ",
    "perf::PERF_COUNT_HW_CACHE_L1D:WRITE",
    "perf::PERF_COUNT_HW_CACHE_L1D:PREFETCH",
    "perf::PERF_COUNT_HW_CACHE_L1D:ACCESS",
    "perf::PERF_COUNT_HW_CACHE_L1D:MISS",
    "perf::PERF_COUNT_HW_CACHE_LL",
    "perf::PERF_COUNT_HW_CACHE_NODE:READ",
    "perf::PERF_COUNT_HW_CACHE_NODE:WRITE",
    "perf::PERF_COUNT_HW_CACHE_NODE:PREFETCH",
    "perf::PERF_COUNT_HW_CACHE_NODE:ACCESS",
    "perf::PERF_COUNT_HW_CACHE_NODE:MISS",
    "L1D:ALLOCATED_IN_M",
    "L1D:ALL_M_REPLACEMENT",
    "L1D:M_EVICT",
    "L1D:REPLACEMENT",
    "L2_LINES_IN",
    "L2_LINES_IN:E",
    "L2_LINES_IN:I",
    "L2_LINES_IN:S",
    "L2_LINES_OUT:DEMAND_CLEAN",
    "L2_LINES_OUT:DEMAND_DIRTY",
    "L2_LINES_OUT:PREFETCH_CLEAN",
    "L2_LINES_OUT:PREFETCH_DIRTY",
    "L2_LINES_OUT:DIRTY_ANY",
    0
  };

  int native_code;

  
  if ( ( retval =
	 PAPI_library_init( PAPI_VER_CURRENT ) ) != PAPI_VER_CURRENT )
    test_fail( __FILE__, __LINE__, "PAPI_library_init", retval );

  if ( ( retval = PAPI_create_eventset( &EventSet ) ) != PAPI_OK )
    test_fail( __FILE__, __LINE__, "PAPI_create_eventset", retval );

 
  for ( event = 0; native_names[event] != NULL; event++ ) {
  //for ( event = 0; eventlist[event] != 0; event++ ) {

    printf("testing %s\n", native_names[event]);
  
    retval = PAPI_event_name_to_code( native_names[event], &native_code );
    if ( retval != PAPI_OK ){
      test_error( __FILE__, __LINE__, "PAPI_event_name_to_code", retval );
      continue;
    }

    if ( PAPI_get_event_info( native_code,  &evinfo ) != PAPI_OK )
    test_fail( __FILE__, __LINE__, "PAPI_get_event_info", retval );
   
    PAPI_event_code_to_name( native_code, descr );
    if ( PAPI_add_event( EventSet, native_code ) != PAPI_OK ){
      fprintf(stderr, "failed to add %s\n", descr);
      continue;
    }

    printf( "\nEvent: %s\nShort: %s\nLong: %s\n",
	    evinfo.symbol, evinfo.short_descr, evinfo.long_descr );
    //printf( "       Bytes\t\tCold\t\tWarm\tPercent\n" );
   
    int access;
    char *access_names[] = {
      "pattern",
      "random",
      "linear"
    };
    for(access = 0; access <= 2; access++){
      if ( ( retval = PAPI_start( EventSet ) ) != PAPI_OK )
	test_fail( __FILE__, __LINE__, "PAPI_start", retval );
   
      gettimeofday(&start, NULL);
      switch(access){
      case 0:
	access_pattern();
	break;
      case 1:
	access_random();
	break;
      case 2:
	access_linear();
	break;
      }
      gettimeofday(&end, NULL);
      
      if ( ( retval = PAPI_read( EventSet, &value ) ) != PAPI_OK )
	test_fail( __FILE__, __LINE__, "PAPI_read", retval );
      
      if ( ( retval = PAPI_stop( EventSet, NULL ) ) != PAPI_OK )
	test_fail( __FILE__, __LINE__, "PAPI_stop", retval );
      
      double t2 =  (end.tv_sec + (end.tv_usec/1000000.0)) - (start.tv_sec + (start.tv_usec/1000000.0));

      printf("event:\t%s,\taccess:\t%s,\ttime:\t%lf,\tvalue:\t%lld\n", 
	     descr, access_names[access], t2, value);
    }
    if ( ( retval =
	   PAPI_remove_event( EventSet, native_code ) ) != PAPI_OK )
      test_fail( __FILE__, __LINE__, "PAPI_remove_event", retval );
  }
  
  if ( ( retval = PAPI_destroy_eventset( &EventSet ) ) != PAPI_OK )
    test_fail( __FILE__, __LINE__, "PAPI_destroy_eventset", retval );
  
  return 0;
  
}
