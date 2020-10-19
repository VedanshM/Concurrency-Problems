#ifndef MUSICIAN_H
#define MUSICIAN_H

#include "global.h"

void musicians_init() {
	musicians = (musician_t *)malloc(musician_cnt * sizeof(musician_t));
	for (int i = 0; i < musician_cnt; i++) {
		char c;
		scanf("%s %c %d", musicians[i].name, &c, &musicians[i].arrival_time);
		switch (c) {
		case 's':
			musicians[i].instr = SINGER;
			musicians[i].stage_type = BOTH_STAGE;
			break;
		case 'p':
			musicians[i].instr = PIANO;
			musicians[i].stage_type = BOTH_STAGE;
			break;
		case 'g':
			musicians[i].instr = GUITAR;
			musicians[i].stage_type = BOTH_STAGE;
			break;
		case 'v':
			musicians[i].instr = VOILIN;
			musicians[i].stage_type = ACOUSTIC_STAGE;
			break;
		case 'b':
			musicians[i].instr = BASS;
			musicians[i].stage_type = ELECTRIC_STAGE;
			break;

		default:
			printf("wrong instrument . setting musician to singer");
			musicians[i].instr = SINGER;
			break;
		}
	}
	for (int i = 0; i < musician_cnt; i++)
		pthread_create(&(musicians[i].tid), NULL, musician_thread, musicians + i);
}

void *musician_thread(void *ptr) {
	musician_t *msc = (musician_t *)ptr;
	sleep(msc->arrival_time);
	printf(MGN_COL "%s (%s) arrived.\n" RST_COL, msc->name, instr_name[msc->instr]);
	fflush(0);
	int performed = 0;
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	ts.tv_sec += impatience_limit;
	if (msc->instr == SINGER) {
		if (sem_timedwait(&singer_sem, &ts) == 0) { //waiting for any stage  to get free
													//one of the stage has to be empty is sem allowed
													//not necessary to search AC first
			if (perform_on_type(msc, ac_stages, acoustic_stg_cnt) == -1)
				perform_on_type(msc, ec_stages, electric_stg_cnt);
			sem_post(&singer_sem);
			performed = 1;
		}

	} else if (msc->stage_type == ELECTRIC_STAGE) {
		if (sem_timedwait(&ec_sem, &ts) == 0) { //waiting for a EC stage  to get free
												//one of the stage has to be empty is sem allowed
			sem_wait(&both_sem);
			perform_on_type(msc, ec_stages, electric_stg_cnt);
			sem_post(&ec_sem);
			sem_post(&both_sem);
			performed = 1;
		}
	} else if (msc->stage_type == ACOUSTIC_STAGE) {
		if (sem_timedwait(&ac_sem, &ts) == 0) { //waiting for a AC stage  to get free
												//one of the stage has to be empty is sem allowed
			sem_wait(&both_sem);
			perform_on_type(msc, ac_stages, acoustic_stg_cnt);
			sem_post(&ac_sem);
			sem_post(&both_sem);
			performed = 1;
		}
	} else if (msc->stage_type == BOTH_STAGE) {
		if (sem_timedwait(&both_sem, &ts) == 0) { //waiting for any stage  to get free
												  //one of the stage has to be empty is sem allowed
												  //not necessary to search AC first
												  //if i am correct(and i am) either of ac or ec should be free
			if (sem_trywait(&ac_sem) == 0) {
				perform_on_type(msc, ac_stages, acoustic_stg_cnt);
				sem_post(&ac_sem);
			} else {
				sem_wait(&ec_sem);
				perform_on_type(msc, ec_stages, electric_stg_cnt);
				sem_post(&ec_sem);
			}
			sem_post(&both_sem);
			performed = 1;
		}
	}
	// printf("DEBUG: name:%s performed: %d\n", msc->name, performed);
	if (!performed) {
		printf(RED_COL "%s (%s) leaving because of impatience !!\n" RST_COL, msc->name, instr_name[msc->instr]);
		fflush(0);
		return NULL;
	}
	ts.tv_sec = 0;
	ts.tv_nsec = 5;
	nanosleep(&ts, NULL);

	// collecting tshirt

	if (coord_cnt <= 0) {
		printf("No Coordinator to give Tshirts!! %s is leaving sad.\n", msc->name);
		fflush(0);
		return NULL;
	}

	sem_wait(&coord_sem);
	printf(CYN_COL "%s (%s) collecting Tshirt.\n" RST_COL,
		   msc->name, instr_name[msc->instr]);
	fflush(0);
	sleep(2);
	sem_post(&coord_sem);

	return NULL;
}

