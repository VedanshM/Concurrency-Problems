#ifndef COMPANY_H
#define COMPANY_H
#include <stdlib.h>

#include "global.h"

void company_init() {
	for (int i = 0; i < no_of_comp; i++) {
		companies[i].id = i + 1;
		companies[i].batch_cnt = 0;
		companies[i].batches = NULL;
		pthread_mutex_init(&companies[i].mutex, NULL);
		pthread_create(&(companies[i].tid), NULL, company_thread, &companies[i]);
	}
}

void *company_thread(void *arg) {
	company_t *cmp = (company_t *)arg;
	while (no_of_students_done < no_of_students) {
		cmp->batch_cnt = randInt(1, 5);
		cmp->batches = (batch_t *)malloc(cmp->batch_cnt * sizeof(batch_t));
		int cap = randInt(10, 20);
		for (int i = 0; i < cmp->batch_cnt; i++)
			cmp->batches[i].capacity = cmp->batches[i].vacc_left = cap;
		printf(YLW_COL "Company %d is preparing %d batches of vaccines with success rate: %.2f\n" RST_COL,
			   cmp->id, cmp->batch_cnt, cmp->prob);
		fflush(0);
		sleep(randInt(2, 5));
		printf(YLW_COL "Company %d has prepared %d batches of vaccines with success rate: %.2f.\n"
					   "Waiting for all the vaccines to be used to resume production\n\n" RST_COL,
			   cmp->id, cmp->batch_cnt, cmp->prob);
		fflush(0);
		distribute_batches(cmp);
		while (1) {
			int anyleft = 0;
			for (int i = 0; i < cmp->batch_cnt; i++) {
				pthread_mutex_lock(&cmp->mutex);
				if (cmp->batches[i].vacc_left > 0) {
					anyleft++;
				}
				pthread_mutex_unlock(&cmp->mutex);
			}
			if (!anyleft)
				break;
		}
		free(cmp->batches);
		printf(YLW_COL "All the vaccines prepared by Pharmaceutical Company %d are emptied. Resuming production now.\n" RST_COL,
			   cmp->id);
		fflush(0);
	}
}

int distribute_batches(company_t *cmp) {
	int batches_left = cmp->batch_cnt;
	while (no_of_students_done < no_of_students && batches_left > 0) {
		for (int i = 0; i < no_of_zone; i++) {
			pthread_mutex_lock(&zones[i].mutex);
			if (zones[i].batch == NULL) {
				printf(YLW_COL "Company %d is delivering a vaccine batch to Zone %d which has success probability %.2f\n" RST_COL,
					   cmp->id, zones[i].id, cmp->prob);
				fflush(0);
				zones[i].batch = &cmp->batches[cmp->batch_cnt - batches_left];
				zones[i].comp = cmp;
				pthread_mutex_unlock(&zones[i].mutex);
				batches_left--;
				break;
			}
			pthread_mutex_unlock(&zones[i].mutex);
		}
	}
}

#endif
