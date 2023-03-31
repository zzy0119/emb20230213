/*
   实现cp功能
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define BUFSIZE	32

int mycopy(int rfd, int wfd);

int main(int argc, char *argv[])
{
	int fd1, fd2;
	int cnt;

	if (argc < 3)
		return 1;

	// 打开argv[1]文件
	fd1 = open(argv[1], O_RDONLY);
	if (-1 == fd1) {
		// 出错
		fprintf(stderr, "open():%s\n", strerror(errno));
		return 1;
	}

	fd2 = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0666);
	if (-1 == fd2) {
		perror("open()");
		goto ERROR;
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
		// 读所打开的文件，写标准输出
		cnt = read(rfd, buf, BUFSIZE);
		if (-1 == cnt) {
			perror("read()");
			return -1;
		}
		if (0 == cnt) {
			break;
		}
		write(wfd, buf, cnt);
	}

	return 0;
}






