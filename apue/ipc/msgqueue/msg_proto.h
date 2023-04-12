#ifndef __MSG_PROTO_H
#define __MSG_PROTO_H

// 约定要通信两个进程的协议(key 数据类型)
#define PATHNAME	"/etc/passwd"
#define PROT_ID		'A'

#define STRSIZE		32

typedef struct {
	long mtype; // 必须有 >0
	char str[STRSIZE];
}msg_t;

#endif

