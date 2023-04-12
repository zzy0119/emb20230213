#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>

#include "msg_proto.h"

int main(void)
{
	key_t key;
	int msgid;
	msg_t buf;

	// 生成key
	key = ftok(PATHNAME, PROT_ID);
	if (-1 == key) {
		perror("ftok()");
		exit(1);
	}

	// 创建消息队列
	msgid = msgget(key, IPC_CREAT | IPC_EXCL | 0600);
	if (-1 == msgid) {
		if (EEXIST == errno) {
			// 说明已经存在
			msgid = msgget(key, 0);
		} else {
			perror("msgget()");
			exit(1);
		}
	}

	while (1) {
		if (-1 == msgrcv(msgid, &buf, STRSIZE, 3, 0)) {
			perror("msgrcv()");
			exit(1);
		}
		printf("rcv data id:%ld, str:%s\n", buf.mtype, buf.str);
	}

	// 销毁
	// msgctl();

	return 0;
}

