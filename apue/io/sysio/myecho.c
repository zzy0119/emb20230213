#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	int fd;

	if (argc < 2)
		return 1;

	// 判断是否有argv[3]
	if (argc > 3 && (strcmp(argv[2], ">") == 0 || strcmp(argv[2], "1>") == 0)) {
		fd = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, 0666);
		// if error
		/*
		close(1);
		dup(fd); // 当前可用最小最为fd复制--->1
		*/
		dup2(fd, 1); // 原子操作
	}

	write(1, argv[1], strlen(argv[1]));
	write(1, "\n", 1);

	return 0;
}

