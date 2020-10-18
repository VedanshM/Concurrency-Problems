#include "global.h"
#include "musician.h"

int main() {
	srand(time(0));
	scanf("%d %d %d %d %d %d %d",
		  &musician_cnt, &acoustic_stg_cnt, &electric_stg_cnt, &coord_cnt, &t1, &t2, &impatience_limit);

	ac_stages = malloc(acoustic_stg_cnt * sizeof(stage_t));
	ec_stages = malloc(electric_stg_cnt * sizeof(stage_t));
	sem_init(&ac_sem, 0, acoustic_stg_cnt);
	sem_init(&ec_sem, 0, electric_stg_cnt);
	sem_init(&both_sem, 0, acoustic_stg_cnt + electric_stg_cnt);
	sem_init(&singer_sem, 0, acoustic_stg_cnt + electric_stg_cnt);
	int stg_id = 0;
	for (int i = 0; i < acoustic_stg_cnt; i++) {
		ac_stages[i].id = ++stg_id;
		ac_stages[i].musician_performing = NULL;
		ac_stages[i].singer_performing = NULL;
		ac_stages[i].type = ACOUSTIC_STAGE;
		pthread_mutex_init(&ac_stages[i].mutex, NULL);
	}
	for (int i = 0; i < electric_stg_cnt; i++) {
		ec_stages[i].id = ++stg_id;
		ec_stages[i].musician_performing = NULL;
		ec_stages[i].singer_performing = NULL;
		ec_stages[i].type = ELECTRIC_STAGE;
		pthread_mutex_init(&ec_stages[i].mutex, NULL);
	}
	musicians_init();
	for (int i = 0; i < musician_cnt; i++)
		pthread_join(musicians[i].tid, NULL);

	printf(RST_COL "\n\n FINISHED.\n");
}
