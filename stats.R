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
b$stall_ratio = b$PAPI_STL_ICY / b$TOT_CYC

for(name in names(b)){

  if(class(b[[name]]) != "numeric") next

  dev.new()
  first = T
  ylim=c(0,max(b[[name]]))
  colors = rainbow(length(accesses))
  index = 0
  
  for(access in accesses){
    index = index + 1
    sel = b$access == access
    if(first){
      plot(b[[name]][sel] ~ b$threads[sel], 
           main=paste(name, 'vs # of threads'),
           ylim=ylim,
	   ylab=name,
	   xlab='threads',
           col = colors[index]
           )
      first = F
    } else {
      points(b[[name]][sel] ~ b$threads[sel],
             col = colors[index]
            )
    }
  }
  legend(x='topright', 
         legend=accesses,
	 col=colors,
	 lwd=3)
}
