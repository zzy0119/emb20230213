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

// 外部声明
extern int server_sd;

typedef struct {
	mlib_t *arr;
	int n;
}thread_list;

static void *thr_job(void *s);
int thr_list_create(mlib_t *arr, int n)
{
	pthread_t tid;
	int err;

	thread_list *chn_list_data = malloc(sizeof(thread_list));;
	chn_list_data->arr = arr;
	chn_list_data->n = n;
	err = pthread_create(&tid, NULL, thr_job, chn_list_data);
	if (err) {
		fprintf(stderr, "pthread_create():%s\n", strerror(err));
		return -1;
	}

	return 0;
}

static void *thr_job(void *s)
{
	thread_list *mylist = s;
	int len;
	struct chn_list *snd;
	struct list_entry_st *entry;
	struct sockaddr_in group_addr;
	int i;

	// 列表包大小
	len = sizeof(chnid_t); // 0
	for (int i = 0; i < mylist->n; i++) {
		len += sizeof(struct list_entry_st) + strlen((mylist->arr)[i].descr);
	}
	snd = malloc(len);
	snd->chnid = CHN_LIST_ID;
	for (i = 0, entry = (void *)((char *)snd + 1); i < mylist->n; i++) {
		entry->chnid = (mylist->arr)[i].chnid;	
		strcpy(entry->descr, (mylist->arr)[i].descr);
		entry->len = sizeof(struct list_entry_st) + strlen(entry->descr);
		entry = (void *)((char *)entry + entry->len);
	}

	group_addr.sin_family = AF_INET;
	group_addr.sin_port = htons(atoi(RCV_PORT));
	inet_aton(RADIO_GROUP, &group_addr.sin_addr);
	while (1) {
		sendto(server_sd, snd, len, 0, (struct sockaddr *)&group_addr, sizeof(group_addr));
		sleep(1);
	}

}


