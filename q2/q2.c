#include "company.h"
#include "global.h"
#include "stud.h"
#include "zone.h"

/**
 * thread of company
 * 
 * each zone when picking batch will keep hold of a mutex
 * 
 * make random no of batches
 * 
*/

int main() {
	srand(time(0));
	scanf("%d %d %d", &no_of_comp, &no_of_zone, &no_of_students);

	no_of_students_done = 0;
	no_of_students_waiting = 0;

	students = malloc(no_of_students * sizeof(student_t));
	zones = malloc(no_of_zone * sizeof(zone_t));
	companies = malloc(no_of_comp * sizeof(company_t));

	for (int i = 0; i < no_of_comp; i++)
		scanf("%f", &companies[i].prob);

	company_init();
	zones_init();
	stud_init();

	for (int i = 0; i < no_of_students; i++)
		pthread_join(students[i].tid, NULL);

	// for(int i=0; i<no_of_zone; i++)

	printf(RST_COL "\n\nSimulation Over.");
	fflush(0);
}
