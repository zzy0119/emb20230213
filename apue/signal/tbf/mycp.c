/*
   实现cp功能
   指定速率
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define BUFSIZE	32
#define CPS		10
#define BURST	(20 * (CPS))

/*
   令牌桶:
   	token
	cps
	burst
	为了方便运算，让cps和token匹配,cps决定1秒钟积攒令牌的个数
 */
static int token = 0;

int mycopy(int rfd, int wfd);

/*
 信号处理函数
 令牌的积攒
 */
static void sig_handler(int s)
{
	alarm(1);
	token += CPS;
	if (token >= BURST)
		token = BURST;
}

int main(int argc, char *argv[])
{
	int fd1, fd2;
	int cnt;

	fd2 = 1;

	// 信号的行为注册好 并定义闹钟
	signal(SIGALRM, sig_handler);
	alarm(1);

	if (argc < 2)
		return 1;

	// 打开argv[1]文件
	fd1 = open(argv[1], O_RDONLY);
	if (-1 == fd1) {
		// 出错
		fprintf(stderr, "open():%s\n", strerror(errno));
		return 1;
	}

	if (argc >= 3) {
		fd2 = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0666);
		if (-1 == fd2) {
			perror("open()");
			goto ERROR;
		}
	}

	mycopy(fd1, fd2);

	// 关闭
	close(fd1);
	close(fd2);

	return 0;
ERROR:
	close(fd1);
	return 1;
}

/*
 复制
 */

int mycopy(int rfd, int wfd)
{
	char buf[BUFSIZE] = {};
	int cnt;

	while (1) {
		// 控制速率--->令牌桶
		while (token <= 0)
			pause(); // 等待信号到来
		// 取令牌
		token -= CPS;

		// 读所打开的文件，写标准输出
		cnt = read(rfd, buf, CPS); // 阻塞的系统调用
		if (-1 == cnt) {
			perror("read()");
			return -1;
		}
		if (0 == cnt) {

		}
		write(wfd, buf, cnt);
	}

	return 0;
}






