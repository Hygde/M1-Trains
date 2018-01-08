#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>           /* For O_* constants */
#include <mqueue.h>
#include "train.h"

// #define QUEUE_NAME  "/test_queue"
#define MAX_SIZE    1024
#define MSG_STOP    "exit"

// DEBUG("%s %s %d\n", __FILE__, __func__, __LINE__);


int initialisation(int verbose, pthread_t* ttrains, int Ntrain, strain* data_trains){
	int result = 0;
	// initialisation
	if(verbose)printf("initialisation des threads `trains` :\n");
	pthread_attr_t attr;
	// creating thread attributes
	if(pthread_attr_init(&attr) != 0){
		perror("pthread_attr_init()");
		result = ERROR;
		exit(ERROR);
	}
	// defining thread attr
	if(pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_JOINABLE) != 0){
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
int initLines(){
	if(signal(SIGINT, freeLines) == SIG_ERR) printf("\nImpossible d avoir le signal ctrl C");
	pthread_mutex_init(&mutex_fifo, NULL);
	sem_init(&sem_fifo,0,1);
	pthread_rwlock_init(&rwlock_fifo,NULL);
	//mqd_fifo = mq_open(QUEUE_NAME, O_CREAT | O_RDWR, 0755, NULL);
	pthread_barrier_init(&barrier,NULL, 3);
	for(int i = 0; i < NB_LINES; i++){
		for(int j = 0; j < NB_LINES; j++){
			//do this job in all cases
			matrix_lines[i][j].nombre_train = 0;
			matrix_lines[i][j].timestamp = 0;
			matrix_lines[i][j].travel_time = 0;

			//todo : add a selection of sync system, optimize memory use
			pthread_mutex_init(&matrix_lines[i][j].mutex_line, NULL);
			sem_init(&matrix_lines[i][j].sem_line,0,1);
			pthread_rwlock_init(&matrix_lines[i][j].rwlock_line, NULL);
		}
	}
	return 0;
}

//time to travel. Trains aren't able to overtake an other
int travelTime(strain* train, int i, int j){
	int result = (rand()%3+1);//travel time E [1;3]
	if(matrix_lines[i][j].travel_time - time(0) > 0){//an other trains is on the line
		result += matrix_lines[i][j].travel_time - time(0);
		//if(result > 3) result = 3;
	}
	matrix_lines[i][j].travel_time = time(0)+result;//updating flag

	train->Ntrajet++;
	train->avg_travel_time += (double) result;
	train->avg_travel_time /= 2.0;

	return result;
}

//select the good sync system
void GetLine(int sync, int x, int y){
	switch(sync){
		case 0:GetLineMutex(x,y);break;
		case 1:GetLineSemaphore(x,y);break;
		case 2:GetLineRwlock(x,y);break;
		case 3:GetLineMq();break;
		default:break;
	}
}

//lock a line + wait free line + inc nb train on
void GetLineMutex(int i, int j){
	pthread_mutex_lock(&mutex_fifo);//on empêche tous les autres threads de travailler
	pthread_mutex_lock(&matrix_lines[i][j].mutex_line);//on lock le sens du trajet
	matrix_lines[i][j].nombre_train++;//inc le nombre trains occupant la voie
	if(matrix_lines[i][j].nombre_train == 1)pthread_mutex_lock(&matrix_lines[j][i].mutex_line);//on verrouille le sens opposé si on est le premier train
	pthread_mutex_unlock(&matrix_lines[i][j].mutex_line);//on unlock le sens du trajet
	pthread_mutex_unlock(&mutex_fifo);//on unlock la file d'attente pour laisser le prochain train sur la même ligne arriver
}

//lock a line using semaphore
void GetLineSemaphore(int i, int j){
	sem_wait(&sem_fifo);
	sem_wait(&matrix_lines[i][j].sem_line);
	matrix_lines[i][j].nombre_train++;
	if(matrix_lines[i][j].nombre_train == 1)sem_wait(&matrix_lines[j][i].sem_line);
	sem_post(&matrix_lines[i][j].sem_line);
	sem_post(&sem_fifo);
}

//lock a line using rwlock (writer)
void GetLineRwlock(int i, int j){
	pthread_rwlock_wrlock(&rwlock_fifo);
	pthread_rwlock_wrlock(&matrix_lines[i][j].rwlock_line);
	matrix_lines[i][j].nombre_train++;
	if(matrix_lines[i][j].nombre_train == 1)pthread_rwlock_wrlock(&matrix_lines[j][i].rwlock_line);

	//pthread_rwlock_unlock(&matrix_lines[i][j].rwlock_line); /!\ core dumbed if un commented with message "Operation not allowed" -> only 1 thread
	pthread_rwlock_unlock(&rwlock_fifo);
}

//unlock line by writting a message in mqd_fifo
void GetLineMq(){
	mq_send(mqd_fifo,"X", QUEUE_FIFO_MAX_MSG_LENGHT, 0);
}

//select the good sync system
void SignalUnusedLine(int sync, int x, int y){
	switch(sync){
		case 0:SignalUnusedLineMutex(x,y);break;
		case 1:SignalUnusedLineSemaphore(x,y);break;
		case 2:SignalUnusedLineRwlock(x,y);break;
		case 3:SignalUnusedLineMq();break;
		default:break;
	}
}

//unlock line when travel complete
void SignalUnusedLineMutex(int i, int j){
	pthread_mutex_lock(&matrix_lines[i][j].mutex_line);
	matrix_lines[i][j].nombre_train--;
	if(matrix_lines[i][j].nombre_train == 0)pthread_mutex_unlock(&matrix_lines[j][i].mutex_line);
	pthread_mutex_unlock(&matrix_lines[i][j].mutex_line);
}

//unlock line when travel complete
void SignalUnusedLineSemaphore(int i, int j){
	sem_wait(&matrix_lines[i][j].sem_line);
	matrix_lines[i][j].nombre_train--;
	if(matrix_lines[i][j].nombre_train == 0)sem_post(&matrix_lines[j][i].sem_line);
	sem_post(&matrix_lines[i][j].sem_line);
}

//unlock line when travel complete
void SignalUnusedLineRwlock(int i, int j){
	//pthread_rwlock_wrlock(&matrix_lines[i][j].rwlock_line); /!\ core dumbed if un commented with message "Operation not allowed" -> only 1 thread
	matrix_lines[i][j].nombre_train--;
	if(matrix_lines[i][j].nombre_train == 0)pthread_rwlock_unlock(&matrix_lines[j][i].rwlock_line);
	pthread_rwlock_unlock(&matrix_lines[i][j].rwlock_line);
}

//unlock line by reading a message in mqd_fifo
void SignalUnusedLineMq(){
	char buffer[1], bytes_read = mq_receive(mqd_fifo, buffer, QUEUE_FIFO_MAX_MSG_LENGHT, NULL);
}

//Free memory when ^C
void freeLines(int sig){
	if(sig == SIGINT){
		printf("\nfreeLines\n");
		pthread_mutex_destroy(&mutex_fifo);
		sem_destroy(&sem_fifo);
		pthread_rwlock_destroy(&rwlock_fifo);
		mq_close(mqd_fifo);
		pthread_barrier_destroy(&barrier);
		for(int i = 0; i < NB_LINES; i++){
			for(int j =0; j < NB_LINES; j++){
				//todo : free only the memery use
				pthread_mutex_destroy(&matrix_lines[i][j].mutex_line);
				sem_destroy(&matrix_lines[i][j].sem_line);
				pthread_rwlock_destroy(&matrix_lines[i][j].rwlock_line);
			}
		}
		exit(EXIT_SUCCESS);//end the program
	}
}

//convert ascii character to int index for matrix
int CvtCharToI(char carac){
	int result;
	switch(carac){
		case 'A':result = A;break;
		case 'B':result = B;break;
		case 'C':result = C;break;
		case 'D':result = D;break;
		case 'E':result = E;break;
		default:result = ERROR;
	}
	return result;
}

//thread execution
void* Move(void*data_train){

	// mqueue init
	mqd_t mq;
	struct mq_attr attr;
	char message[MAX_SIZE+1];

	attr.mq_flags = 0;
	attr.mq_maxmsg = 10;
	attr.mq_msgsize = MAX_SIZE;
	attr.mq_curmsgs = 0;


	// initialisation
	int N = strlen(((strain*)data_train)->trajet), x = ERROR, y = ERROR;
	printf("Train %d a pour trajet: %s de %d etapes\n",((strain*)data_train)->number,((strain*)data_train)->trajet,N);
	pthread_barrier_wait(&barrier);
	if( ((strain*)data_train)->number == 0){
		sleep(1);
		printf("\n");
	}
	pthread_barrier_wait(&barrier);
	// looping throug the path
	while(1){
		for(int i = 0; i < N; i++){
			if(((strain*)data_train)->trajet[i] != ((strain*)data_train)->trajet[(i+1)%N]){
				printf("Le train %d s`engage sur %c %c debug : %d %d\n",((strain*)data_train)->number,((strain*)data_train)->trajet[i],((strain*)data_train)->trajet[(i+1)%N],i,(i+1)%N);
				x = CvtCharToI(((strain*)data_train)->trajet[i]);
				y = CvtCharToI(((strain*)data_train)->trajet[(i+1)%N]);
				GetLine(((strain*)data_train)->sync,x,y);
				sleep(travelTime((strain*)data_train,x,y));
				SignalUnusedLine(((strain*)data_train)->sync,x,y);
				printf("Le train %d a atteint sa destination sur %c %c debug : %d %d\n",((strain*)data_train)->number,((strain*)data_train)->trajet[i],((strain*)data_train)->trajet[(i+1)%N],i,(i+1)%N);

				if (((strain*)data_train)->station) {
					char c = ((strain*)data_train)->trajet[(i+1)%N];
					if (c == 'A') {
						mq = mq_open("/station_a", O_CREAT | O_WRONLY, 0644, &attr);
					}
					else if (c == 'B') {
						mq = mq_open("/station_b", O_CREAT | O_WRONLY, 0644, &attr);
					}
					else if (c == 'C') {
						mq = mq_open("/station_c", O_CREAT | O_WRONLY, 0644, &attr);
					}
					else if (c == 'D') {
						mq = mq_open("/station_d", O_CREAT | O_WRONLY, 0644, &attr);
					}
					else if (c == 'E') {
						mq = mq_open("/station_e", O_CREAT | O_WRONLY, 0644, &attr);
					}

					assert((mqd_t)-1 != mq);
					snprintf(message, sizeof(message), "Le train %d en provenance de %c entre en gare %c",((strain*)data_train)->number,((strain*)data_train)->trajet[i],((strain*)data_train)->trajet[(i+1)%N]);
					assert(0 <= mq_send(mq, message, MAX_SIZE, 0));
					// assert(0 <= mq_send(mq, message, MAX_SIZE, 0));

				}
			}
		}
	}
	/* cleanup */
	assert(mq_close(mq) != (mqd_t)-1);
	return (NULL);
}
