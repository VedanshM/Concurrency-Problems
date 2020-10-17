#ifndef MUSICIAN_H
#define MUSICIAN_H

#include "global.h"

void musicians_init(int n) {
	musicians = (musician_t *)malloc(n * sizeof(musician_t));
	for (int i = 0; i < n; i++) {
		char c;
		scanf("%s %c %d", musicians[i].name, c, musicians[i].arrival_time);
		switch (c) {
		case 's':
			musicians[i].instr = SINGER;
			break;
		case 'p':
			musicians[i].instr = PIANO;
			break;
		case 'g':
			musicians[i].instr = GUITAR;
			break;
		case 'v':
			musicians[i].instr = VOILIN;
			break;
		case 'b':
			musicians[i].instr = BASS;
			break;

		default:
			printf("wrong instrument . setting musician to singer");
			musicians[i].instr = SINGER;
			break;
		}
		musicians[i].state = NOT_ARRIVED;
	}
	for (int i = 0; i < n; i++)
		pthread_create(&(musicians[i].tid), NULL, musician_thread, musicians + i);
}

void *musician_thread(void *ptr) {
	musician_t *msc = (musician_t *)ptr;
	sleep(msc->arrival_time);

	int performed = 0;
	struct timespec ts;
	ts.tv_nsec = 0;
	ts.tv_sec = impatience_limit;
	if (msc->stage_type == ELECTRIC_STAGE) {
		if (sem_timedwait(&ec_sem, &ts) == 0) { //waiting for a EC stage  to get free
												//one of the stage has to be empty is sem allowed
			perform_on_type(msc, ec_stages, electric_stg_cnt);
			sem_post(&ec_sem);
			performed = 1;
		}
	} else if (msc->stage_type == ACOUSTIC_STAGE) {
		if (sem_timedwait(&ac_sem, &ts) == 0) { //waiting for a AC stage  to get free
												//one of the stage has to be empty is sem allowed
			perform_on_type(msc, ac_stages, acoustic_stg_cnt);
			sem_post(&ac_sem);
			performed = 1;
		}
	} else if (msc->stage_type == BOTH_STAGE) {
		if (sem_timedwait(&both_sem, &ts) == 0) { //waiting for any stage  to get free
												  //one of the stage has to be empty is sem allowed
												  //not necessary to search AC first
			if (perform_on_type(msc, ac_stages, acoustic_stg_cnt) == -1)
				perform_on_type(msc, ec_stages, electric_stg_cnt);
			sem_post(&both_sem);
			performed = 1;
		}
	}

	if (!performed) {
		printf(RED_COL "%s %s leaving because of impatience !!\n" RST_COL, msc->name, instr_name[msc->instr]);
		return NULL;
	}
	// collect tshirt
}

int perform_on_type(musician_t *msc, stage_t *stages, int stg_cnt) {
	int i;
	if (msc->instr != SINGER) {
		for (i = 0; i < stg_cnt; i++) {
			pthread_mutex_lock(&stages[i].mutex);
			//assuming musicians enter completely empty stage
			if (!stages[i].musician_performing && !stages[i].singer_performing) {
				stages[i].musician_performing = 1;
				clock_gettime(CLOCK_MONOTONIC_RAW, &stages[i].performance_endtime);
				int perform_time = randInt(t1, t2);
				stages[i].performance_endtime.tv_nsec += perform_time * 1e9;
				pthread_mutex_unlock(&(stages[i].mutex));

				sleep(perform_time);
				if (stages[i].singer_performing) {
					struct timespec ts;
					clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
					ts.tv_nsec -= stages[i].performance_endtime.tv_nsec;
					if (ts.tv_nsec < 0) {
						ts.tv_nsec += 1e9;
						ts.tv_sec -= 1;
					}
					ts.tv_sec -= stages[i].performance_endtime.tv_sec;
					nanosleep(&ts, NULL);
				}
				stages[i].musician_performing = 0;
				break;
			}
			pthread_mutex_unlock(&(stages[i].mutex));
		}
	} else {
		for (int i = 0; i < stg_cnt; i++) {
			pthread_mutex_lock(&stages[i].mutex);
			if (!stages[i].singer_performing) {
				stages[i].singer_performing = 1;
				if (stages[i].musician_performing) {
					stages[i].performance_endtime.tv_sec += 2;
				} else {
					clock_gettime(CLOCK_MONOTONIC_RAW, &stages[i].performance_endtime);
					stages[i].performance_endtime.tv_sec += randInt(t1, t2);
				}
				pthread_mutex_unlock(&(stages[i].mutex));

				struct timespec ts;
				clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
				ts.tv_nsec -= stages[i].performance_endtime.tv_nsec;
				if (ts.tv_nsec < 0) {
					ts.tv_nsec += 1e9;
					ts.tv_sec -= 1;
				}
				ts.tv_sec -= stages[i].performance_endtime.tv_sec;
				nanosleep(&ts, NULL);

				stages[i].singer_performing = 0;
			}
			pthread_mutex_unlock(&(stages[i].mutex));
		}
	}
	if (i == stg_cnt)
		return -1;
	return i;
}

#endif
