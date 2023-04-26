#ifndef __UDP_PROTO_H
#define __UDP_PROTO_H

// 被动端地址
#define SERVER_IP	"192.168.199.128"
#define SERVER_PORT	1111

#define NAMESIZE	32

// 数据类型
struct msg_st {
	int8_t id; // 明确数据类型
	char name[NAMESIZE];
}__attribute__((packed)); // 不对齐

#endif

