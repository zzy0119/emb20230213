#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(void)
{
	pid_t pid;

	pid = fork();
	if (-1 == pid) {
		perror("fork()");
		return 1;
	}

	if (0 == pid) {
		// child
		while (1) {
			write(1, "!", 1);
			sleep(1);
		}
	}

	while (1) {
		write(1, "*", 1);
		sleep(1);
	}

	return 0;
}


