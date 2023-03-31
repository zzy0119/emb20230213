/*
 实现一个cat功能
 即将argv[1]文件读出来写到标准输出
 */

#include <stdio.h>
#include <string.h>

#define BUFSIZE	32

int main(int argc, char *argv[])
{
	// 文件流
	FILE *fp = NULL;
	int c;
	char buf[BUFSIZE] = {};

	if (argc < 2) {
		return 1;
	}

	// 打开文件--->文件流指向文件开头
	fp = fopen(argv[1], "r");
	if (NULL == fp) {
		return 1;
	}

	// 读所打开的文件 写标准输出
	while (1) {
		// 第一次读读取第一个字符,当读完了，文件流自然会向后偏移
		// 初始化buf
		memset(buf, 0, BUFSIZE);
		if (NULL == fgets(buf, BUFSIZE, fp)) {
			if (ferror(fp)) {
				// 出错
				fprintf(stderr, "fgetc() error\n");
				goto ERROR;
			}
			break;
		}
		// stdout就是标准输出文件的文件流 stdin标准输入文件的文件流 stderr 标准错误输出文件的文件流
		fputs(buf, stdout);
	}

	// 关闭
	fclose(fp);
	fp = NULL;

	return 0;
ERROR:
	fclose(fp);
	return 1;
}

