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
