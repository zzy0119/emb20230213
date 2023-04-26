#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <syslog.h>
#include <sys/file.h>

#define BUFSIZE		128
#define LOCKFILE	"/var/run/mydaemon.pid"

int already_running(void);
int mydaemon(void);
int main(void)
{
	time_t tm;
	struct tm *tmp;
	char buf[BUFSIZE] = {};
	FILE *fp;

	// 构建连接
	openlog("mydaemon", LOG_PID | LOG_PERROR, LOG_DAEMON);

	if (-1 == mydaemon())
		exit(1);

	// 单实例
	if (-1 == already_running()) {
		syslog(LOG_ERR, "already_running() failed");
		exit(1);
	}

	// 获取当前时间，将年月日十分妙每秒钟写入"/tmp/out"		
	fp = fopen("/tmp/out", "w");
	if (NULL == fp) {
		// perror("fopen()");
		// 提交日志
		syslog(LOG_ERR, "fopen():%s", strerror(errno));
		return 1;
	}

	while (1) {
		time(&tm);	
		tmp = localtime(&tm);
		memset(buf, 0, BUFSIZE);
		strftime(buf, BUFSIZE, "%Y/%m/%d %H:%M:%S\n", tmp);
		// printf("%s 写入了文件\n", buf);
		syslog(LOG_INFO, "buf写入了文件");
		fputs(buf, fp);
		fflush(NULL);
		sleep(1);
	}

	return 0;
}

int mydaemon(void)
{
	pid_t pid;
	int fd;

	pid = fork();
	if (-1 == pid) {
		perror("fork()");
		return -1;
	}
	if (pid > 0)
		exit(0);

	// 子进程
	if (-1 == setsid()) {
		perror("setsid()");
		return -1;
	}
	// SID == PGID == PID 脱离控制终端 守护进程的必要条件

	// 以下工作 守护进程最好做
	umask(0);
	chdir("/");

	fd = open("/dev/null", O_RDWR);
	if (-1 == fd) {
		perror("open()");
		return -1;
	}
	dup2(fd, 0);
	dup2(fd, 1);
	dup2(fd, 2);
	if (fd > 2)
		close(fd);

	return 0;
}

/*
 单实例守护进程:同时只能运行一次
 进程在启动之前要求获得文件的互斥锁
 flock(2) lockf(3)
 */
int already_running(void)
{
	// 拿到互斥锁
	int fd;	
	char buf[BUFSIZE] = {};

	fd = open(LOCKFILE, O_RDWR | O_CREAT, 0644);
	if (-1 == fd) {
		syslog(LOG_ERR, "open %s failed with %s", LOCKFILE, strerror(errno));
		return -1;
	}

	// 加锁
	if (-1 == flock(fd, LOCK_EX | LOCK_NB)) {
		close(fd);
		return -1;
	}
	// 成功了--》可以运行 将pid写入锁文件
	ftruncate(fd, 0);
	snprintf(buf, BUFSIZE, "%d", getpid());
	write(fd, buf, strlen(buf));

	// 一定不要关闭文件 会影响锁
	return 0;
}





