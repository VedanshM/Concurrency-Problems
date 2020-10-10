#define _POSIX_C_SOURCE 199309L //required for clock
#include <fcntl.h>
#include <inttypes.h>
#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <wait.h>

const long long BILLION = 1e9;

void swap(int *a, int *b) {
	int t = *a;
	*a = *b;
	*b = t;
}

int *get_sharemem(size_t size) {
	key_t mem_key = IPC_PRIVATE;
	int shm_id = shmget(mem_key, size, IPC_CREAT | 0666);
	return shmat(shm_id, NULL, 0);
}

void normal_selc_sort(int *a, int n) {
	for (int i = 0; i < n - 1; i++) {
		int min_idx = i;
		for (int j = i + 1; j < n; j++)
			if (a[min_idx] > a[j])
				min_idx = j;

		swap(&a[i], &a[min_idx]);
	}
}

void merge(int *a, int na, int *b, int nb) {
	int c[na + nb];
	int i = 0, j = 0, k = 0;
	while (i < na && j < nb)
		c[k++] = (a[i] < b[j]) ? a[i++] : b[j++];
	while (i < na)
		c[k++] = a[i++];
	while (j < nb)
		c[k++] = b[j++];
	for (i = 0; i < k; i++)
		a[i] = c[i];
}

void normal_merge_sort(int *a, int n) {
	if (n < 5) {
		return normal_selc_sort(a, n);
	}
	int n1 = n / 2, n2 = n - n / 2;
	int *b = a + n1;

	normal_merge_sort(a, n1);
	normal_merge_sort(a + n1, n2);
	merge(a, n1, a + n1, n2);
}

void forked_merge_sort(int *a, int n) {
	if (n < 5) {
		return normal_selc_sort(a, n);
	}
	int n1 = n / 2, n2 = n - n / 2;
	int *b = a + n1;

	pid_t pid1 = fork();
	if (pid1 < 0) {
		perror("error in forking");
		return;
	} else if (pid1 == 0) {
		forked_merge_sort(a, n1);
		exit(0);
	} else {
		pid_t pid2 = fork();
		if (pid2 == 0) {
			forked_merge_sort(b, n2);
			exit(0);
		} else {
			waitpid(pid1, NULL, WUNTRACED);
			waitpid(pid2, NULL, WUNTRACED);
		}
	}

	merge(a, n1, b, n2);
}

typedef struct args {
	int *arr;
	int n;
} args;

void *thrd_merge_sort(void *ptr) {
	args *arg = ptr;
	if (arg->n < 5) {
		normal_selc_sort(arg->arr, arg->n);
		return NULL;
	}

	args part1 = {.arr = arg->arr, .n = arg->n / 2};
	args part2 = {.arr = arg->arr + part1.n, .n = arg->n - part1.n};
	pthread_t tid1, tid2;
	pthread_create(&tid1, NULL, thrd_merge_sort, (void *)(&part1));
	pthread_create(&tid2, NULL, thrd_merge_sort, (void *)(&part2));
	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);

	merge(part1.arr, part1.n, part2.arr, part2.n);
}

int main() {
	struct timespec start_tm, end_tm;
	long long norm_dur, forked_dur, thrd_dur;
	long long int n;
	scanf("%lld", &n);
	int org_arr[n];
	for (int i = 0; i < n; i++) {
		scanf("%d", org_arr + i);
	}
	int arr[n];
	memcpy(arr, org_arr, sizeof(org_arr[0]) * n);
	clock_gettime(CLOCK_MONOTONIC_RAW, &start_tm);
	normal_merge_sort(arr, n);
	clock_gettime(CLOCK_MONOTONIC_RAW, &end_tm);
	norm_dur = (end_tm.tv_sec - start_tm.tv_sec) * BILLION + end_tm.tv_nsec - start_tm.tv_nsec;

	printf("Time taken by normal merge sort: %lld ns \nSorted array:", norm_dur);
	for (int i = 0; i < n; i++) {
		printf("%d ", arr[i]);
	}

	printf("\n");

	int *f_arr = get_sharemem(n * sizeof(int));
	memcpy(f_arr, org_arr, sizeof(org_arr[0]) * n);

	clock_gettime(CLOCK_MONOTONIC_RAW, &start_tm);
	forked_merge_sort(f_arr, n);
	clock_gettime(CLOCK_MONOTONIC_RAW, &end_tm);
	forked_dur = (end_tm.tv_sec - start_tm.tv_sec) * BILLION + end_tm.tv_nsec - start_tm.tv_nsec;

	printf("Time taken by multi process concurrent merge sort: %lld ns \nSorted array:", forked_dur);
	for (int i = 0; i < n; i++) {
		printf("%d ", f_arr[i]);
	}

	printf("\n");

	int t_arr[n];
	memcpy(t_arr, org_arr, sizeof(org_arr[0]) * n);
	args arg = {.arr = t_arr, .n = n};

	clock_gettime(CLOCK_MONOTONIC_RAW, &start_tm);
	thrd_merge_sort(&arg);
	clock_gettime(CLOCK_MONOTONIC_RAW, &end_tm);
	thrd_dur = (end_tm.tv_sec - start_tm.tv_sec) * BILLION + end_tm.tv_nsec - start_tm.tv_nsec;

	printf("Time taken by multi thread concurrent merge sort: %lld ns \nSorted array:", thrd_dur);
	for (int i = 0; i < n; i++) {
		printf("%d ", t_arr[i]);
	}
	printf("\n\n\n");

	printf("Normal mergesort ran"
		   " \n\t[%.2Lf] times faster than multi process concurrent merge sort and "
		   " \n\t[%.2Lf] times faster than multi threaded concurrent merge sort.\n",
		   (long double)(forked_dur) / norm_dur, (long double)(thrd_dur) / norm_dur);
}
