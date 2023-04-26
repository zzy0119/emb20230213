#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>

#include "media_lib.h"
#include "../../proto/proto.h"

extern int server_sd;

static void *thr_job(void *s);
int thr_data_create(chnid_t chnid)
{
	pthread_t tid;
	int err = pthread_create(&tid, NULL, thr_job, (void *)chnid);
	if (err) {
		fprintf(stderr, "pthread_create():%s\n", strerror(err));
		return -1;
	}
}

static void *thr_job(void *s)
{
	// 向媒体库模块请求数据 向多播组发送数据包
	struct chn_data *snd_data;
	chnid_t mychnid = (chnid_t)s;
	ssize_t get_size;
	struct sockaddr_in group_addr;

	snd_data = malloc(MAX_MSG-1); // chnid减掉
	if (NULL == data) {
		pthread_exit(1);
	}

	snd_data->chnid = mychnid;

	group_addr.sin_family = AF_INET;
	group_addr.sin_port = htons(atoi(RCV_PORT));
	inet_aton(RADIO_GROUP, &group_addr.sin_addr);
	while (1) {
		get_size = mlib_read_chn_data(mychnid, snd_data->data, MAX_MSG-1);
		if (-1 == get_size) {
			fprintf(stderr, "mlib_read_chn_data() failed\n");
			pthread_exit(1);
		}
		sendto(server_sd, snd_data, get_size + 1, 0, (void *)&group_addr, sizeof(group_addr));
		// 流量控制
		sleep(1);
	}


}

