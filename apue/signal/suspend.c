#include <stdio.h>
#include <unistd.h>
#include <signal.h>

static void sig_handler(int s);
int main(void)
{
	sigset_t set, oldset;

	signal(SIGINT, sig_handler);
	signal(SIGRTMIN, sig_handler);

	// 信号集变量使用之前必须先设置为🈳 / 满
	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	// 加入一个实时信号
	sigaddset(&set, SIGRTMIN);

	/*
	 打印10行*，每一行10颗*，每秒打印1个
	 要求打印每一行期间不受2号信号干扰
	 每一行结束后必须等待一个信号的到来，驱动程序的继续运行
	 */
	// 屏蔽2号信号
	sigprocmask(SIG_BLOCK, &set, &oldset);
	for (int line = 0; line < 10; line++) {
		for (int j = 0; j < 10; j++) {
			write(1, "*", 1);
			sleep(1);
		}
		write(1, "\n", 1);
		sigsuspend(&oldset); // 原子操作
		/*
		 sigprocmask(SIG_SETMASK, &oldset, &save);
		 pause();
		 sigprocmask(SIG_SETMASK, &save, NULL);
		 */
	}

	return 0;
}

static void sig_handler(int s)
{
	write(1, "!", 1);
}


