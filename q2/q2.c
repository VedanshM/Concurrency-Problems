#include "global.h"

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
	scanf("%d %d %d", no_of_comp, no_of_zone, no_of_students);
	success_rate = malloc(sizeof(float) * no_of_comp);
	for (int i = 0; i < no_of_comp; i++)
		scanf("%f", &success_rate[i]);

	free(success_rate);
}
