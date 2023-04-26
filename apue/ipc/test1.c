/*
	下载一个mp3音乐文件	
	父进程将此文件发送给子进程，子进程播放
	提示:
		下载安装播放器， sudo apt install mplayer
		子进程运行播放器进程: execxx();
		播放器进程读管道:
			dup2(pfd[0], 0);
			mplayer -  读0文件描述符
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUFSIZE	1024

int mycopy(int rfd, int wfd);

int main(int argc, char *argv[])
{
	int pfd[2] = {};
	int fd;
	pid_t pid;

	if (argc < 2)
		return 1;
	
	if (-1 == pipe(pfd)) {
		perror("pipe()");
		return 1;
	}

	pid = fork();
	if (-1 == pid) {
		perror("fork()");
		goto ERROR;
	}
	if (0 == pid) {
		close(pfd[1]);
		// 运行播放器进程
		dup2(pfd[0], 0);
		execl("/usr/bin/mplayer", "mplayer", "-", NULL);
		perror("execl()");
		close(pfd[0]);
		exit(0);
	}

	fd = open(argv[1], O_RDONLY);
	if (-1 == fd) {
		perror("open()");
		goto ERROR;
	}
	close(pfd[0]);
	mycopy(fd, pfd[1]);
	close(pfd[1]);
	close(fd);

	return 0;
ERROR:
	close(pfd[0]);
	close(pfd[1]);

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
		cnt = read(rfd, buf, BUFSIZE); // 阻塞的系统调用
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







