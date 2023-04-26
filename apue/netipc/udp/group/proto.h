#ifndef __PROTO_H
#define __PROTO_H

#define GROUP_IP	"224.2.3.4"
#define RCV_PORT	"2234"

#define MAXMSG		1024

struct msg_st {
	char str[MAXMSG];
};

#endif

