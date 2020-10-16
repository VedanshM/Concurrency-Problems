#ifndef ZONE_H
#define ZONE_H
#include "global.h"

void zones_init() {
	for (int i = 0; i < no_of_zone; i++) {
		zones[i].id = i + 1;
		zones[i].slots = 0;
		zones[i].slots_filled = 0;
		zones[i].comp = NULL;
		zones[i].batch = NULL;
		zones[i].studd_can_go = 0;
		pthread_mutex_init(&zones[i].mutex, NULL);
		pthread_create(&zones[i].tid, NULL, zone_thread, zones + i);
	}
}

void *zone_thread(void *arg) {
	zone_t *zn = (zone_t *)arg;
	while (no_of_students_done < no_of_students) {
		// WAITING_FOR_BATCH
		while (1) {
			pthread_mutex_lock(&zn->mutex);
			zn->phase = WAITING_FOR_BATCH;
			if (zn->batch) {
				pthread_mutex_unlock(&zn->mutex);
				break;
			}
			pthread_mutex_unlock(&zn->mutex);
		}

		printf(BLU_COL "Company %d has delivered vaccines to Zone %d,"
					   " resuming vaccinations now.\n" RST_COL,
			   zn->comp->id, zn->id);
		fflush(0);

		while (1) { // till vacc is left
			pthread_mutex_lock(&zn->mutex);
			zn->slots = randInt(1, min(8, zn->batch->vacc_left));
			zn->slots_filled = 0;
			pthread_mutex_unlock(&zn->mutex);

			//waiting for students to acquire slots
			printf(BLU_COL "Zone %d is ready to vaccinate with %d slots\n" RST_COL, zn->id, zn->slots);
			fflush(0);
			zn->studd_can_go = 0;
			zn->phase = SEARCHING_STUDS;
			while (1) {
				pthread_mutex_lock(&zn->mutex);
				pthread_mutex_lock(&global_mutex);
				if (zn->slots_filled == zn->slots || (no_of_students_waiting == 0 && zn->slots_filled >= 1)) {
					pthread_mutex_unlock(&global_mutex);
					zn->phase = VACCINATING_STUDS;
					pthread_mutex_unlock(&zn->mutex);
					break;
				}
				pthread_mutex_unlock(&global_mutex);
				pthread_mutex_unlock(&zn->mutex);
			}

			//vaccinating
			printf(BLU_COL "Zone %d entering Vaccination Phase\n", zn->id);
			fflush(0);
			sleep(2);
			zn->studd_can_go = 1;
			//vacc done

			/**no one will access var:slots_filled while
			 * below code is running because in vacc phase
			*/

			pthread_mutex_lock(&zn->comp->mutex);
			zn->batch->vacc_left -= zn->slots_filled;
			if (zn->batch->vacc_left == 0) {
				zn->batch = NULL; // free this in compnay
				pthread_mutex_unlock(&zn->comp->mutex);
				printf(BLU_COL "Zone %d has run out of vaccines", zn->id);
				fflush(0);
				break;
			}
			pthread_mutex_unlock(&zn->comp->mutex);
		}
	}
}

#endif
