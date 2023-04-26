#ifndef __THR_LIST_H__
#define __THR_LIST_H__

#include "media_lib.h"

/*
arr:存储频道列表数组的首地址 
n:频道个数
return:是否成功
 */
int thr_list_create(mlib_t *arr, int n);

#endif

