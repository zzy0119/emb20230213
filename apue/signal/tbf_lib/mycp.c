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
#include "tbf.h"

#define BUFSIZE	32
#define CPS		10
#define BURST	(20 * (CPS))

int mycopy(int rfd, int wfd);

int main(int argc, char *argv[])
{
	int fd1, fd2;
	int cnt;

	fd2 = 1;

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
	int td;
	int n;

	// 初始化令牌桶
	td = tbf_init(CPS, BURST);
	if (td < 0) {
		fprintf(stderr, "tbf_init(): %s\n", strerror(-td));
		return -1;
	}

	while (1) {
		n = tbf_fetch_token(td, CPS);

		// 读所打开的文件，写标准输出
		cnt = read(rfd, buf, n); // 阻塞的系统调用
		if (-1 == cnt) {
			perror("read()");
			return -1;
		}
		if (0 == cnt) {

		}
		write(wfd, buf, cnt);
	}

	tbf_destroy(td);

	return 0;
}






