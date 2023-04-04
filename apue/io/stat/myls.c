#include <stdio.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <grp.h>
#include <time.h>

#define BUFSIZE	128

char file_type(mode_t mode);
char *file_mode(mode_t mode);
long file_nlink(nlink_t n);
const char *file_owner(uid_t uid);
const char *file_group(gid_t gid);
long file_size(size_t size);
const char *file_mtime(time_t tm);

/*
 char *argv[]
 */
int main(int argc, char **argv)
{
	struct stat res;
	char *mode;

	if (argc < 2)
		return 1;

	if (-1 == stat(argv[1], &res)) {
		perror("stat()");
		return 1;
	}


	mode = file_mode(res.st_mode);
	printf("%c%s %ld %s %s %ld %s %s\n", file_type(res.st_mode), mode, file_nlink(res.st_nlink), \
			file_owner(res.st_uid), file_group(res.st_gid), file_size(res.st_size), \
			file_mtime(res.st_mtime), argv[1]);

	free(mode);
	mode = NULL;

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

/*
 文件权限
 基本权限:
 	拥有者 所属组 其他用户: r w x
 */
char *file_mode(mode_t mode)
{
	// static char buf[10] = {};
	char *ret = malloc(sizeof(char) * 10);
	char m[] = "rwxrwxrwx";
	int value[] = {S_IRUSR, S_IWUSR, S_IXUSR, S_IRGRP, S_IWGRP, S_IXGRP, S_IROTH, S_IWOTH, S_IXOTH};
	int i;

	for (i = 0; i < strlen(m); i++) {
		if (!(mode & value[i]))
			m[i] = '-';
	}

	memset(ret, 0, 10);
	strcpy(ret, m);

	return ret;

}

/*
 硬连接个数
 */
long file_nlink(nlink_t n)
{
	return n;
}

/*
 拥有者 "/etc/passwd"
 */
const char *file_owner(uid_t uid)
{
	struct passwd *pwd = getpwuid(uid);
	if (NULL == pwd) {
		perror("getspuid()");
		return NULL;
	}

	return pwd->pw_name;
}

/*
 所属组 "/etc/group"
 */
const char *file_group(gid_t gid)
{
	struct group *grp = getgrgid(gid);
	if (NULL == grp) {
		perror("getgrgid()");
		return NULL;
	}

	return grp->gr_name;
}

/*
 文件字节大小
 */
long file_size(size_t size)
{
	return size;
}

/*
 文件最后一次修改时间
 mtime
 stat(2) ---> st_mtime(time_t)--->localtime(3)时间结构体--->strftime(3)格式化的时间字符串
 */
const char *file_mtime(time_t tm)
{
	static char buf[BUFSIZE] = {};

	struct tm *tmp = localtime(&tm);
	strftime(buf, BUFSIZE, "%m月 %d %H:%M", tmp);

	return buf;
}



