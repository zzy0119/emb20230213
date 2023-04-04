#define _XOPEN_SOURCE       /* See feature_test_macros(7) */
#include <unistd.h>
#include <stdio.h>
#include <unistd.h>
#include <shadow.h>
#include <string.h>

int main(int argc, char *argv[])
{
	struct spwd *sp = NULL;
	char *encrypted;
	// 校验argv[1]密码
	if (argc < 2)
		return 1;

	// 读密码
	char *pwd_str = getpass("密码:");

	// 校验密码-->获取argv[1]用户的密码序列串
	sp = getspnam(argv[1]);
	// sp->sp_pwdp argv[1]用户加密后的密码 
	// 加密
	encrypted = crypt(pwd_str, sp->sp_pwdp);

	// 比对
	if (!strcmp(encrypted, sp->sp_pwdp)) {
		printf("密码正确\n");
	} else
		printf("鉴定错误\n");

	return 0;
}




