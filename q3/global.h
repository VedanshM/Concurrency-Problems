#ifndef GLOBAL_H
#define GLOBAL_H

#define _POSIX_C_SOURCE 200112L
#include <fcntl.h>
#include <inttypes.h>
#include <limits.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <wait.h>

#define ELECTRIC_STAGE 0
#define ACOUSTIC_STAGE 1
#define BOTH_STAGE 2

typedef enum {
	NOT_ARRIVED,
	WAITING_TO_PERFORM,
	PERFORMING_SOLO,
	PERFORMING_W_SINGER,
	WAITING_FOR_TS,
	COLLECTING_TS,
	EXITED,
} musician_state_t;

typedef enum {
	SINGER,
	PIANO,
	GUITAR,
	VOILIN,
	BASS,
} instrument_t;

char *instr_name[5] = {
	"SINGER", "PIANO", "GUITAR", "VOILIN", "BASS"};

typedef struct musician {
	char name[1024];
	musician_state_t state;
	instrument_t instr;
	int stage_type;
	int arrival_time;
	pthread_t tid;
} musician_t;

typedef struct stage {
	int id;
	int type;
	musician_t *musician_performing;
	musician_t *singer_performing;
	struct timespec performance_endtime;
	pthread_mutex_t mutex;
} stage_t;

pthread_mutex_t global_mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t ac_sem, ec_sem, both_sem, singer_sem;
int musician_cnt, acoustic_stg_cnt, electric_stg_cnt, coord_cnt;
int impatience_limit, t1, t2;
stage_t *ac_stages, *ec_stages;

musician_t *musicians;

int randInt(int a, int b) { //generate random int in [a,b]
	return a + rand() % (b - a + 1);
}

int min(int a, int b) {
	return a < b ? a : b;
}

int max(int a, int b) {
	return a > b ? a : b;
}

void musicians_init();
void *musician_thread(void *);

int perform_on_type(musician_t *msc, stage_t *stages, int stg_cnt);

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