int perform_on_type(musician_t *msc, stage_t *stages, int stg_cnt) {
	int i;
	if (msc->instr != SINGER) {
		for (i = 0; i < stg_cnt; i++) {
			pthread_mutex_lock(&stages[i].mutex);
			//assuming musicians enter completely empty stage
			if (!stages[i].musician_performing && !stages[i].singer_performing) {
				stages[i].musician_performing = msc;
				int perform_time = randInt(t1, t2);
				clock_gettime(CLOCK_REALTIME, &stages[i].performance_endtime);
				stages[i].performance_endtime.tv_sec += perform_time;
				pthread_mutex_unlock(&(stages[i].mutex));
				printf(YLW_COL "%s performing %s at stage :%d (%s) for %d sec.\n" RST_COL,
					   msc->name, instr_name[msc->instr], stages[i].id,
					   (stages[i].type == ACOUSTIC_STAGE) ? "acoustic" : "electric", perform_time);
				fflush(0);

				sleep(perform_time);
				if (stages[i].singer_performing) {
					struct timespec ts;
					clock_gettime(CLOCK_REALTIME, &ts);
					ts.tv_nsec = stages[i].performance_endtime.tv_nsec - ts.tv_nsec;
					ts.tv_sec = stages[i].performance_endtime.tv_sec - ts.tv_sec;
					if (ts.tv_nsec < 0) {
						ts.tv_nsec += 1e9;
						ts.tv_sec -= 1;
					}
					nanosleep(&ts, NULL);
				}
				stages[i].musician_performing = NULL;
				break;
			}
			pthread_mutex_unlock(&(stages[i].mutex));
		}
	} else {
		for (i = 0; i < stg_cnt; i++) {
			pthread_mutex_lock(&stages[i].mutex);
			if (!stages[i].singer_performing) {
				stages[i].singer_performing = msc;
				int solo = 0;
				if (stages[i].musician_performing) {
					stages[i].performance_endtime.tv_sec += 2;
					printf(YLW_COL "%s joined %s's performance, performance extended by 2 sec.\n" RST_COL,
						   msc->name, stages[i].musician_performing->name);
				} else {
					solo = 1;
					sem_wait(&both_sem);
					if (stages[i].type == ACOUSTIC_STAGE)
						sem_wait(&ac_sem);
					else
						sem_wait(&ec_sem);
					clock_gettime(CLOCK_REALTIME, &stages[i].performance_endtime);
					int perform_time = randInt(t1, t2);
					stages[i].performance_endtime.tv_sec += perform_time;
					printf(YLW_COL "%s singer started solo performance at stage: %d (%s) for %d sec.\n" RST_COL,
						   msc->name, stages[i].id, stages[i].type == ACOUSTIC_STAGE ? "acoustic" : "electric",
						   perform_time);
				}
				pthread_mutex_unlock(&(stages[i].mutex));
				fflush(0);
				struct timespec ts;
				clock_gettime(CLOCK_REALTIME, &ts);
				ts.tv_nsec = stages[i].performance_endtime.tv_nsec - ts.tv_nsec;
				ts.tv_sec = stages[i].performance_endtime.tv_sec - ts.tv_sec;
				if (ts.tv_nsec < 0) {
					ts.tv_nsec += 1e9;
					ts.tv_sec -= 1;
				}
				// printf("DEBUG: %ld %ld\n\n", ts.tv_sec, ts.tv_nsec);
				nanosleep(&ts, NULL);

				stages[i].singer_performing = NULL;
				if (solo) {
					sem_post(&both_sem);
					if (stages[i].type == ACOUSTIC_STAGE)
						sem_post(&ac_sem);
					else
						sem_post(&ec_sem);
				}
				break;
			}
			pthread_mutex_unlock(&(stages[i].mutex));
		}
	}
	if (i == stg_cnt)
		return -1;
	printf(GRN_COL "%s performance ends on stage:%d (%s).\n" RST_COL,
		   msc->name, stages[i].id, stages[i].type == ACOUSTIC_STAGE ? "acoustic" : "electric");
	fflush(0);
	return i;
}

#endif
