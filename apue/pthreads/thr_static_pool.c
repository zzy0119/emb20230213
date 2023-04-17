/*
 100~300之间共201个整型数，判断一个整型数是否为质数
 现要求有四个工作线程，能者多劳的方式竞争待判断的数值
 */
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>

#define START	100
#define END		300

#define THR_NR	4

// 任务发放和获取
/*
 > 0 待计算的数值
 == 0 数值还未发放
 == -1 所有数值都已发放完成
 */
static int job = 0; // 所有线程都要存取--->临界区--->同步--->互斥量
// 初始化 unlocked
static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

static void *job_thread(void *s);
	
int main(void)
{
	pthread_t tids[THR_NR] = {};	
	int arr[THR_NR] = {};
	int err;
	int i, j;

	// 并发4个工作线程	
	for (i = 0; i < THR_NR; i++) {
		err = pthread_create(tids + i, NULL, job_thread, NULL);
		if (err) {
			fprintf(stderr, "pthread_create():%s\n", strerror(err));
			exit(1);
		}
	}

	// 发放待计算的数值
	for (i = START; i <= END; i++) {
		// 依次将i的值赋给job	
		// 如果返回了说明竞争到了此互斥量，如果已经被占用了会阻塞
		pthread_mutex_lock(&mut);
		while (job != 0) { // 异步事件-->轮询/通知 
			// 只有将互斥量状态设置为unlock job值才有可能变化
			pthread_mutex_unlock(&mut);
			pthread_mutex_lock(&mut);
		}
		job = i;
		pthread_mutex_unlock(&mut);
	}

	// 等待job为0--》最后一个数值也被计算完成了
	pthread_mutex_lock(&mut);
	while (job > 0) {
		pthread_mutex_unlock(&mut);
		pthread_mutex_lock(&mut);
	}
	job = -1; // 所有任务发放完成
	pthread_mutex_unlock(&mut);

	// 等着收尸
	for (i = 0; i < THR_NR; i++) {
		pthread_join(tids[i], NULL);
	}

	pthread_mutex_destroy(&mut);

	return 0;
}

static int is_primer(int n);

static void *job_thread(void *s)
{
	int n;

	while (1) {
		pthread_mutex_lock(&mut);	
		if (-1 == job) {
			pthread_mutex_unlock(&mut);
			break;
		}
		if (0 == job) {
			// main线程还未发放任务
			pthread_mutex_unlock(&mut);
			continue;
		}

		n = job;
		job = 0; // 任务已经被取走
		pthread_mutex_unlock(&mut);
		pause();

		if (is_primer(n))
			printf("%d is a primer\n", n);
	}

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



