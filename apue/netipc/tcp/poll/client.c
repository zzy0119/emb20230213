#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <strings.h>

#include "proto.h"

#define BUFSIZE MSGSIZE	

int main(void)
{
	int tcp_socket;
	struct sockaddr_in server_addr;
	int new_socket;
	pid_t pid;
	char buf[BUFSIZE] = {};
	int cnt;

	tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == tcp_socket) {
		perror("socket()");
		exit(1);
	}

	// 请求连接
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(SERVER_PORT));
	inet_aton(SERVER_IP, &server_addr.sin_addr);
	if (-1 == connect(tcp_socket, (void *)&server_addr, sizeof(server_addr))) {
		perror("connect()");
		goto ERROR;
	}

	// 非阻塞io open用非阻塞选项 fcntl设置
	int tcp_socket_save = fcntl(tcp_socket, F_GETFL);
	fcntl(tcp_socket, F_SETFL, tcp_socket_save | O_NONBLOCK);
	int stdin_save = fcntl(0, F_GETFL);
	fcntl(0, F_SETFL, stdin_save | O_NONBLOCK);

	while (1) {
		bzero(buf, BUFSIZE);
		cnt = read(0, buf, BUFSIZE);
		if (-1 == cnt) {
			if (EAGAIN !=  errno) {
				// EAGAIN非阻塞io没有读到数据
				perror("read()");
				goto ERROR;
			}
		}
		if (0 == cnt) {
			break;
		}
		if (cnt > 0)
			write(tcp_socket, buf, cnt);
		cnt = read(tcp_socket, buf, MSGSIZE);
		if (-1 == cnt) {
			if (EAGAIN !=  errno) {
				// EAGAIN非阻塞io没有读到数据
				perror("read()");
				goto ERROR;
			}
		}
		if (0 == cnt)
			break;
		if (cnt > 0)
			puts(buf);
	}

	close(tcp_socket);

	return 0;
ERROR:
	close(tcp_socket);
}

