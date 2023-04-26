#include <stdio.h>
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
	struct sockaddr_in laddr;
	struct ip_mreqn imr;
	struct msg_st buf;

	udp_sd = socket(AF_INET, SOCK_DGRAM, 0);
	if (-1 == udp_sd) {
		perror("socket()");
		exit(1);
	}

	laddr.sin_family = AF_INET;
	inet_aton(GROUP_IP, &laddr.sin_addr);
	laddr.sin_port = htons(atoi(RCV_PORT));
	if (-1 == bind(udp_sd, (struct sockaddr *)&laddr, sizeof(laddr))) {
		perror("bind()");
		goto ERROR;
	}

	// 加入多播组
	inet_aton(GROUP_IP, &imr.imr_multiaddr); // 多播地址
	inet_aton("0.0.0.0", &imr.imr_address); // 本地地址
	imr.imr_ifindex = if_nametoindex("ens33"); // "ens33"转换为设备号
	if (-1 == setsockopt(udp_sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &imr, sizeof(imr))) {
		perror("setsockopt()");
		goto ERROR;
	}

	bzero(&buf, MAXMSG);
	recvfrom(udp_sd, &buf, MAXMSG, 0, NULL, NULL);
	puts(buf.str);

	close(udp_sd);

	return 0;
ERROR:
	close(udp_sd);
	return 1;
}

