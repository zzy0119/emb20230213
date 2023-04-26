#include <sys/mman.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define DEBUG
#undef DEBUG

int main(void)
{
	void *ptr;
	pid_t pid;

	// 共享内存
	ptr = mmap(NULL, 1024, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, \ -1, 0);
	if (MAP_FAILED == ptr) {
		perror("mmap()");
		exit(1);
	}
#ifdef DEBUG
	printf("[%d]debug...\n", __LINE__);
#endif

	pid = fork();
	if (-1 == pid) {
		perror("fork()");	
		goto ERROR;
	}

	if (0 == pid) {
		memcpy(ptr, "hello", 5);
		munmap(ptr, 1024);
#ifdef DEBUG
		printf("[%d]debug...\n", __LINE__);
#endif
		exit(0);
	}
#ifdef DEBUG
	printf("[%d]debug...\n", __LINE__);
#endif
	wait(NULL);
#ifdef DEBUG
	printf("[%d]debug...\n", __LINE__);
#endif

	puts(ptr);

	munmap(ptr, 1024);
	return 0;
ERROR:
	munmap(ptr, 1024);
	exit(1);
}

