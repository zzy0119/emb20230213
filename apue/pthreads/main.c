#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

void *new_thread(void *s);
int main(void)
{
	pthread_t tid;
	int err;
	int i;
	pthread_t main_tid;

	// 调用线程线程标识
	main_tid = pthread_self();

	err = pthread_create(&tid, NULL, new_thread, &main_tid);
	if (0 != err) { // if (err)
		fprintf(stderr, "pthread_exit():%s\n", strerror(err));		
		exit(1);
	}

	// main线程和新线程同时执行(并发)

	i = 0;
	while (1) {
		if (i++ >= 5)
			break;
		write(1, "*", 1);
		sleep(1);
	}

	// pthread_exit((void *)0);
	pthread_exit(NULL);
}

/*
 线程函数
 */
void *new_thread(void *s)
{
	pthread_t *m_tid = s;

	pthread_join(*m_tid, NULL);

	while (1) {
		write(1, "!", 1);
		sleep(1);
	}
}






