/**
   \file main.c
   \brief Asynchronous C projet simulating a rail traffic 
   \author Samuel DE VALS, Paul VIALART 

   \author Email  : samuel.devals@outook.fr, paulvialart@gmail.fr
   \date 10/2017
*/


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#include "trains/train.h"

/** \def DEBUG
     Good practice : use this to debug the program 
*/
#define DEBUG if (1) printf

#define NB_TRAINS 3

int CheckArgv(int argc, char*argv[]){
	int result = ERROR;
	if(argc == 2){
		result = atoi(argv[argc-1]);
		if((result < 0) || (result > 3)){
			result = ERROR;
			printf("Error, you should give a parameter in [0;3] :\n");
			printf("0- mutex\n");
			printf("1- semaphore\n");
			printf("2- rwlock\n");
			printf("3- message queue\n");
		}
	}
	else{
		printf("Error, you should give a parameter in [0;3] :\n");
		printf("0- mutex\n");
		printf("1- semaphore\n");
		printf("2- rwlock\n");
		printf("3- message queue\n");
	}
	
	return result;
}

void InitStructTrain(strain *trains,char trajet[3][6], int sync,int N){
	for(int i = 0; i < N; i++){
		(trains+i)->number = i;
		(trains+i)->Ntrain = NB_TRAINS;
		(trains+i)->trajet = trajet[i];
		(trains+i)->sync = sync;
	}
}

int main(int argc, char*argv[]){

	int sync = CheckArgv(argc, argv);
	if(sync == ERROR)exit(ERROR);
	
	printf("\tProjet PTR\n");
	// delcaring variables
	pthread_t ttrains[NB_TRAINS];
	strain data_trains[NB_TRAINS];
	char trajet[3][6] = {{'A','B','C','B','A','\0'},{'A','B','D','B','A','\0'},{'A','B','D','C','E','\0'}};

	// DEBUG("%s %s %d\n", __FILE__, __func__, __LINE__);
	
	// initialisation
	InitStructTrain(data_trains,trajet, sync,3);
	Initlines();
	Initialisation(1,ttrains,3,data_trains);

	pthread_exit(0);
}

