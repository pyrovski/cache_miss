targets=testL3 testL3-only testL3-pattern testL3-random

CFLAGS=-O0 -I$(HOME)/local/include -Xlinker -rpath=$(HOME)/local/lib64/ -L$(HOME)/local/lib64/ -lpapi

all: $(targets)

clean:
	rm -f $(targets) *~
