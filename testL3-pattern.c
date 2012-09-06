#include <stdio.h>
#include <stdint.h>
#include<stdlib.h>
#include<time.h>
#include <papi.h>
#include <assert.h>

#ifdef _OPENMP
#include <omp.h>
#endif

#define MULT 20
#define INTS_IN_L3 (1024*1024*20/sizeof(int))

uint32_t *arrMuchBiggerThanL3=NULL;
int *indices = 0;
long long max = (MULT * INTS_IN_L3);

uint32_t readValue, p;
//Step by 64k -- 64k = 2048 ints
uint32_t delta = 8192;
int EventSet = PAPI_NULL;
int retval;
char descr[PAPI_MAX_STR_LEN];

int numThreads;
#ifdef _OPENMP
int thread;
#endif


volatile long long i, n;
struct timeval start, end;


void test_error(char * file, int line, char * message, int val){
fprintf(stderr, "#%s: %d; %s: %d\n", 
	  file, line, message, val);
}

void test_fail(char * file, int line, char * message, int val){
test_error(file, line, message, val);
exit(1);  
}

void access_pattern(){
#ifdef _OPENMP
#pragma omp parallel for private(n, i)
#endif
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
#ifdef _OPENMP
#pragma omp parallel for private(n, i)
#endif
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
#ifdef _OPENMP
#pragma omp parallel for private(n, i)
#endif
  for(i=0; i<max; i++){
    //Access a[max - i], followed by a[i]
    readValue = arrMuchBiggerThanL3[max-i];
    p = arrMuchBiggerThanL3[i];
    p += readValue;
    arrMuchBiggerThanL3[i] = p;
  }
}

void do_event(int *mask, char ** eventNames){
  long long *values = 0;
  int access;
  int event;
  char *access_names[] = {
    "pattern",
    "random",
    "linear"
  };
  int numEvents = PAPI_num_events(EventSet);

  if(!numEvents)
    return;

  printf("#numEvents: %d\n", numEvents);
  i = 0;
  printf("access\ttime\tthreads");
  for(event = 0; event < numEvents; event++){
    while(mask[i] != 2) i++;
    printf("\t%s", eventNames[i]);
    i++;
  }
  printf("\n");
  
  values = (long long *)malloc(sizeof(long long) * numEvents);
  assert(values);

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

    if ( ( retval = PAPI_read( EventSet, values ) ) != PAPI_OK ){
      free(values);
      test_fail( __FILE__, __LINE__, "PAPI_read", retval );
    }
 
    if ( ( retval = PAPI_stop( EventSet, NULL ) ) != PAPI_OK ){
      free(values);
      test_fail( __FILE__, __LINE__, "PAPI_stop", retval );
    }
      
    double t2 =  (end.tv_sec + (end.tv_usec/1000000.0)) - (start.tv_sec + (start.tv_usec/1000000.0));

    printf("%s\t%lf\t%d", 
	   access_names[access], t2, numThreads);
    for(event = 0; event < numEvents; event++)
      printf("\t%lld", values[event]);
    printf("\n");

    fflush(stdout);
  }
  free(values);
}

void chooseEvents(int *eventlist, int *mask, int *remaining, char ** eventNames, 
		  int numEvents){
  int setSize = 0, event;
  for ( event = 0; event < numEvents; event++ ){
    if(mask[event])
      mask[event] = 1;
    if(!eventlist[event] && !mask[event]){
      mask[event] = 1;
      --*remaining;
    }
  }
  for ( event = 0; event < numEvents; event++ ){
    if(mask[event])
      continue;
    if (PAPI_add_event( EventSet, eventlist[event] ) != PAPI_OK ){
      if(!setSize){
	fprintf(stderr, "#failed to add %s\n", eventNames[event]);
	mask[event] = 1;
	--*remaining;
	return;
      }
      continue;
    }
    mask[event] = 2;
    (*remaining)--;
    setSize++;
    // more than 4 events seems to cause negative values
    //if(setSize >= 4) break;
  }
  /*
    if ( PAPI_get_event_info( eventlist[event],  &evinfo ) != PAPI_OK )
      test_fail( __FILE__, __LINE__, "PAPI_get_event_info", retval );
   
    printf("#testing %s\n", descr);


    printf( "#Event: %s\n#Short: %s\n#Long: %s\n",
	    evinfo.symbol, evinfo.short_descr, evinfo.long_descr );

  }
  */
}

