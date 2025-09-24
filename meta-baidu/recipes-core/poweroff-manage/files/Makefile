CC ?= gcc
CFLAGS = -Wall -Wextra -I ./*.h
LDFLAGS += -W
TARGET = poweroff-manage

all: $(TARGET)

$(TARGET): poweroff-manage.o
	$(CC) $^ -o $@ $(LDFLAGS) 

poweroff-manage.o: poweroff-manage.c
	$(CC) -c $<

clean:
	rm -f *.o $(TARGET)