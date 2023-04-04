#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <dirent.h>

int main(int argc, char *argv[])
{
	DIR *dp;
	struct dirent *entry;

	if (argc < 2)
		return 1;

	dp = opendir(argv[1]);	
	if (NULL == dp) {
		perror("opendir()");
		return 1;
	}

	// 读
	while (1) {
		entry = readdir(dp);
		if (NULL == entry) {
			if (errno) {
				perror("readdir()");
				goto ERROR;
			}
			break; // 没有设置errno 读到了目录流的末尾
		}
		// 目录中文件名
		printf("%ld %s\n", entry->d_ino, entry->d_name);
	}

	closedir(dp);

	return 0;
ERROR:
	closedir(dp);
	return 1;
}

