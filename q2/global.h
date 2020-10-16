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

pthread_mutex_t global_mutex = PTHREAD_MUTEX_INITIALIZER;
int no_of_comp, no_of_zone, no_of_students;
int no_of_students_done = 0;
int no_of_students_waiting = 0;

int randInt(int a, int b) { //generate random int in [a,b]
	return a + rand() % (b - a + 1);
}

int min(int a, int b) {
	return a < b ? a : b;
}

int max(int a, int b) {
	return a > b ? a : b;
}

typedef struct batch {
	int capacity;
	int vacc_left;
} batch_t;

typedef struct company {
	int id;
	int batch_cnt;
	batch_t *batches;
	float prob;
	pthread_t tid;
	pthread_mutex_t mutex;
} company_t;

typedef struct student {
	int id;
	int no_of_trials;
	int status;
	pthread_t tid;
} student_t;

typedef struct zone {
	int id;
	int phase;
	int slots;
	int slots_filled;
	int studd_can_go;
	batch_t *batch;
	company_t *comp;
	pthread_t tid;
	pthread_mutex_t mutex;
} zone_t;

company_t *companies;
zone_t *zones;
student_t *students;

void company_init();
void *company_thread(void *arg);

void zones_init();
void *zone_thread(void *);

void stud_init();
void *stud_thread(void *);

int distribute_batches(company_t *cmp);
float get_vaccine(student_t *std);
int check_vaccine(float);

#define WAITING_FOR_BATCH 0
#define SEARCHING_STUDS 1
#define VACCINATING_STUDS 2

//   Colour escape codes
#define BLK_COL "\033[30;1m"
#define RED_COL "\033[31;1m"
#define GRN_COL "\033[32;1m"
#define YLW_COL "\033[33;1m"
#define BLU_COL "\033[34;1m"
#define MGN_COL "\033[35;1m"
#define CYN_COL "\033[36;1m"
#define WHT_COL "\033[37;1m"
#define RST_COL "\033[0m"

#endif
