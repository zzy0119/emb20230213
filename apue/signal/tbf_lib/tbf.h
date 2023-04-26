/*
 实现流量控制库:
 	最多支持令牌桶个数为1024--->合理的数据结构(数组)
 ADT:
 	struct tbf_st {
		int token;
		int cps;
		int burst;
	};
	struct tbf_st *jobs[1024];
 functions:
	// 初始化令牌桶
 	tbf_init();
	// 取令牌 积攒令牌不是用户调用接口--->信号处理函数
	tbf_fetch_token();
	// 释放
	tbf_destroy();
 */
#ifndef __TBF_H
#define __TBF_H

#define TBF_MAX	1024

/******************************
* 初始化令牌桶
* cps:令牌桶速率
* burst:容量
* return:桶标识---桶描述符
*******************************/
int tbf_init(int cps, int burst);

/******************************
 * 从指定令牌桶取令牌
 * td: 桶描述符
 * n: 取的个数
 * return: 取到的个数
 ****************************/
int tbf_fetch_token(int td, int n);

/**************************
 * 销毁指定令牌桶
 * td: 桶描述符
 **************************/
int tbf_destroy(int td);

/**************************
 * 销毁所有的令牌桶
 **************************/
void tbf_destroy_all();

#endif

