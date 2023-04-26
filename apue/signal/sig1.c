#include <stdio.h>
#include <unistd.h>
#include <signal.h>

typedef void (*sighandler_t)(int);

void sig_handler(int s);
int main(void)
{
	sighandler_t ret;
	int i = 0;
	struct sigaction act;

	// 改变信号的行为 signal(2) / sigaction(2)
	// signal(SIGINT, SIG_IGN);
	/*
	ret = signal(SIGINT, sig_handler);
	if (SIG_ERR == ret) {
		perror("signal()");
		return 1;
	}
	*/
	act.sa_handler = sig_handler; // 信号处理函数
	sigemptyset(&act.sa_mask); // 信号屏蔽字，在执行信号处理函数期间屏蔽的信号
	sigaddset(&act.sa_mask, SIGQUIT);
	act.sa_flags = 0;
	sigaction(SIGINT, &act, NULL);

	while (1) {
		i++;
		if (i == 10) {
			signal(SIGINT, SIG_DFL);
		}
		write(1, "*", 1);
		sleep(1);
	}

	return 0;
}

// 信号处理函数
void sig_handler(int s)
{
	for (int i = 0; i < 3; i++) {
		write(1, "!", 1);
		sleep(1);
	}
}



