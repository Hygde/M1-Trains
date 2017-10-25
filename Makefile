FLAG=-Wall
LIBRARIES=-lpthread
OUT=-o projet

main.o: main.c trains/train.h
	gcc -c main.c -o main.o

train.o: trains/train.c
	gcc -c trains/train.c -o train.o

all: main.o train.o
	gcc $(FLAG) main.o train.o $(LIBRARIES) $(OUT)
	rm *.o
