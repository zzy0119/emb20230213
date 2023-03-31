/*
   实现cat功能
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define BUFSIZE	32

int main(int argc, char *argv[])
{
	int fd;
	char buf[BUFSIZE] = {};
	int cnt;

	if (argc < 2)
		return 1;

	// 打开argv[1]文件
	fd = open(argv[1], O_RDONLY);
	if (-1 == fd) {
		// 出错
		fprintf(stderr, "open():%s\n", strerror(errno));
		return 1;
	}

	while (1) {
		// 读所打开的文件，写标准输出
		cnt = read(fd, buf, BUFSIZE);
		if (-1 == cnt) {
			perror("read()");
			goto ERROR;
		}
		if (0 == cnt) {
			break;
		}
		write(1, buf, cnt);
	}

	// 关闭
	close(fd);

	return 0;
ERROR:
	close(fd);
	return 1;
}


