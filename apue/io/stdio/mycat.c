/*
 实现一个cat功能
 即将argv[1]文件读出来写到标准输出
 */

#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	// 文件流
	FILE *fp = NULL;
	int c;

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
		c = fgetc(fp);
		if (EOF == c) {
			// 读到EOF或者出错
			if (ferror(fp)) {
				// 出错
				fprintf(stderr, "fgetc() error\n");
				goto ERROR;
			}
			break;
		}
		// stdout就是标准输出文件的文件流 stdin标准输入文件的文件流 stderr 标准错误输出文件的文件流
		// fputc(c, stdout);
		/*
		 缓存:标准io在打开文件的时候会创建缓存区，默认4k
		 缓存方式:
		 	行缓存:stdin stdout
			全缓存:
			无缓存:stderr
		刷新缓存方式:
			1.行缓存遇到'\n'
			2.进程正常终止(main return)
			3.fflush(3)强制刷新
		 */
		fputc(c, stdout);
		fflush(NULL); // 刷新所有的输出流
		sleep(1);
	}

	// 关闭
	fclose(fp);

	return 0;
ERROR:
	fclose(fp);
	return 1;
}

