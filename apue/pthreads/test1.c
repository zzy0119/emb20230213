/*
 100~300之间共201个整型数，如果判断一个整型数是否为质数需要1s
 现要求多线程并发，筛选201个整型变量是否为质数共用1s
 */
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>

#define START	100
#define END		300

#define THR_NR	((END) - (START) + 1)

static void *job_thread(void *s);
	
int main(void)
{
	pthread_t tids[THR_NR] = {};	
	int arr[THR_NR] = {};
	int err;
	int i, j;

	// 并发所有的工作线程	
	for (i = START, j = 0; i <= END; i++, j++) {
		// arr[j] = i;
		// err = pthread_create(tids + i - START, NULL, job_thread, arr + j);
		err = pthread_create(tids + i - START, NULL, job_thread, (void *)i);
		if (err) {
			fprintf(stderr, "pthread_create():%s\n", strerror(err));
			exit(1);
		}
	}

	// pause();
	// 等着收尸
	for (i = 0; i < THR_NR; i++) {
		pthread_join(tids[i], NULL);
	}

	return 0;
}

static int is_primer(int n);

static void *job_thread(void *s)
{
	// 判断得到的任务是否是质数
	sleep(1);
	// int n = *((int *)s);
	// printf("the new thread get n:%d\n", n);
	int n = (int)s;

	if (is_primer(n)) {
		printf("%d is a primer\n", n);
	}

	// pthread_exit(NULL);
	return NULL; // 从启动历程返回
}

static int is_primer(int n)
{
	int i;
	
	if (n < 2)
		return 0;

	for (i = 2; i <= n / 2; i++) {
		if (n % i == 0)		
			return 0;
	}
	return 1;
}



