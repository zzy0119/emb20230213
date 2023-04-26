/*
 100~300之间共201个整型数，加入判断一个整型数是否为质数需要1s钟，
 现在要求201个整型数都要判断是否为质数，要求在1s内判断
 质数打印输出
 和数则不输出
 */
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

#define START	100
#define END		300

static int is_primer(int n);

int main(void)
{
	pid_t pid;
	int i;
	
	for (i = START; i <= END; i++) {
		pid = fork();
		if (-1 == pid) {
			perror("fork()");
			exit(1);
		}
		if (0 == pid) {
			if (is_primer(i)) {
				printf("%d is a primer\n", i);
			}
			exit(0);
		}
	}

	for (i = START; i <= END; i++)
		wait(NULL);

	return 0;
}

/*
 筛质数
 */
static int is_primer(int n)
{
	int i;

	if (n < 1)
		return -1;

	if (n == 1)
		return 0;

	for (i = 2; i < n; i++) {
		if (n % i == 0)		
			return 0;
	}

	return 1;
}


