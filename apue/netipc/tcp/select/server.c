#include <stdio.h>
#include <string.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/select.h>
#include <errno.h>
#include <strings.h>

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
	fd_set rset, tmp_rset;
	int max_fd = -1;
	int connected_arr[MAX_CONNECT];
	int i = 0;
	char buf[BUFSIZE];
	int cnt, ret;

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

	// 清空读集
	FD_ZERO(&rset);
	// 标准输入0加入读集
	FD_SET(tcp_socket, &rset);
	FD_SET(0, &rset);

	max_fd = max_two_number(tcp_socket, 0);

	while (1) {
		// 阻塞系统调用
		tmp_rset = rset;
		ret = select(max_fd + 1, &tmp_rset, NULL, NULL, NULL);	
		if (-1 == ret) {
			if (EINTR == errno) {
				// 假错
				continue;
			}
			perror("select()");
			goto ERROR;
		}
		// 监听的事件发生了
		if (FD_ISSET(tcp_socket, &tmp_rset) && ret > 0) {
			// 有客户端连接请求到达
			connected_arr[i] = accept(tcp_socket, NULL, NULL);
			if (-1 == new_socket) {
				perror("accept()");
				goto ERROR;
			}
			FD_SET(connected_arr[i], &rset);
			// 最大文件描述符可能会发生变化
			max_fd = max_two_number(max_fd, connected_arr[i]);
			i++;
		} else if (FD_ISSET(0, &tmp_rset) && ret > 0) {
			cnt = read(0, buf, BUFSIZE);
			for (int j = 0; j < i; j++)	 {
				write(connected_arr[j], buf, cnt);
			}
		} else {
			// 客户端数据到来了
			for (int j = 0; j < i; j++) {
				if (FD_ISSET(connected_arr[j], &tmp_rset)) {
					bzero(buf, BUFSIZE);
					cnt = read(connected_arr[j], buf, BUFSIZE);
					if (-1 == cnt) {
						perror("read()");
						goto ERROR;
					}
					if (0 == cnt) {
						// 客户端连接已经断开
						FD_CLR(connected_arr[j], &rset);
						close(connected_arr[j]);
						memcpy(connected_arr + j, connected_arr + j + 1, (i - (j + 1)) * sizeof(int));
						i--;
						j--;
					}

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




