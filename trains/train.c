#include "train.h"


int Initialisation(int verbose, pthread_t* ttrains, int Ntrain, strain* data_trains){
	int result = 0;
	// initialisation	
	srand(time(NULL));
	if(verbose)printf("Initialisation des threads `trains` :\n");
	pthread_attr_t attr;
	// creating thread attributes
	if(pthread_attr_init(&attr) != 0){
		perror("pthread_attr_init()");
		result = ERROR;
		exit(ERROR);
	}
	// defining thread attr
	if(pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED) != 0){
		perror("pthread_attr_setdetachstate()");
		result = ERROR;
		exit(ERROR);
	}
	// creating threads
	for(int i = 0;i < Ntrain; i++)
		pthread_create(&ttrains[i],&attr,Move,(void*)(data_trains+i));
	// destroyong attributes
	if(pthread_attr_destroy(&attr) != 0){
		perror("pthread_attr_destroy()");
		result = ERROR;
		exit(ERROR);
	}
	return result;
}

void* Move(void*data_train){

	// initialisation
	int N = strlen(((strain*)data_train)->trajet);
	printf("Train %d a pour trajet: %s de %d etapes\n\n",((strain*)data_train)->number,((strain*)data_train)->trajet,N);
	// looping throug the path
	while(1){
	/**
		\todo 
		synchronize threads : 	1. 0 to N train(s) on the same path but only one direction a time
					2. A train can't overtake an other
	*/
		for(int i = 0; i < N; i++){
			if(((strain*)data_train)->trajet[i] != ((strain*)data_train)->trajet[(i+1)%N]){
				printf("Le train %d s`engage sur %c %c debug : %d %d\n",((strain*)data_train)->number,((strain*)data_train)->trajet[i],((strain*)data_train)->trajet[(i+1)%N],i,(i+1)%N);
				sleep((rand()%2)+1);
			}
		}
	}
	return(NULL);
}
