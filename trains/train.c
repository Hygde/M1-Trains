#include "train.h"

int Initialisation(int verbose,pthread_t* ttrains,int Ntrain, strain* data_trains){
	int result = 0;
	//initialisation	
	srand(time(NULL));
	if(verbose)printf("Initialisation des threads `trains` :\n");
	pthread_attr_t attr;
	//on crée l'attribut de nos threads
	if(pthread_attr_init(&attr) != 0){
		perror("pthread_attr_init()");
		result = ERROR;
		exit(ERROR);
	}
	//definition de l'attribut de nos threads
	if(pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED) != 0){
		perror("pthread_attr_setdetachstate()");
		result = ERROR;
		exit(ERROR);
	}
	//création des threads
	for(int i = 0;i < Ntrain; i++)
		pthread_create(&ttrains[i],&attr,Move,(void*)(data_trains+i));
	//destruction de l'attribut
	if(pthread_attr_destroy(&attr) != 0){
		perror("pthread_attr_destroy()");
		result = ERROR;
		exit(ERROR);
	}
	return result;
}

void* Move(void*data_train){
	//initialisation
	int N = strlen(((strain*)data_train)->trajet);
	printf("\nTrain %d a pour trajet: %s de %d etapes\n",((strain*)data_train)->number,((strain*)data_train)->trajet,N);
	//parcours du trajet en boucle
	while(1){
	//TODO : corriger le bug d'affichage des lettres en monôme
		int i;
		for(i = 0; i < N-1; i++){
			printf("Le train %d s`engage sur %c %c\n",((strain*)data_train)->number,((strain*)data_train)->trajet[i],((strain*)data_train)->trajet[i+1]);
			sleep((rand()%2)+1);
		}
		printf("Le train %d s`engage sur %c %c\n",((strain*)data_train)->number,((strain*)data_train)->trajet[i+1],((strain*)data_train)->trajet[0]);
		sleep((rand()%2)+1);
	}
	return(NULL);
}
