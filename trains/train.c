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

//Initialize all pthread_cond
int Initlines(){
	if(signal(SIGINT, FreeLines) == SIG_ERR) printf("\nImpossible d avoir le signal ctrl C");
	pthread_mutex_init(&mutex_access_matrix, NULL);
	for(int i = 0; i < NB_LINES; i++){
		for(int j = 0; j < NB_LINES; j++){
			matrix_lines[i][j].nombre_train = 0;
			matrix_lines[i][j].timestamp = 0;
			matrix_lines[i][j].travel_time = 0;
			pthread_mutex_init(&matrix_lines[i][j].line, NULL);
			pthread_cond_init(&matrix_lines[i][j].cond_free, NULL);
		}
	}
	return 0;
}

//lock a line + wait free line + inc nb train on
int GetLigne(int i, int j){
	pthread_mutex_lock(&mutex_access_matrix);
	if((matrix_lines[i][j].nombre_train > 0) || (pthread_mutex_trylock(&matrix_lines[j][i].line) == 0))//lock the opposite sens
		matrix_lines[i][j].nombre_train++;//inc le nombre de train
	else{//Error while locking down the way
		pthread_mutex_unlock(&mutex_access_matrix);
		pthread_cond_wait(&matrix_lines[j][i].cond_free, &matrix_lines[j][i].line);//passive wait for the line

		pthread_mutex_lock(&mutex_access_matrix);//only 1 thread can modify at a time
		pthread_mutex_trylock(&matrix_lines[j][i].line);
		matrix_lines[i][j].nombre_train++;//inc le nombre de train		
	}
	pthread_mutex_unlock(&mutex_access_matrix);
}

//unlock the the line when unused
int SignalUnusedLine(int i, int j){
	pthread_mutex_lock(&mutex_access_matrix);
	matrix_lines[i][j].nombre_train--;//dec le nombre de train
	if(matrix_lines[i][j].nombre_train == 0){
		pthread_mutex_unlock(&matrix_lines[j][i].line);
		pthread_cond_broadcast(&matrix_lines[j][i].cond_free);//signals to sleeping thread to wake up
	}
	pthread_mutex_unlock(&mutex_access_matrix);
}

//Free memory when ^C
void FreeLines(int sig){
	if(sig == SIGINT){
		printf("\nFreeLines\n");
		pthread_mutex_destroy(&mutex_access_matrix);
		for(int i = 0; i < NB_LINES; i++){
			for(int j =0; j < NB_LINES; j++){
				pthread_mutex_destroy(&matrix_lines[i][j].line);
				pthread_cond_destroy(&matrix_lines[i][j].cond_free);
			}
		}
		exit(0);//end the program
	}
}

//convert ascii character to int index for matrix
int CvtCharToI(char carac){
	int result;
	switch(carac){
		case 'A':
			result = A;
		break;
		case 'B':
			result = B;
		break;
		case 'C':
			result = C;
		break;
		case 'D':
			result = D;
		break;
		case 'E':
			result = E;
		break;
		default:
			result = ERROR;
	}
	return result;
}

//thread execution
void* Move(void*data_train){
	
	// initialisation
	int N = strlen(((strain*)data_train)->trajet), x = ERROR, y = ERROR;
	printf("Train %d a pour trajet: %s de %d etapes\n\n",((strain*)data_train)->number,((strain*)data_train)->trajet,N);
	// looping throug the path
	while(1){
	
		for(int i = 0; i < N; i++){
			if(((strain*)data_train)->trajet[i] != ((strain*)data_train)->trajet[(i+1)%N]){
				printf("Le train %d s`engage sur %c %c debug : %d %d\n",((strain*)data_train)->number,((strain*)data_train)->trajet[i],((strain*)data_train)->trajet[(i+1)%N],i,(i+1)%N);
				x = CvtCharToI(((strain*)data_train)->trajet[i]);
				y = CvtCharToI(((strain*)data_train)->trajet[(i+1)%N]);
				GetLigne(x,y);
				sleep((rand()%2)+1);
				SignalUnusedLine(x,y);
				printf("Le train %d a atteint sa destination sur %c %c debug : %d %d\n",((strain*)data_train)->number,((strain*)data_train)->trajet[i],((strain*)data_train)->trajet[(i+1)%N],i,(i+1)%N);
			}
		}
	}
	return (NULL);
}
