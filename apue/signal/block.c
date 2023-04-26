/*
 俄罗斯方块图形自由下落
 1s下落一次,下落的代码间隔执行
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>

// 图形坐标
static int x = 1; 
static int y = 1;

// 图形
static int block[4][4] = {
	{1, 1, 0, 0},
	{0, 1, 0, 0},
	{0, 1, 0, 0},
	{0, 0, 0, 0}
};
/*
 [][]
   []
   []
 */

static void show();
static void sig_handler(int s);

int main(void)
{
	struct itimerval itv;

	// 想让sig_handler每秒钟调用一次
	signal(SIGALRM, sig_handler);
	// alarm(1);
	// 周期性定时
	/*
	 setitimer不仅仅能产生SIGALRM
	 时间精度更高,精确到微秒
	 周期性产生信号,it_value倒计时为0，it_interval赋值给it_value
	 */
	itv.it_interval.tv_sec = 0;
	itv.it_interval.tv_usec = 50000;
	itv.it_value.tv_sec = 3;
	itv.it_value.tv_usec = 0;
	setitimer(ITIMER_REAL, &itv, NULL);

	show();

	while (1) {
		// 读取用户游戏控制
		getchar();
	}

	exit(0);
}

/*
 信号处理函数:方块自由下落
	y++; 
 */
static void sig_handler(int s)
{
	// alarm(1);
	y ++ ;	
	show();
}

/*
 显示图形---数组打印
 */
static void show()
{
	int i, j;

	// 清屏
	printf("\033[2J");

	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			if (block[i][j]) {
				printf("\033[%d;%dH[]", y + i, x + j * 2);
			}
		}
	}
	fflush(NULL);
}



