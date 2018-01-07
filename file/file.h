#ifndef __FILE_H__ // permet d'eviter une double inclusion
#define __FILE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_NAME "values.txt"

#define ERROR -1

#ifdef __cplusplus
	extern "C" {
#endif

int writeSeparator();
int writeParameter(int seed, int sync, int station);
int writeResult(int thread_id, int Ntrajet, double avg_travel_time);

#ifdef __cplusplus
	}
#endif

#endif
