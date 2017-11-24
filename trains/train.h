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
#include <signal.h>
#include <time.h>

#define ERROR -1
#define NB_LINES 5
#define RUNTIME  30//20*60 //20 minutes per execution

//index for matrix
#define A 0
#define B 1
#define C 2
#define D 3
#define E 4

#ifdef __cplusplus// signal au compilateur C++  le début de la section à traiter comme du C
extern "C" {
#endif

pthread_mutex_t mutex_fifo;
sem_t sem_fifo;
pthread_rwlock_t rwlock_fifo;
pthread_barrier_t barrier;

//representation of lines
typedef struct lines{
	int nombre_train;
	time_t timestamp;
	long int travel_time;

	pthread_mutex_t mutex_line;
	sem_t sem_line;
	pthread_rwlock_t rwlock_line;
}lines;

//Matrix of the ligne
lines matrix_lines[NB_LINES][NB_LINES];


  /** 
   \typedef strain

   Structure used to build trains 
  */

typedef struct strain{
	int number;	/**<train number*/
	int Ntrain; 	/**<Total count of train*/
	char*trajet;	/**<path that the train will follow*/
	int Ntrajet;
	double avg_travel_time;
	int sync;	/**<variable use to select the good sync system*/
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

int Initlines();//initialize

int travelTime(strain* train, int x, int y);//calcul the time of travels, trains can't overtakes an other

void GetLine(int sync, int x, int y);//call the good sync
void GetLineMutex(int i, int j);//lock a line with mutex
void GetLineSemaphore(int i, int j);//lock a line with semaphore
void GetLineRwlock(int i, int j);//lock a line with rwlock

void SignalUnusedLine(int sync, int x, int y);//call the good sync
void SignalUnusedLineMutex(int i, int j);//signal to others threads they can use it with mutex
void SignalUnusedLineSemaphore(int i, int j);//signal to others threads they can use it with semaphore
void SignalUnusedLineRwlock(int i, int j);//signal to others threads they can use it with rwlock

void FreeLines(int sig);//free memory

int CvtCharToI(char carac);//convert ascii character to int


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
