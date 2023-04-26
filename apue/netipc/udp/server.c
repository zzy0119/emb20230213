// 被动端
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <stdlib.h>
#include <unistd.h>

#include "udp_proto.h"

int main(void)
{
	int udp_socket;
	// 地址类型 man 7 ip
	struct sockaddr_in local_addr, remote_addr;
	socklen_t remote_addr_len;
	struct msg_st buf;
	int cnt;

	// 创建套接字
	udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if (-1 == udp_socket) {
		perror("socket()");
		exit(1);
	}

	// 绑定本地地址
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(SERVER_PORT); // 转换为网络字节序
	// 点分十进制地址字符串转换为地址结构体
	inet_aton(SERVER_IP, &local_addr.sin_addr);
	if (-1 == bind(udp_socket, (void *)&local_addr, sizeof(local_addr))) {
		perror("bind()");
		goto ERROR;
	}

	// 接受数据包之前一定初始化raddrlen
	remote_addr_len = sizeof(remote_addr);
	while (1) {
		cnt = recvfrom(udp_socket, &buf, sizeof(buf), 0, (void *)&remote_addr, &remote_addr_len);
		if (-1 == cnt) {
			perror("recvfrom()");
			goto ERROR;
		}
		printf("*****************debug*****************\n");
		printf("ip:%s, port:%d\n", inet_ntoa(remote_addr.sin_addr)/*将地址结构体转换为地址字符串*/, \
				ntohs(remote_addr.sin_port));
		printf("msg id:%hhd, msg name:%s\n", buf.id, buf.name);
		printf("*****************debug*****************\n");
	}

	// sendto();
	close(udp_socket);

	return 0;
ERROR:
	close(udp_socket);
	exit(0);
}


