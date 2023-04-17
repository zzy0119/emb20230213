/*
有5个工作线程，分别循环向标准输出疯狂打印'a', 'b', 'c', 'd', 'e'
要求整个进程执行5s（alarm(5)),进程的输出结果是abcdeabcdeabcde....
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#define THRNR	5

/*
	curid == 0 'a' 
	curid == 1 'b'
	......
 */
static int curid = -1;
static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

static void *thr_job(void *s);
int main(void)
{
	pthread_t tids[THRNR];	
	int i, err;

	alarm(5);

	for (i = 0; i < THRNR; i++) {
		err = pthread_create(tids + i, NULL, thr_job, (void *)i);
		if (err > 0) {
			fprintf(stderr, "pthread_create():%s\n", strerror(err));
			exit(1);
		}
	}

	// 控制应该打印输出的线程
	for (i = 0; ; i = (i + 1) % THRNR) {
		pthread_mutex_lock(&mut);
		while (curid != -1) {
			// 上一个线程还未打印
			pthread_cond_wait(&cond, &mut);
		}
		curid = i;
		pthread_cond_broadcast(&cond);
		pthread_mutex_unlock(&mut);
		sleep(1);
	}

	return 0;
}

static void *thr_job(void *s)
{
	int myid = (int)s;
	char c = 'a' + myid;

	while (1) {
		pthread_mutex_lock(&mut);
		while (myid != curid) {
			// 不该输出
			pthread_cond_wait(&cond, &mut);
		}
		write(1, &c , 1);	
		curid = -1; // main线程当前工作线程已经打印完成
		pthread_cond_broadcast(&cond);
		pthread_mutex_unlock(&mut);
	}
}



