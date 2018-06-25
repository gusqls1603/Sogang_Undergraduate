CC = gcc
CFLAGS = -W -Wall
TARGET = 20141583.out
SRCS = 20141583.h 20141583.c
OBJECTS = 20141583.o

$(TARGET):$(OBJECTS)
	$(CC) -o $(TARGET) $(OBJECTS) $(CFLAGS)

$(OBJECTS): 20141583.c
	$(CC) -c 20141583.c $(CFLAGS)

clean:
	rm -rf *.o *.out
