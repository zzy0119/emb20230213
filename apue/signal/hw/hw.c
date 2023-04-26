#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <errno.h>

#include "hw.h"

// 时钟结构
typedef struct {
	int sec;
	alarm_handler_t handler;
}alarm_t;

// 时钟库
static alarm_t *jobs[MAX_ALARM];
static struct sigaction oldact;
static struct itimerval old_itv;
static int inited;

static void alarm_moduler_load(void);
static void alarm_moduler_unload(void);

static int get_free_pos()
{
	int i;

	for (i = 0; i < MAX_ALARM; i++)
		if (NULL == jobs[i])
			return i;

	return -1;
}

int myalarm(int sec, alarm_handler_t hd)
{
	alarm_t *me;
	int pos;

	// 第一次注册闹钟 启动SIGALARM信号	
	if (!inited) {
		alarm_moduler_load();
		inited = 1;
	}

	me = malloc(sizeof(*me)); // int i = 10; sizeof(i++)
	if (NULL == me)
		return -ENOMEM;
	me->sec = sec;
	me->handler = hd;

	// 存库
	pos = get_free_pos();	
	if (-1 == pos) {
		free(me);
		me = NULL;
		return -ENOSPC;
	}
	jobs[pos] = me;

	return pos;
}

static void alarm_handler(int s)
{
	// 遍历时钟库 
	for (int i = 0; i < MAX_ALARM; i++) {
		if (jobs[i]) {
			// 有时钟
			jobs[i]->sec --;
			if (jobs[i]->sec == 0) {
				// 响应
				(jobs[i]->handler.fun)(jobs[i]->handler.arg);
				myalarm_cancel(i);
			}
		}
	}
}

static void alarm_moduler_load(void)
{
	struct sigaction act = {};
	struct itimerval itv;

	// 注册SIGALRM信号的行为
	act.sa_handler = alarm_handler;
	sigaction(SIGALRM, &act, &oldact);

	// 定闹钟
	itv.it_interval.tv_sec = 1;
	itv.it_interval.tv_usec = 0;
	itv.it_value.tv_sec = 1;
	itv.it_value.tv_usec = 0;
	setitimer(ITIMER_REAL, &itv, &old_itv);
}

static void alarm_moduler_unload(void)
{
	sigaction(SIGALRM, &oldact, NULL);
	setitimer(ITIMER_REAL, &old_itv, NULL);
}

void myalarm_cancel(int alarm_dsc)
{
	jobs[alarm_dsc]->sec = 0;
	free(jobs[alarm_dsc]);
	jobs[alarm_dsc] = NULL;
}

void myalarm_destroy_all(void)
{
	int i;

	for (i = 0; i < MAX_ALARM; i++) {
		if (jobs[i]) {
			myalarm_cancel(i);
		}
	}
}





