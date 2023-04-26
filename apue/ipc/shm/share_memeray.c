#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>

int main(void)
{
	pid_t pid;
	int shmid;
	void *p;

	shmid = shmget(IPC_PRIVATE, 1024, IPC_CREAT | IPC_EXCL | 0600);
	if (-1 == shmid) {
		if (EEXIST == errno)
			shmid = shmget(IPC_PRIVATE, 1024, 0);
		else {
			perror("shmget()");
			exit(1);
		}
	}

	pid = fork();
	if (-1 == pid) {
		perror("fork()");
		goto ERROR;
	}
	if (0 == pid) {
		// 使用共享内存  映射到调用进程的虚拟地址空间
		void *ptr = shmat(shmid, NULL, 0);
		if ((void *)-1 == ptr) {
			perror("shmat()");
			exit(1);
		}

		memcpy(ptr, "got it", 6);

		// 解除映射
		shmdt(ptr);
		exit(0);
	}
	
	wait(NULL);

	p = shmat(shmid, NULL, 0);
	if ((void *)-1 == p) {
		perror("shmat()");
		goto ERROR;
	}
	
	puts(p);

	shmdt(p);
	p = NULL;


	shmctl(shmid, IPC_RMID, NULL);

	return 0;
ERROR:
	shmctl(shmid, IPC_RMID, NULL);
	exit(1);
}

