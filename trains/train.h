/**
  \file trains.h
  \brief Train structure library

   Trains are the basic object used to run the program

   \author Samuel DE VALS, Paul VIALART 

   \author Email  : 
   \date 10/2017
*/


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define ERROR -1

  /** 
   \typedef strain

   Structure used to build trains 
  */

typedef struct strain{
	int number;	/**<train number*/
	char*trajet;	/**<path that the train will follow*/
}strain;

int Initialisation(int verbose,pthread_t* ttrains, int Ntrain, strain* data_trains);
void*Move(void*trajet);

