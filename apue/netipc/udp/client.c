// 主动端
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "udp_proto.h"

int main(int argc, char **argv)
{
	int udp_socket;
	// 地址类型 man 7 ip
	struct sockaddr_in local_addr, remote_addr;
	socklen_t remote_addr_len;
	struct msg_st buf;
	int cnt;

	if (argc < 3)
		exit(1);

	// 创建套接字
	udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if (-1 == udp_socket) {
		perror("socket()");
		exit(1);
	}

	/*
	   可省略 内核自动bind空闲的port
	*/

	// sendto();
	remote_addr.sin_family = AF_INET;
	remote_addr.sin_port = htons(SERVER_PORT); // 转换为网络字节序
	// 点分十进制地址字符串转换为地址结构体
	inet_aton(SERVER_IP, &remote_addr.sin_addr);
	
	buf.id = atoi(argv[1]);
	strncpy(buf.name, argv[2], NAMESIZE);
	sendto(udp_socket, &buf, sizeof(int8_t) + strlen(argv[2]) + 1, 0, (void *)&remote_addr, sizeof(remote_addr));

	close(udp_socket);

	return 0;
ERROR:
	close(udp_socket);
	exit(0);
}


