#!/usr/bin/env Rscript
a = read.table('table', header=T)

events = unique(a$event)
accesses = unique(a$access)
threads = unique(a$threads)

b_access = list()
b = data.frame()

for(thread in threads){
  for(access in accesses){
    sel = a$access == access & a$threads == thread
    mean_time = mean(a$time[sel])
    b_access[[access]] = data.frame(mean_time)
    b_access[[access]]$sd_time = sd(a$time[sel])
    b_access[[access]]$access = access
    b_access[[access]]$threads = thread
    for(event in events){
      b_access[[access]][[event]] = a$count[sel & a$event == event]
    }
    b = rbind(b, b_access[[access]])
  }
}

b$LLC_MISS_RATIO = b$LLC_MISSES/b$LLC_REFERENCES
b$PAPI_L2_DCM_RATIO = b$PAPI_L2_DCM/b$PAPI_L2_DCA
b$PERF_COUNT_HW_CACHE_L1D_MISS_RATIO = b$'perf::PERF_COUNT_HW_CACHE_L1D:MISS' / b$'perf::PERF_COUNT_HW_CACHE_L1D:ACCESS'
