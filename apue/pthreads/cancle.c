/*
有5个工作线程，分别循环向标准输出疯狂打印'a', 'b', 'c', 'd', 'e'
要求整个进程执行5s（alarm(5)),进程的输出结果是abcdeabcdeabcde....
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#define THRNR	5

/*
	curid == 0 'a' 
	curid == 1 'b'
	......
 */
static int curid = -1;
static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
static pthread_t tids[THRNR] = {0};	

static void *thr_job(void *s);

// 闹钟时间到了将调用的函数
static void exit_process(int s)
{
	int i;

	pthread_mutex_unlock(&mut);

	// 向所用的工作线程发送取消请求
	for (i = 0; i < THRNR; i++) {
		pthread_cancel(tids[i]);
	}

	// 收尸
	for (i = 0; i < THRNR; i++)
		pthread_join(tids[i], NULL);

	exit(0);
}

int main(void)
{
	int i, err;
	struct sigaction act;

	act.sa_handler = exit_process;
	sigaction(SIGALRM, &act, NULL);
	alarm(1);

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
	}

	return 0;
}

static void thr_clean(void *s)
{
	write(1, "unlock", 6);
	// 解锁
	pthread_mutex_unlock(&mut);
}

static void *thr_job(void *s)
{
	int myid = (int)s;
	char c = 'a' + myid;

	// 注册终止助理函数
	pthread_cleanup_push(thr_clean, NULL);

	// 异步取消:随时可以响应取消
	int err = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	if (err) {
		fprintf(stderr, "pthread_setcanceltype():%s\n", strerror(err));
	}


	while (1) {
		pthread_mutex_lock(&mut);
		write(1, "lock", 4);
		while (myid != curid) {
			// 不该输出
			pthread_cond_wait(&cond, &mut);
		}
		write(1, "wait", 4);
		//write(1, &c , 1);	
		curid = -1; // main线程当前工作线程已经打印完成
		pthread_cond_broadcast(&cond);
		write(1, "broadcast", 9);
		pthread_mutex_unlock(&mut);
		write(1, "thr unlock", 10);
	}
	
	pthread_exit((void *)0);

	// 不会执行
	pthread_cleanup_pop(0);

}



