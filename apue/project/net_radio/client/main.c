#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <net/if.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#include "../proto/proto.h"

int main(void)
{
	int udp_sd;
	struct sockaddr_in laddr;
	struct ip_mreqn imr;
	msg_t *msg;
	struct chn_list *mylist;
	int size;
	struct list_entry_st *entry;
	int select;

	udp_sd = socket(AF_INET, SOCK_DGRAM, 0);
	if (-1 == udp_sd) {
		perror("socket()");
		exit(1);
	}

	laddr.sin_family = AF_INET;
	inet_aton("0.0.0.0", &laddr.sin_addr);
	laddr.sin_port = htons(atoi(RCV_PORT));
	if (-1 == bind(udp_sd, (struct sockaddr *)&laddr, sizeof(laddr))) {
		perror("bind()");
		goto ERROR;
	}

	// 加入多播组
	inet_aton(RADIO_GROUP, &imr.imr_multiaddr); // 多播地址
	inet_aton("0.0.0.0", &imr.imr_address); // 本地地址
	imr.imr_ifindex = if_nametoindex("ens33"); // "ens33"转换为设备号
	if (-1 == setsockopt(udp_sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &imr, sizeof(imr))) {
		perror("setsockopt()");
		goto ERROR;
	}

	// 接受频道列表
	msg = malloc(MAX_MSG);
	if (NULL == msg) {
		perror("malloc()");
		goto ERROR;
	}
	while (1) {
		memset(msg, 0, MAX_MSG);
		size = recvfrom(udp_sd, msg, MAX_MSG, 0, NULL, NULL);
		if (-1 == size) {
			perror("recvfrom()");
			goto ERROR;
		}
		if (CHN_LIST_ID == msg->chnid)
			break;
	}
	// 打印列表
	mylist = (struct chn_list *)msg;
		
	for (entry = (void *)((char *)mylist + 1); (void *)entry < (void *)((char *)mylist + size - 1); \
			entry = (void *)((char *)entry + entry->len)) {
		printf("%d %s\n", entry->chnid, entry->descr);
	}
	printf("请选择你要收听的频道:");
	scanf("%d", &select);
	
	// 创建管道--->fork子进程--->父进程读多播组接受select频道数据写管道，子进程运行播放器，读管道


	close(udp_sd);

	return 0;
ERROR:
	close(udp_sd);
	return 1;
}

