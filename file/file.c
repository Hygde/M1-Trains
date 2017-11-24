#include "file.h"

int writeSeparator(){
	int result = 0;
	FILE*file = fopen(FILE_NAME,"a");
	if(file == NULL){
		printf("Error while opening file\n");
		result = ERROR;
	}
	else{
		for(int i =0; i < 3; i++)
			fprintf(file,"------------------------------------------------------------------\n");
		fclose(file);
	}
	return 0;	
}

int writeParameter(int seed, int sync){
	int result = 0;
	FILE*file = fopen(FILE_NAME,"a");
	if(file == NULL){
		printf("Error while opening file\n");
		result = ERROR;
	}
	else{
		fprintf(file,"Param%ctre d' ex%ccution : seed = %d ",232,233,seed);
		if(sync == 0)fprintf(file,"sync = mutex\n");
		if(sync == 1)fprintf(file,"sync = semaphore\n");
		if(sync == 2)fprintf(file,"sync = rwlock\n");
		if(sync == 3)fprintf(file,"sync = message queue\n");
		fclose(file);
	}
	return result;
}

int writeResult(int thread_id, int Ntrajet, double avg_travel_time){
	int result = 0;
	FILE*file = fopen(FILE_NAME,"a");
	if(file == NULL){
		printf("Error while opening file\n");
		result = ERROR;
	}
	else{
		fprintf(file,"Le temps moyen des trajets est %f secondes sur %d trajets effectu%c par le thread %d\n",avg_travel_time, Ntrajet, 233, thread_id);
		fclose(file);
	}
	return result;
}
