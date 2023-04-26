#ifndef __PROTO_H__
#define __PROTO_H__

#define RADIO_GROUP 	"224.5.5.5"
#define RCV_PORT		"5566"
#define CHN_NR			200
#define MIN_CHNID		1
#define MAX_CHNID		((CHN_NR) - (MIN_CHNID) + 1)
#define CHN_LIST_ID		0
#define CHN_DATA_SIZE	256
#define MAX_MSG			512

// #pragma pack(1)

typedef int8_t chnid_t;
typedef int16_t	len_t;

// 频道列表
// 单个频道
struct list_entry_st {
	chnid_t chnid;
	len_t len; // 自述长度
	char descr[1]; // 频道描述
}__attribute__((packed));

// 整个列表
struct chn_list{
	chnid_t chnid; // 必须是CHN_LIST_ID
	struct list_entry_st list[1]; // 变长结构体
} __attribute__((packed));

// 频道数据
struct chn_data {
	chnid_t chnid; /*MIN_CHNID ~ MAX_CHNID*/
	char data[1]; // 最大CHN_DATA_SIZE
}__attribute__((packed));

// 
typedef union {
	chnid_t chnid;
	struct chn_list list_msg;
	struct chn_data data_msg;
}msg_t;

#endif

