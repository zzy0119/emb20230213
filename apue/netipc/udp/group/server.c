#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <strings.h>
#include <net/if.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "proto.h"

int main(void)
{
	int udp_sd;
	struct sockaddr_in raddr;
	struct ip_mreqn imr;
	struct msg_st buf;

	udp_sd = socket(AF_INET, SOCK_DGRAM, 0);
	if (-1 == udp_sd) {
		perror("socket()");
		exit(1);
	}

	/*
	   可省略
	if (-1 == bind(udp_sd, (struct sockaddr *)&raddr, sizeof(raddr))) {
		perror("bind()");
		goto ERROR;
	}
	*/

	// 将本地设备用于多播
	inet_aton(GROUP_IP, &imr.imr_multiaddr); // 多播地址
	inet_aton("0.0.0.0", &imr.imr_address); // 本地地址
	imr.imr_ifindex = if_nametoindex("ens33"); // "ens33"转换为设备号
	if (-1 == setsockopt(udp_sd, IPPROTO_IP, IP_MULTICAST_IF, &imr, sizeof(imr))) {
		perror("setsockopt()");
		goto ERROR;
	}

	raddr.sin_family = AF_INET;
	inet_aton(GROUP_IP, &raddr.sin_addr);
	raddr.sin_port = htons(atoi(RCV_PORT));
	while (1) {
		bzero(&buf, MAXMSG);
		strncpy(buf.str, "hello", MAXMSG);
		sendto(udp_sd, &buf, strlen(buf.str) + 1, 0, (struct sockaddr *)&raddr, sizeof(raddr));
		sleep(1);
	}

	close(udp_sd);

	return 0;
ERROR:
	close(udp_sd);
	return 1;
}

