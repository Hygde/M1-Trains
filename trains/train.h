#ifndef __TRAIN_H__ // permet d'eviter une double inclusion
#define __TRAIN_H__

/**
  \file trains.h
  \brief Train structure library

   Trains are the basic object used to run the program

   \author Samuel DE VALS, Paul VIALART 

   \author Email  : samuel.devals@outook.fr, paulvialart@gmail.fr
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

#ifdef __cplusplus// signal au compilateur C++  le début de la section à traiter comme du C
extern "C" {
#endif

  /** 
   \typedef strain

   Structure used to build trains 
  */

typedef struct strain{
	int number;	/**<train number*/
	char*trajet;	/**<path that the train will follow*/
}strain;

  /**
  \fn   int Initialisation (int verbose, pthread_t* ttrains, int Ntrain, strain* data_trains);
  \brief Init nTrains trains

  \param[in] int displaying debug messages
  \param[in] ttrains trains threads
  \param[in] nTrains Number of trains
  \param[in] data_trains strain structure

  \param[out] result 0 or ERROR
  
  \par Exemple:
	Initialisation(1,ttrains,3,data_trains);
  */
int Initialisation(int verbose,pthread_t* ttrains, int Ntrain, strain* data_trains);


  /**
  \fn   void*Move (void*trajet);
  \brief Thread function used to make the trains move

  \param[in] trajet The path followed by the train
  
  \par Exemple:
	pthread_create(&ttrains[i],&attr,Move,(void*)(data_trains+i));
  */
void*Move(void*trajet);

#ifdef __cplusplus// signal au compilateur C++  la fin de la section à traiter comme du C
}
#endif

#endif
