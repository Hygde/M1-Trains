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

void InitStructTrain(strain *trains,char trajet[3][6],int N){
	for(int i = 0; i < N; i++){
		(trains+i)->number = i;
		(trains+i)->trajet = trajet[i];
	}
}

int main(int argc, char*argv[]){

	printf("\tProjet PTR\n");
	// delcaring variables
	pthread_t ttrains[3];
	strain data_trains[3];
	char trajet[3][6] = {{'A','B','C','B','A','\0'},{'A','B','D','B','A','\0'},{'A','B','D','C','E','\0'}};

	// DEBUG("%s %s %d\n", __FILE__, __func__, __LINE__);
	
	// initialisation
	InitStructTrain(data_trains,trajet,3);
	Initialisation(1,ttrains,3,data_trains);
	pthread_exit(0);
}
