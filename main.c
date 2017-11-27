/**
   \file main.c
   \brief Asynchronous C projet simulating a rail traffic 
   \author Samuel DE VALS, Paul VIALART 

   \author Email  : samuel.devals@outook.fr, paulvialart@gmail.fr
   \date 10/2017
*/

/*
	HOWTOLAUNCH : 
	$cmake CMakeLists.txt
	$make
	$./ProGenWCMake [seed] [lockMode]
	
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#include "file/file.h"
#include "trains/train.h"

/** \def DEBUG
     Good practice : use this to debug the program 
*/
#define DEBUG if (1) printf

#define NB_TRAINS 3

//display
void printSeedChoiceError(){
	printf("Error, seed value must be in [1;1000]\n");
}

//display
void printSyncChoiceError(){
	printf("Error, you should give a parameter in [0;3] :\n");
	printf("0- mutex\n");
	printf("1- semaphore\n");
	printf("2- rwlock\n");
	printf("3- message queue\n");
}

//checking arguments
int CheckArgv(int argc, char*argv[], int*seed,int*sync){
	int result = 0;
	if(argc == 3){
		*seed = atoi(argv[1]);
		if((*seed < 1) || (*seed > 1000)){
			result = ERROR;
			printSeedChoiceError();
		}
		*sync = atoi(argv[2]);
		if((*sync < 0) || (*sync > 3)){
			result = ERROR;
			printSyncChoiceError();
		}
	}
	else{
		printSeedChoiceError();
		printSyncChoiceError();
		result = ERROR;
	}
	
	return result;
}

//initialisation struct strain
void InitStructTrain(strain *trains,char trajet[3][6], int sync,int N){
	for(int i = 0; i < N; i++){
		(trains+i)->number = i;
		(trains+i)->Ntrain = NB_TRAINS;
		(trains+i)->trajet = trajet[i];
		(trains+i)->Ntrajet = 0;
		(trains+i)->avg_travel_time = 0.0;
		(trains+i)->sync = sync;
	}
}

int main(int argc, char*argv[]){
	// DECLERATION
	int seed = ERROR, sync = ERROR;

	int res = CheckArgv(argc, argv, &seed, &sync);
	if(res == ERROR)exit(ERROR);
	
	writeSeparator();
	writeParameter(seed, sync);
	
	pthread_t ttrains[NB_TRAINS];
	strain data_trains[NB_TRAINS];
	char trajet[3][6] = {{'A','B','C','B','A','\0'},{'A','B','D','B','A','\0'},{'A','B','D','C','E','\0'}};
	
	// INITIALIZATION
	srand(seed);
	InitStructTrain(data_trains,trajet, sync,3);
	Initlines();
	Initialisation(1,ttrains,3,data_trains);
	
	sleep(RUNTIME);//attente
	
	for(int i =0; i < NB_TRAINS; i++){//end of prog
		while(pthread_cancel(ttrains[i]) != 0);
		pthread_join(ttrains[i],NULL);
		writeResult(data_trains[i].number, data_trains[i].Ntrajet, data_trains[i].avg_travel_time);
	}
		
	FreeLines(SIGINT);//free mem

	return 0;
}