int main(){
#ifdef _OPENMP
  printf("#OPENMP in effect\n");
#endif

#ifdef _OPENMP
#pragma omp parallel
  {
  if(!omp_get_thread_num()){
  numThreads = omp_get_num_threads();
  printf("#%d threads\n", numThreads);
}
}
#endif


  arrMuchBiggerThanL3 = (uint32_t*)malloc((max) * sizeof(uint32_t));
  indices = (int*)malloc((max) * sizeof(int));

  long long k=0;
  
  printf("#RAND_MAX: %ld\n", (long)RAND_MAX);

  //Initialize
  for(i=0; i<max; i++){
    arrMuchBiggerThanL3[i] = ((++k) % (65535));
    indices[i] = ((unsigned long)rand()) % max;
  }

  printf("#Memory used: %1f MB\n", ((double) (max * sizeof(uint32_t)))/(1024.0 * 1024.0));
  
  if ( ( retval =
	 PAPI_library_init( PAPI_VER_CURRENT ) ) != PAPI_VER_CURRENT )
    test_fail( __FILE__, __LINE__, "PAPI_library_init", retval );

#ifdef _OPENMP
  if((retval = PAPI_thread_init(&omp_get_thread_num)) != PAPI_OK)
    test_fail( __FILE__, __LINE__, "PAPI_thread_init", retval );
#endif
  int event;
  PAPI_event_info_t evinfo;
  
  char* preset_names[] = {
    "PAPI_L3_TCM",
    "PAPI_L3_DCA",
    "PAPI_L3_TCA",
    "PAPI_L3_DCR",
    "PAPI_LD_INS",
    "PAPI_TOT_CYC",
    "PAPI_STL_ICY",
    "PAPI_TOT_INS",
    "PAPI_L2_DCA",
    "PAPI_L2_DCM",
    "PAPI_L1_DCM",
    /*
    "PAPI_L1_DCA",
    "PAPI_L1_TCM",
    "PAPI_L1_DCM",
    "PAPI_L1_DCH",
    "PAPI_L2_DCA",
    "PAPI_L2_DCM",
    "PAPI_L2_DCH",
    "PAPI_L3_DCA",
    "PAPI_L3_TCA",
    "PAPI_L3_TCM",
    "PAPI_TOT_INS",
    "PAPI_TOT_CYC",
    "PAPI_STL_ICY",
    */    
#if 0
    "PAPI_L1_LDM",
    "PAPI_L1_STM",
    "PAPI_L1_DCR",
    "PAPI_L1_DCW",
    "PAPI_L1_ICM",
    "PAPI_LD_INS",
    "PAPI_SR_INS",
    "PAPI_LST_INS",
    "PAPI_L2_DCR",
    "PAPI_L2_DCW",
    "PAPI_CSR_TOT",
    "PAPI_MEM_SCY",
    "PAPI_MEM_RCY",
    "PAPI_MEM_WCY",
    "PAPI_L1_ICH",
    "PAPI_L1_ICA",
    "PAPI_L1_ICR",
    "PAPI_L1_ICW",
    "PAPI_L1_TCH",
    "PAPI_L1_TCA",
    "PAPI_L1_TCR",
    "PAPI_L1_TCW",
    "PAPI_L2_DCM",
    "PAPI_L2_ICM",
    "PAPI_L2_TCM",
    "PAPI_L2_LDM",
    "PAPI_L2_STM",
    "PAPI_L2_DCH",
    "PAPI_L2_DCA",
    "PAPI_L2_DCR",
    "PAPI_L2_DCW",
    "PAPI_L2_ICH",
    "PAPI_L2_ICA",
    "PAPI_L2_ICR",
    "PAPI_L2_ICW",
    "PAPI_L2_TCH",
    "PAPI_L2_TCA",
    "PAPI_L2_TCR",
    "PAPI_L2_TCW",
#endif
    0
  };

  char *native_names[] = {
    "PAPI_L2_DCM",
    //"LAST_LEVEL_CACHE_REFERENCES",
    //"L2_RQSTS:CODE_RD_MISS",
    "PAPI_TOT_CYC",
#if 0
    "PAPI_L3_TCM",
    "PAPI_L3_DCA",
    "PAPI_L3_TCA",
    "PAPI_L3_DCR",

    "PAPI_LD_INS",
    "PAPI_STL_ICY",
    "PAPI_TOT_INS",
    "PAPI_L2_DCA",
    "PAPI_L1_DCM",
//
    "MEM_TRANS_RETIRED:L3_HIT",
    "MEM_TRANS_RETIRED",
    "MEM_LOAD_RETIRED",
    "HW_PRE_REQ",
    "HW_PRE_REQ:L1D_MISS"
    "ix86arch::LLC_REFERENCES",
    "ix86arch::LLC_MISSES",
    "perf::PERF_COUNT_HW_CACHE_L1D",
    "perf::PERF_COUNT_HW_CACHE_L1D:READ",
    "perf::PERF_COUNT_HW_CACHE_L1D:WRITE",
    "perf::PERF_COUNT_HW_CACHE_L1D:PREFETCH",
    "perf::PERF_COUNT_HW_CACHE_L1D:ACCESS",
    "perf::PERF_COUNT_HW_CACHE_L1D:MISS",
    "perf::PERF_COUNT_HW_CACHE_LL",
    "perf::PERF_COUNT_HW_CACHE_NODE",
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
    "L2_LINES_OUT",
    "L2_LINES_OUT:DEMAND_CLEAN",
    "L2_LINES_OUT:DEMAND_DIRTY",
    "L2_LINES_OUT:PREFETCH_CLEAN",
    "L2_LINES_OUT:PREFETCH_DIRTY",
    "L2_LINES_OUT:DIRTY_ANY",
    "L2_RQSTS",
    "L2_RQSTS:ALL_DEMAND_DATA_RD",
    "L2_RQSTS:ALL_DEMAND_DATA_HIT",
    "L2_RQSTS:ALL_PF",
    "L2_RQSTS:PF_HIT",
    "L2_RQSTS:PF_MISS",
    "L2_RQSTS:RFO_ANY",
    "L2_RQSTS:RFO_HITS",
    "L2_RQSTS:RFO_MISS",
    "LLC_REFERENCES",
    "LLC_MISSES",
    "L3_LAT_CACHE",
    "L3_LAT_CACHE:MISS",
    "L3_LAT_CACHE:REFERENCE",
    "MEM_LOAD_LLC_HIT_RETIRED",
    "OFFCORE_REQUESTS",
    "OFFCORE_REQUESTS:ALL_DATA_READ",
    "OFFCORE_REQUESTS:DEMAND_DATA_RD",
    "UNHALTED_CORE_CYCLES",
    "UNHALTED_REFERENCE_CYCLES",
    "PACKAGE_ENERGY:PACKAGE0",
#endif
    0
  };
    
  int native_code, *native_codes = 0;
  int numNative = 0, numPreset = 0;
  int *presetMask = 0, *nativeMask = 0;
  int *preset_codes = 0;

  while(native_names[numNative]) numNative++;
  while(preset_names[numPreset]) numPreset++;

  preset_codes = (int*)calloc(numPreset, sizeof(int));
  native_codes = (int*)calloc(numNative, sizeof(int));
  presetMask = (int*)calloc(numPreset, sizeof(int));
  nativeMask = (int*)calloc(numNative, sizeof(int));

  assert(nativeMask && presetMask && native_codes && preset_codes);

  for(event = 0; event < numPreset; event++){
    retval = PAPI_event_name_to_code(preset_names[event], &preset_codes[event]);
    if ( retval != PAPI_OK ){
      test_error( __FILE__, __LINE__, "PAPI_event_name_to_code", retval );
      continue;
    }
  }


  for(event = 0; event < numNative; event++){
    retval = PAPI_event_name_to_code( native_names[event], &native_codes[event] );
    if ( retval != PAPI_OK ){
      test_error( __FILE__, __LINE__, "PAPI_event_name_to_code", retval );
      continue;
    }
  }

  int remainingNative = numNative;
  int remainingPreset = numPreset;

  if ( ( retval = PAPI_create_eventset( &EventSet ) ) != PAPI_OK )
    test_fail( __FILE__, __LINE__, "PAPI_create_eventset", retval );

  while(remainingPreset){
    chooseEvents(preset_codes, presetMask, &remainingPreset, preset_names, 
		 numPreset);
    do_event(presetMask, preset_names);

    if ( ( retval =
	   PAPI_cleanup_eventset( EventSet ) ) != PAPI_OK )
      test_fail( __FILE__, __LINE__, "PAPI_cleanup_eventset", retval );
  }

  while(remainingNative){
    chooseEvents(native_codes, nativeMask, &remainingNative, native_names,
		 numNative);
    do_event(nativeMask, native_names);

    if ( ( retval =
	   PAPI_cleanup_eventset( EventSet ) ) != PAPI_OK )
      test_fail( __FILE__, __LINE__, "PAPI_cleanup_eventset", retval );
  }

  if ( ( retval = PAPI_destroy_eventset( &EventSet ) ) != PAPI_OK )
    test_fail( __FILE__, __LINE__, "PAPI_destroy_eventset", retval );
  
  return 0;
  
}
