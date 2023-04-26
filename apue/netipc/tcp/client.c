#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

#include "proto.h"

int main(void)
{
	int tcp_socket;
	struct sockaddr_in server_addr;
	int new_socket;
	pid_t pid;
	char buf[MSGSIZE] = {};

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

	read(tcp_socket, buf, MSGSIZE);
	puts(buf);

	close(tcp_socket);

	return 0;
ERROR:
	close(tcp_socket);
}

