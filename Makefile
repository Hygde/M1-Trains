FLAG=-Wall
LIBRARIES=-lpthread
IN=main.c trains/train.c
OUT=-o projet

all:
	gcc $(FLAG) $(IN) $(LIBRARIES) $(OUT)


