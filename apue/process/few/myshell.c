#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h> 

#define N	128

int parse_string(char *ptr, char *delim, char *arr[], int n);
int main(void)
{
	char *lineptr = NULL;
	size_t n = 0;
	ssize_t ret;
	char *arr[N] = {};
	pid_t pid;

	while (1) {
		// 命令提示字符串
		printf("[%s]$ ", getenv("PWD"));
		// 读输入的命令
		ret = getline(&lineptr, &n, stdin); // "ls -l\n"
		if (-1 == ret) {
			break;
		}
		if (!strcmp(lineptr, "exit\n"))
			break;
		// 切割"ls" "-l"  ---> strtok(3) / strsep(3)--->arr指针数组中
		parse_string(lineptr, " \t\n", arr, N);
	
#if 1
		pid = fork();
		if (-1 == pid) {
			perror("fork()");
			exit(1);
		}
		if (0 == pid) {
			// 进程替换
			execvp(arr[0], arr);
			perror("execvp()");
			exit(1);
		}
#endif
		wait(NULL);
	}
	free(lineptr);
	lineptr = NULL;

	return 0;
}

/*
 将ptr字符串按照delim切割符切割，将结果回填到arr指针数组，数组容量n个元素
 */
int parse_string(char *ptr, char *delim, char *arr[], int n)
{
	int i = 0;

	memset(arr, 0, n * sizeof(char *));
	while (i < n - 1) {
		arr[i] = strtok(ptr, delim);
		if (arr[i] == NULL)
			break;
		if (strcmp(arr[i], "") == 0)
			continue;
		i++;
		ptr = NULL;
	}

	return 0;
}




