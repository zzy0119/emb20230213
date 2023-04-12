#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <string.h>
#include <sys/msg.h>
#include <errno.h>

#include "msg_proto.h"

int main(int argc, char *argv[])
{
	key_t key;
	int msgid;
	msg_t buf;

	if (argc < 3)
	   exit(1);	

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

	// 发送
	buf.mtype = atol(argv[1]);
	strncpy(buf.str, argv[2], STRSIZE);
	msgsnd(msgid, &buf, strlen(argv[2]) + 1, 0);

	// 销毁
	// msgctl();

	return 0;
}

