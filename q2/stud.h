#ifndef STUDENT_H
#define STUDENT_H
#include "global.h"

void stud_init() {
	for (int i = 0; i < no_of_students; i++) {
		students[i].status = 0;
		students[i].no_of_trials = 0;
		students[i].id = i + 1;
		pthread_create(&(students[i].tid), NULL, stud_thread, students + i);
	}
}

void *stud_thread(void *arg) {
	student_t *std = (student_t *)arg;
	while (std->no_of_trials < 3) {
		std->no_of_trials++;
		pthread_mutex_lock(&global_mutex);
		no_of_students_waiting++;
		pthread_mutex_unlock(&global_mutex);

		printf(GRN_COL "STUDENT %d has arrived for round %d of vaccination\n" RST_COL, std->id, std->no_of_trials);
		fflush(0);
		float pr = get_vaccine(std);
		int success = check_vaccine(pr);
		if (success) {
			printf(CYN_COL "Student %d has tested positive for antibodies.\n" RST_COL,
				   std->id);
			fflush(0);
			pthread_mutex_lock(&global_mutex);
			no_of_students_done++;
			pthread_mutex_unlock(&global_mutex);
			std->status = 1;
			break;
		} else {
			printf(GRN_COL "Student %d has tested " RED_COL "negative" GRN_COL " for antibodies.\n" RST_COL, std->id);
			fflush(0);
			if (std->no_of_trials == 3) {
				printf(RED_COL "Student %d is HOPELESS, going back to home.\n" RST_COL, std->id);
				fflush(0);
				pthread_mutex_lock(&global_mutex);
				no_of_students_done++;
				pthread_mutex_unlock(&global_mutex);
				std->status = -1;
			}
		}
	}
}

float get_vaccine(student_t *std) {
	printf(GRN_COL "STUDENT %d is waiting to be allocated a slot on a Zone\n" RST_COL, std->id);
	fflush(0);

	while (1) {
		for (int i = 0; i < no_of_zone; i++) {
			if (pthread_mutex_trylock(&zones[i].mutex) != 0) {
				continue;
			}
			int got_slot = 0;
			if (zones[i].phase = SEARCHING_STUDS && zones[i].slots_filled < zones[i].slots) {
				zones[i].slots_filled++;
				got_slot = 1;
			}
			pthread_mutex_unlock(&zones[i].mutex);
			if (got_slot) {
				pthread_mutex_lock(&global_mutex);
				no_of_students_waiting--;
				pthread_mutex_unlock(&global_mutex);
				printf(GRN_COL "Student %d assigned a slot on the Zone %d and waiting to be vaccinated\n", std->id, zones[i].id);
				fflush(0);
				while (!zones[i].studd_can_go)
					;
				printf(GRN_COL "Student %d on Zone %d has been vaccinated which has success probability %.2f\n" RST_COL, std->id, zones[i].id, zones[i].comp->prob);
				fflush(0);
				return zones[i].comp->prob;
			}
		}
	}
	return 0;
}
int check_vaccine(float pr) {
	float x = randInt(0, 1000);
	x /= 1000;
	if (x <= pr)
		return 1;
	return 0;
}

#endif
