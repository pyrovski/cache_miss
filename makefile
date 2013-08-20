targets=testL3 testL3-only testL3-pattern testL3-random

CFLAGS=-O0 -I$(HOME)/local/include -g -fopenmp
LDFLAGS=-Xlinker -rpath=$(HOME)/local/lib/ -L$(HOME)/local/lib/ -lpapi -fopenmp

.suffixes=.c

%:%.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

all: $(targets)

clean:
	rm -f $(targets) *~
