#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define BUFSIZE	4

static int reverse_file(int src_fd, int dest_fd);

int main(int argc, char *argv[])
{
	int fd1, fd2;

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

	reverse_file(fd1, fd2);

	close(fd1);
	close(fd2);

	return 0;
ERROR:
	close(fd1);

	return 1;
}

static int reverse_str(char *buf)
{
	char c;	
	for (int i = 0, j = strlen(buf) - 1 ; i < j; i++, j--) {
		c = buf[i];
		buf[i] = buf[j];
		buf[j] = c;
	}
	return 0;
}

static int reverse_file(int src_fd, int dest_fd)
{
	char buf[BUFSIZE+1] = {};	
	int size;
	int cnt;
	int readn;

	// 得到src_fd文件的字节大小
	size = lseek(src_fd, 0, SEEK_END);
	
	// 倒序读写
	while (size > 0) {
		// 初始化buf
		memset(buf, 0, BUFSIZE + 1);

		if (size >= BUFSIZE) {
			size -= BUFSIZE;
			readn = BUFSIZE;
		} else {
			// 剩余的字符不足BUFSIZE
			readn = size;
			size = 0;
		}
		if (-1 == lseek(src_fd, size, SEEK_SET)) {
			perror("lseek()");
			return -1;
		}

		cnt = read(src_fd, buf, readn);	
		if (-1 == cnt) {
			perror("read()");	
			return -1;
		}
	
		// printf("read string:%s\n", buf);
		reverse_str(buf);
		// printf("reverser string:%s\n", buf);

		write(dest_fd, buf, cnt);
	}

	return 0;
}


