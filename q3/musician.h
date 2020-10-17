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
	ts->tv_sec = impatience_limit;
	ts->tv_nsec = 0;
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
			pthread_mutex_lock(stages[i].mutex);
			//assuming musicians enter completely empty stage
			if (!stages[i].musician_performing && !stages[i].singer_performing) {
				stages[i].musician_performing = 1;
				pthread_mutex_unlock(&(stages[i].mutex));
				int perform_time = randInt(t1, t2);
				int singer_joined = 0;
				clock_t performance_st_time = clock();
				while (clock() - performance_st_time < perform_time * CLOCKS_PER_SEC) {
						if( !singer_joined  && stages[i].singer_performing]){
							singer_joined = 1;
							perform_time += 2;
						}
				}
				stages[i].musician_performing = 0;
				break;
			}
			pthread_mutex_unlock(&(stages[i].mutex));
		}
	} else {
		for (int i = 0; i < stg_cnt; i++) {
			pthread_mutex_lock(stages[i].mutex);
			if (!stages[i].singer_performing) {
				stages[i].singer_performing = 1;
				int musician_there = stages[i].musician_performing;
				pthread_mutex_unlock(&(stages[i].mutex));
				if (musician_there)
					while (stages[i].musician_performing)
						;
				else
					sleep(randInt(t1, t2));
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
