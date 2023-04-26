#ifndef __HW_H__
#define __HW_H__

#define MAX_ALARM	1024

// typedef void (*alarm_t)(void *);
typedef struct {
	void (*fun)(void *);
	void *arg;
}alarm_handler_t;

// 注册的闹钟
// int myalarm(int sec, alarm_t handler, void *arg);
int myalarm(int sec, alarm_handler_t hd);

void myalarm_cancel(int alarm_dsc);

void myalarm_destroy_all(void);

#endif

