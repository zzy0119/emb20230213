/*
 du -sh path 统计所占磁盘空间大小
 */
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define BUFSIZE	128
int mydu(const char *path); 

int main(int argc, char *argv[])
{
	int size;

	if (argc < 2)
		return 1;

	size = mydu(argv[1]);
	printf("%dK %s\n", size, argv[1]);

	return 0;
}

/*
 返回给定路径文件所占磁盘大小，单位K
 */
int mydu(const char *path) 
{
	struct stat res;
	DIR *dp;
	struct dirent *entry;
	char buf[BUFSIZE] = {};
	int sum;

	// 判断path文件类型
	if (-1 == stat(path, &res)) {
		perror("stat()");
		return -1;
	}
	// 如果是非目录文件
	if (!S_ISDIR(res.st_mode))
		return res.st_blocks / 2; // K

	// 初始化sum
	sum = res.st_blocks / 2;

	// 如果是目录--->解析目录
	dp = opendir(path);
	if (NULL == dp) {
		perror("opendir()");
		return -1;
	}

	while (1) {
		entry = readdir(dp);
		if (NULL == entry) {
			if (errno) {
				perror("readdir()");
				goto ERROR;
			}
			break;
		}
		// entry->d_name 目录下的文件名 . ..刨除
		if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
			continue;
		// 文件名匹配为路径 path-->"/etc" d_name--->"/passwd"--->"/etc/passwd"
		// 已知文件路径---》得到所占磁盘空间大小
		strcpy(buf, path);
		strcat(buf, "/");
		strcat(buf, entry->d_name);
		sum += mydu(buf);
	}

	closedir(dp);
	return sum;
ERROR:
	closedir(dp);
	return -1;
}

