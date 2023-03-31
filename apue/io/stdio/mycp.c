/*
 实现cp命令
 ./mycp /etc/passwd ps
 将"/etcpasswd"文件复制到当前路径下叫ps
 思路:
 	argc >= 3
	读argv[1]---> "r"
	写argv[2] 如果argv[2]文件不存在创建 ---> "w"
 
 */
#include <stdio.h>
#include <string.h>

#define BUFSIZE	32

int mycp(FILE *rfp, FILE *wfp);
int main(int argc, char *argv[])
{
	FILE *fp1, *fp2;
	char buf[BUFSIZE] = {};

	if (argc < 3)
		return 1;

	// 打开原文件
	fp1 = fopen(argv[1], "r");
	if (NULL == fp1) {
		fprintf(stderr, "fopen() failed\n");	
		return 1;
	}
	// 打开目标文件
	fp2 = fopen(argv[2], "w");
	if (NULL == fp2) {
		fprintf(stderr, "fopen() failed\n");
		goto ERROR;
	}

	mycp(fp1, fp2);

	// fp1文件流指向最后
	// 获取文件流位置
	printf("copy后文件流的位置是:%ld\n", ftell(fp1));

	// 设置文件流指向开头
	rewind(fp1);

	// 改变文件流的位置
	fseek(fp1, 5, SEEK_SET);

	fgets(buf, BUFSIZE, fp1);
	puts(buf);

	fclose(fp1);
	fclose(fp2);

	return 0;
ERROR:
	fclose(fp1);
	return 1;
}

/*
 复制
 */
int mycp(FILE *rfp, FILE *wfp)
{
	char buf[BUFSIZE] = {};

	while (1) {
		memset(buf, 0, BUFSIZE);
		if (NULL == fgets(buf, BUFSIZE, rfp)) {
			if (ferror(rfp))
				return -1;
			break; // EOF
		}
		fputs(buf, wfp);	
	}

	return 0;
}



