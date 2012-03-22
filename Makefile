CC=gcc
CFLAGS=-g -Wall -lzmq

all: 
	$(CC) $(CFLAGS) src/*.c -o topicstore
    
clean:
	rm -rf *o topicstore