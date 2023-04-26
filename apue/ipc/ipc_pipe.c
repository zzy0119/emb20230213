#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFSIZE	3

int main(void)
{
	int pfd[2] = {};
	pid_t pid;
	char buf[BUFSIZE] = {};
	int cnt;

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
		// 不需要的一端关掉!!!!!!
		close(pfd[0]);
		sleep(3);
		write(pfd[1], "hello world", 11);
		close(pfd[1]);
		exit(0);
	}

	// 读
	close(pfd[1]);
	while (1) {
		cnt = read(pfd[0], buf, BUFSIZE);
		if (-1 == cnt) {
			perror("read()");
			close(pfd[0]);	
			exit(1);
		}
		if (0 == cnt) {
			// eof 所有写端关闭
			break;
		}
		write(1, buf, cnt);
	}
	close(pfd[0]);

	wait(NULL);

	return 0;
ERROR:
	close(pfd[0]);
	close(pfd[1]);
	exit(1);
}

