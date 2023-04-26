#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

#include "proto.h"

int main(void)
{
	int tcp_socket;
	struct sockaddr_in myaddr;
	struct sockaddr_in client_addr;
	socklen_t client_addr_len;
	int new_socket;
	pid_t pid;
	struct sigaction act;

	/*
	 SIGCHLD
	 当子进程终止的时候不变成僵尸进程，直接释放资源
	 */
	act.sa_handler = SIG_DFL;
	act.sa_flags = SA_NOCLDWAIT;
	sigaction(SIGCHLD, &act, NULL);

	tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == tcp_socket) {
		perror("socket()");
		exit(1);
	}

	// 复用地址:解决当服务器已经终止，但是端口还未被释放,再次bind出错
	int val = 1;
	if (-1 == setsockopt(tcp_socket, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val))) {
		perror("setsockopt()");
		goto ERROR;
	}

	myaddr.sin_family = AF_INET;
	myaddr.sin_port = htons(atoi(SERVER_PORT));
	myaddr.sin_addr.s_addr = INADDR_ANY; // == inet_aton("0.0.0.0", &myaddr.sin_addr)
	if (-1 == bind(tcp_socket, (struct sockaddr *)&myaddr, sizeof(myaddr))) {
		perror("bind()");
		goto ERROR;
	}

	// 将套接字设置为监听状态
	if (-1 == listen(tcp_socket, 32)) {
		perror("listen()");
		goto ERROR;
	}

	client_addr_len = sizeof(struct sockaddr_in);
	while (1) {
		//返回新的套接字，用于数据交换 
		new_socket = accept(tcp_socket, (void *)&client_addr, &client_addr_len);
		if (-1 == new_socket) {
			perror("accept()");
			goto ERROR;
		}
		printf("****************************\n");
		printf("connect successfully with ip:%s, port:%d\n", \
				inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
		printf("****************************\n");

		if ((pid = fork()) == -1) {
			perror("fork()");
			goto ERROR;
		}
		if (0 == pid) {
			// 数据交换
			close(tcp_socket);
			write(new_socket, "hello", 6);
			close(new_socket);
			exit(0);
		}
		close(new_socket);
	}

	close(tcp_socket);

	return 0;
ERROR:
	close(tcp_socket);
}

