#ifndef GLOBAL_H
#define GLOBAL_H

#include <inttypes.h>
#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <wait.h>

int no_of_comp, no_of_zone, no_of_students;
float *success_rate;

int randInt(int a, int b) { //generate random int in [a,b]
	return a + rand() % (b - a + 1);
}

#endif
