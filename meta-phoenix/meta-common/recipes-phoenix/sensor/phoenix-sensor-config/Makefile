
CFLAGS = $(shell pkg-config --cflags json-c) -I. -static
LDFLAGS = $(shell pkg-config --libs json-c)

all: gensdr genraw

gensdr: gensdr.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

genraw: genraw.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY: clean

clean:
	rm -rf *.o gensdr
	rm -rf *.o genraw

