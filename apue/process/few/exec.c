#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(void)
{
	pid_t pid;
	char *my_args[] = {"ls", "-l", "-a", NULL};

	printf("fork child process\n");
	pid = fork();
	if (-1 == pid) {
		perror("fork()");
		return -1;
	}

	if (0 == pid) {
		// 执行不一样的程序
		// execl("/bin/ls", "ls", "-l", NULL);
		// execlp()要求可执行文件必须在环境遍历PATH路径下
		// execlp("fork_p2", "./fork_p2", NULL);
		execv("/bin/ls", my_args);
		perror("execl()");
		exit(1);
	}
	wait(NULL);
	printf("child exit\n");

	return 0;
}

