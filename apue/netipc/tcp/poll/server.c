#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <strings.h>
#include <poll.h>

#include "proto.h"

#define MAX2NUM(a, b) 	((a) > (b) ? (a) : (b))
#define MAX_CONNECT		1024
#define BUFSIZE			MSGSIZE

static int max_two_number(int a, int b);
int main(void)
{
	int tcp_socket;
	struct sockaddr_in myaddr;
	struct sockaddr_in client_addr;
	socklen_t client_addr_len;
	int new_socket;
	pid_t pid;
	struct sigaction act;
	int max_fd = -1;
	int connected_arr[MAX_CONNECT];
	int i = 0;
	char buf[BUFSIZE];
	int cnt, ret;
	struct pollfd pfd[MAX_CONNECT] = {};
	int nfds = 0;

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

	/*
	 需要监听客户端的连接是否到来
	 需要监听标准输入文件是否有数据，有数据发送给所有已连接的客户端
	 需要监听所有已连接成功的客户端是否有数据到来
	 */

	// 监听0
	pfd[0].fd = 0; // 监听的文件描述符
	pfd[0].events = POLLIN; 
	pfd[1].fd = tcp_socket;
	pfd[1].events = POLLIN;

	nfds = 2;

	while (1) {
		// 阻塞系统调用
		ret = poll(pfd, nfds, -1);
		if (-1 == ret) {
			if (EINTR == errno) {
				// 假错
				continue;
			}
			perror("poll()");
			goto ERROR;
		}
		// 监听的事件发生了
		if (pfd[1].revents & POLLIN && ret > 0) {
			// 有客户端连接请求到达
			new_socket = accept(tcp_socket, NULL, NULL);
			if (-1 == new_socket) {
				perror("accept()");
				goto ERROR;
			}

			// 成功连接的套接字添加到pfd数组中
			if (nfds < MAX_CONNECT) {
				pfd[nfds].fd = new_socket;
				pfd[nfds].events = POLLIN;
				nfds ++;
			}
		} else if (pfd[0].revents & POLLIN && ret > 0) {
			cnt = read(0, buf, BUFSIZE);
			for (int j = 2; j < nfds; j++)	 {
				write(pfd[j].fd, buf, cnt);
			}
		} else {
			// 客户端数据到来了
			for (int j = 2; j < nfds; j++) {
				if (pfd[j].revents & POLLIN) {
					bzero(buf, BUFSIZE);
					read(pfd[j].fd, buf, BUFSIZE);
					puts(buf);
				}
			}
		
		}
	}

	close(tcp_socket);

	return 0;
ERROR:
	close(tcp_socket);
}

static int max_two_number(int a, int b)
{
	return a > b ? a : b;
}




