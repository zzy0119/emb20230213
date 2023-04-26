#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h> 

void fun2(void);
void fun1(void);

int main(void)
{
	pid_t pid;
	int n = 100;

	atexit(fun1);
	atexit(fun2);
	// 标准输出缓存区
	printf("good afternoon");

	// pid = fork();
	pid = vfork();
	if (-1 == pid) {
		perror("fork()");
		return 1;
	}
	if (0 == pid) {
		// child
		// 注册终止处理函数

		sleep(1);
		printf("pid:%d ppid:%d, the child process is running\n", getpid(), getppid());

		// 复制了父进程的虚拟地址空间，数据相同
		printf("child pid:%d\n", n++);

		// return 0; // 进程终止
		// exit(3)
		// exit(EXIT_SUCCESS); // exit(0)
		// _exit(0);
		_exit(0);
	}

	// 等待子进程终止---->收尸
	// wait(NULL); // 等待任意一个子进程终止，并不关心其终止状态

	printf("pid:%d ppid:%d, the parent process is running\n", getpid(), getppid());
	printf("n:%d\n", n);

	return 0;
}

// 进程终止处理函数
void fun1(void)
{
	printf("%s is called\n", __FUNCTION__);
}

void fun2(void)
{
	printf("%s is called\n", __FUNCTION__);
}



