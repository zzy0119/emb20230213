#include <stdio.h>
#include <setjmp.h>

jmp_buf env;

int sub2num(int a, int b);

int main(void)
{
	int num1, num2;
	int res;

	// 如果除数为0,要跳转的位置
	if (0 == setjmp(env))
		// 第一次调用，非跳转来的
		printf("请输入两个整型数:");
	else
		// 不是0,则一定是跳转来的
		printf("请重新输入:");
	scanf("%d%d", &num1, &num2);

	res = sub2num(num1, num2);

	printf("%d / %d = %d\n", num1, num2, res);

	return 0;
}

/*
 相除
 */
int sub2num(int a, int b)
{
	if (0 == b) {
		// 跳转到读入b位置
		longjmp(env, 1);
	}

	return a / b;
}



