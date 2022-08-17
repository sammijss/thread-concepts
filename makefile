CC=g++
FLAGS=-Wall -c -g -std=c++11

all: thread-affinity\
     thread-priority\
     thread-taskset-example

thread-affinity: thread-affinity.o
	$(CC) thread-affinity.o -o thread-affinity -lpthread

thread-priority: thread-priority.o
	$(CC) thread-priority.o -o thread-priority -lpthread

thread-taskset-example: thread-taskset-example.o
	$(CC) thread-taskset-example.o -o thread-taskset-example -lpthread

thread-affinity.o: thread-affinity.cpp
	$(CC) $(FLAGS) thread-affinity.cpp

thread-priority.o: thread-priority.cpp
	$(CC) $(FLAGS) thread-priority.cpp

thread-taskset-example.o: thread-taskset-example.cpp
	$(CC) $(FLAGS) thread-taskset-example.cpp

clean:
	rm -f thread-affinity.o\
          thread-priority.o\
          thread-taskset-example.o\
          thread-affinity\
          thread-priority\
          thread-taskset-example\
