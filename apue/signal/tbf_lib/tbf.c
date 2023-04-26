#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include "tbf.h"

// ADT
typedef struct {
	int token;
	int cps;
	int burst;
}tbf_t;

typedef void (*sighandler_t)(int);
static sighandler_t alrm_save;

// 库
static tbf_t *jobs[TBF_MAX] = {};
static int inited;

static void sig_moduler_load(void);
static void sig_moduler_unload(void);
static int get_free_pos();

int tbf_init(int cps, int burst)
{
	tbf_t *me;
	int pos;

	// 第一个令牌桶初始化--->启动信号模块
	if (!inited) {
		sig_moduler_load();
		inited = 1;
	}

	me = malloc(sizeof(tbf_t));
	if (NULL == me) {
		return -ENOMEM;
	}
	me->token = 0;
	me->cps = cps;
	me->burst = burst;

	pos = get_free_pos();
	if (-1 == pos) {
		// 没有空间了
		free(me);
		me = NULL;
		return -ENOSPC;
	}
	jobs[pos] = me;	

	return pos;
}

/*
 找到令牌桶库中第一个空闲位置
 */
static int get_free_pos()
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

static void alarm_handler(int s);
static void sig_moduler_load(void)
{
	alrm_save = signal(SIGALRM, alarm_handler);	
	alarm(1);
}

// SIGALRM信号的信号处理函数 
static void alarm_handler(int s)
{
	int i;

	alarm(1);

	for (i = 0; i < TBF_MAX; i++) {
		if (jobs[i]) {
			// 令牌桶
			jobs[i]->token += jobs[i]->cps;
			if (jobs[i]->token >= jobs[i]->burst)
				jobs[i]->token = jobs[i]->burst;
		}
	}
}

static void sig_moduler_unload(void)
{
	signal(SIGALRM, alrm_save);	
	alarm(0);
}


int tbf_fetch_token(int td, int n)
{
	int ret;

	if (!(td >= 0 && n > 0))
		return -EINVAL;
	while (jobs[td]->token <= 0)
		pause();
	if (n > jobs[td]->token)
		ret = jobs[td]->token; // 不够你所需的令牌，全部给你
	else
		ret = n;
	jobs[td]->token -= ret;

	return ret;
}

int tbf_destroy(int td)
{
	if (!(td >= 0))
		return -EINVAL;
	free(jobs[td]);
	jobs[td] = NULL;

	return 0;
}

void tbf_destroy_all()
{
	sig_moduler_unload();

	for (int i = 0; i < TBF_MAX; i++)
		if (jobs[i])
			tbf_destroy(i);
}


