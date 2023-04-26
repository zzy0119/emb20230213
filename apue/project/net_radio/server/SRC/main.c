#include <stdio.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <net/if.h>

#include "media_lib.h"
#include "thr_list.h"
#include "thr_chn_data.h"
#include "../../proto/proto.h"

int server_sd;

static int socket_init();
int main(void)
{
	mlib_t *mylist = NULL;
	int chn_nr = 0;

	// 初始化套接字
	socket_init();

	// 请求频道列表
	mlib_get_chn_list(&mylist, &chn_nr);

	for (int i = 0; i < chn_nr; i++) {
		printf("%d %s\n", mylist[i].chnid, mylist[i].descr);
	}

	// 启动发送频道列表的线程 线程分离pthread_detach()
	thr_list_create(mylist, chn_nr);

#if 0
	// 启动所有发送频道数据的线程
	for (i = 0; i < chn_nr; i++) {
		thr_data_create(mylist[i].chnid);
	}
#endif

	pthread_exit(0);	
}

static int socket_init()
{
	struct ip_mreqn imr;

	server_sd = socket(AF_INET, SOCK_DGRAM, 0);
	if (-1 == server_sd) {
		perror("socket()");
		return -1;
	}

	// 设置多播选项
	// 将本地设备用于多播
	inet_aton(RADIO_GROUP, &imr.imr_multiaddr); // 多播地址
	inet_aton("0.0.0.0", &imr.imr_address); // 本地地址
	imr.imr_ifindex = if_nametoindex("ens33"); // "ens33"转换为设备号
	if (-1 == setsockopt(server_sd, IPPROTO_IP, IP_MULTICAST_IF, &imr, sizeof(imr))) {
		perror("setsockopt()");
		return -1;
	}

	return 0;
}



