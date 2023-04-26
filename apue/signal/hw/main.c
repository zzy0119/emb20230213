#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include "hw.h"

void alarm_handler1(void *s)
{
	write(1, "!", 1);;
}

void alarm_handler2(void *s)
{
	write(1, "$", 1);;
}

void alarm_handler3(void *s)
{
	write(1, "&", 1);;
}

int main(void)
{
	struct sigaction act;
	alarm_handler_t handl;
	/*
	act.sa_handler = alarm_handler;
	act.sa_flags = 0;
	sigaction(SIGALRM, &act, NULL);
	*/

	// 闹钟3s，3s后调用alarm_handler函数
	// myalarm(3, void *(*p)(void *), void *arg);
	handl.fun = alarm_handler1;
	handl.arg = NULL;
	myalarm(3, handl);

	handl.fun = alarm_handler2;
	handl.arg = NULL;
	myalarm(5, handl);

	handl.fun = alarm_handler3;
	handl.arg = NULL;
	myalarm(2, handl);
	// **&*!**$*******.....

	while (1) {
		write(1, "*", 1);
		sleep(1);
	}

	return 0;
}

