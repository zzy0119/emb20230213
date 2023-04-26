#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "tbf.h"

// ADT
typedef struct {
	int token; // 多线程竞争
	int cps;
	int burst;
	pthread_mutex_t token_mutex; // 多线程村取token同步
	pthread_cond_t token_cond; // 获取token变量的值采用通知法
}tbf_t;

static pthread_t tid;

// 库
static tbf_t *jobs[TBF_MAX] = {};
static pthread_mutex_t job_mut = PTHREAD_MUTEX_INITIALIZER; // 多线程同时tbf_init 同步
static int inited;

static void thread_moduler_load(void);
static void sig_moduler_unload(void);
static int get_free_pos_unlock();

int tbf_init(int cps, int burst)
{
	tbf_t *me;
	int pos;

	// 第一个令牌桶初始化--->启动多线程模块
	if (!inited) {
		thread_moduler_load();
		inited = 1;
	}

	me = malloc(sizeof(tbf_t));
	if (NULL == me) {
		return -ENOMEM;
	}
	me->token = 0;
	me->cps = cps;
	me->burst = burst;
	pthread_mutex_init(&me->token_mutex, NULL);
	pthread_cond_init(&me->token_cond, NULL);

	// 将初始化的令牌桶放到库中
	pthread_mutex_lock(&job_mut);
	pos = get_free_pos_unlock();
	if (-1 == pos) {
		// 没有空间了
		free(me);
		me = NULL;
		return -ENOSPC;
	}
	jobs[pos] = me;	
	pthread_mutex_unlock(&job_mut);

	return pos;
}

/*
 找到令牌桶库中第一个空闲位置
 */
static int get_free_pos_unlock()
{
	int pos;

	for (int i = 0; i < TBF_MAX; i++)
		if (NULL == jobs[i])
			return i;
	return -1;
}

/*
信号模块:周期性产生 SIGALRM	alarm(2) 
 */

static void* thr_handler(void *s);
static void thread_moduler_load(void)
{

	// 构建积攒令牌的线程
	int err = pthread_create(&tid, NULL, thr_handler, NULL);
	if (err) {
		fprintf(stderr, "pthread_create():%s\n", strerror(err));
		exit(1);
	}
}

// 并发的积攒令牌的线程函数 
static void* thr_handler(void *s)
{
	int i;

	while (1) {
		for (i = 0; i < TBF_MAX; i++) {
			if (jobs[i]) {
				// 令牌桶
				pthread_mutex_lock(&jobs[i]->token_mutex);
				jobs[i]->token += jobs[i]->cps;
				pthread_cond_broadcast(&jobs[i]->token_cond);
				if (jobs[i]->token >= jobs[i]->burst)
					jobs[i]->token = jobs[i]->burst;
				pthread_mutex_unlock(&jobs[i]->token_mutex);
			}
		}
		sleep(1);
	}
}

static void sig_moduler_unload(void)
{
	pthread_cancel(tid);
}


int tbf_fetch_token(int td, int n)
{
	int ret;

	if (!(td >= 0 && n > 0))
		return -EINVAL;
	pthread_mutex_lock(&jobs[td]->token_mutex);
	while (jobs[td]->token <= 0)
		// 等待条件发生变化
		pthread_cond_wait(&jobs[td]->token_cond, &jobs[td]->token_mutex);

	if (n > jobs[td]->token)
		ret = jobs[td]->token; // 不够你所需的令牌，全部给你
	else
		ret = n;
	jobs[td]->token -= ret;

	pthread_mutex_unlock(&jobs[td]->token_mutex);

	return ret;
}

int tbf_destroy(int td)
{
	if (!(td >= 0))
		return -EINVAL;
	pthread_mutex_lock(&job_mut);
	free(jobs[td]);
	jobs[td] = NULL;
	pthread_mutex_lock(&job_mut);

	return 0;
}

void tbf_destroy_all()
{
	sig_moduler_unload();

	for (int i = 0; i < TBF_MAX; i++)
		if (jobs[i])
			tbf_destroy(i);
}


