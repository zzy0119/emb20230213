#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

char file_type(mode_t mode);

int main(int argc, char **argv)
{
	struct stat res;

	if (argc < 2)
		return 1;

	if (-1 == stat(argv[1], &res)) {
		perror("stat()");
		return 1;
	}

	printf("%c\n", file_type(res.st_mode));


	return 0;
}

/*
   获取文件类型
   - 普通文件
   d 目录文件
   c 字符设备文件
   b 块设备文家
   l 符号连接文件
   p 管道文件
   s 套接字文件
 */
char file_type(mode_t mode)
{
	char c;

	switch (mode & S_IFMT) {
		case S_IFREG:
			c = '-';		
			break;
		case S_IFDIR:
			c = 'd';
			break;
		default:
			c = '?';
			break;
	}

	return c;
}










