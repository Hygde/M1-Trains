#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define ERROR -1

typedef struct strain{
	int number;
	char*trajet;
}strain;

int Initialisation(int verbose,pthread_t* ttrains, int Ntrain, strain* data_trains);
void*Move(void*trajet);

