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
	pthread_mutex_init(&fifo, NULL);
	pthread_barrier_init(&barrier,NULL, 3);
	for(int i = 0; i < NB_LINES; i++){
		for(int j = 0; j < NB_LINES; j++){
			matrix_lines[i][j].nombre_train = 0;
			matrix_lines[i][j].timestamp = 0;
			matrix_lines[i][j].travel_time = 0;
			pthread_mutex_init(&matrix_lines[i][j].mutex_line, NULL);
		}
	}
	return 0;
}

//time to travel. Trains aren't able to overtake an other
int travelTime(int i, int j){
	int result = (rand()%3+1);//travel time E [1;3]
	if(matrix_lines[i][j].travel_time - time(0) > 0){//an other trains is on the line
		result += matrix_lines[i][j].travel_time - time(0);
		if(result > 3) result = 3;
	}
	matrix_lines[i][j].travel_time = time(0)+result;//updating flag
	return result;
}

//lock a line + wait free line + inc nb train on
int GetLine(int i, int j){
	pthread_mutex_lock(&fifo);//on empêche tous les autres threads de travailler
	pthread_mutex_lock(&matrix_lines[i][j].mutex_line);//on lock le sens du trajet
	matrix_lines[i][j].nombre_train++;//inc le nombre trains occupant la voie
	if(matrix_lines[i][j].nombre_train == 1)pthread_mutex_lock(&matrix_lines[j][i].mutex_line);//on verrouille le sens opposé si on est le premier train
	pthread_mutex_unlock(&matrix_lines[i][j].mutex_line);//on unlock le sens du trajet
	pthread_mutex_unlock(&fifo);//on unlock la file d'attente pour laisser le prochain train sur la même ligne arriver
}

//unlock line when travel complete
int SignalUnusedLine(int i, int j){
	pthread_mutex_lock(&matrix_lines[i][j].mutex_line);
	matrix_lines[i][j].nombre_train--;
	if(matrix_lines[i][j].nombre_train == 0)pthread_mutex_unlock(&matrix_lines[j][i].mutex_line);
	pthread_mutex_unlock(&matrix_lines[i][j].mutex_line);
}

//Free memory when ^C
void FreeLines(int sig){
	if(sig == SIGINT){
		printf("\nFreeLines\n");
		pthread_mutex_destroy(&fifo);
		pthread_barrier_destroy(&barrier);
		for(int i = 0; i < NB_LINES; i++){
			for(int j =0; j < NB_LINES; j++){
				pthread_mutex_destroy(&matrix_lines[i][j].mutex_line);
			}
		}
		exit(EXIT_SUCCESS);//end the program
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
	printf("Train %d a pour trajet: %s de %d etapes\n",((strain*)data_train)->number,((strain*)data_train)->trajet,N);
	pthread_barrier_wait(&barrier);
	if( ((strain*)data_train)->number == 0) printf("\n");
	pthread_barrier_wait(&barrier);
	// looping throug the path
	while(1){
	
		for(int i = 0; i < N; i++){
			if(((strain*)data_train)->trajet[i] != ((strain*)data_train)->trajet[(i+1)%N]){
				printf("Le train %d s`engage sur %c %c debug : %d %d\n",((strain*)data_train)->number,((strain*)data_train)->trajet[i],((strain*)data_train)->trajet[(i+1)%N],i,(i+1)%N);
				x = CvtCharToI(((strain*)data_train)->trajet[i]);
				y = CvtCharToI(((strain*)data_train)->trajet[(i+1)%N]);
				GetLine(x,y);
				sleep(travelTime(x,y));
				SignalUnusedLine(x,y);
				printf("Le train %d a atteint sa destination sur %c %c debug : %d %d\n",((strain*)data_train)->number,((strain*)data_train)->trajet[i],((strain*)data_train)->trajet[(i+1)%N],i,(i+1)%N);
			}
		}
	}
	return (NULL);
}
