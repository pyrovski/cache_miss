targets=testL3 testL3-only testL3-pattern testL3-random

CFLAGS=-O0 -I$(HOME)/local/include -Xlinker -rpath=$(HOME)/local/lib/ -L$(HOME)/local/lib/ -lpapi -g -fopenmp

all: $(targets)

clean:
	rm -f $(targets) *~
