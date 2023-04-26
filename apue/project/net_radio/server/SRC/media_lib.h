#ifndef __MEDIA_LIB_H
#define __MEDIA_LIB_H

#include <stdint.h>

typedef int8_t chnid_t;

typedef struct {
	chnid_t chnid;	
	char *descr;
}mlib_t;

// 解析频道列表
/*
*arr:存储频道列表结构数组的起始地址,所以arr是存储地址对应的指针变量的地址
n:回填数组的成员个数
return:是否成功的状态 0成功
 */
int mlib_get_chn_list(mlib_t **arr, int *n);

/*
chnid:频道号
buf:存储读到的数据的存储空间
size:要读取的字节个数
return:读到的字节个数
 */
ssize_t mlib_read_chn_data(chnid_t chnid, void *buf, size_t size);

#endif

