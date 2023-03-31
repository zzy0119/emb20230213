/*
将argv[1]文件倒序写入argv[2] 
argv[1]--->"abc"
argv[2]--->"cba"

分析:
	argv[1]---"r"
	fseek()-->文件流定位到末尾--->文件字节大小(ftell(3))size
	fseek(fp, size-1, SEEK_SET);
	fgetc()
	fseek(fp, size-2, SEEK_SET);
 */
#include <stdio.h>

int reverse_file(FILE *src_fp, FILE *dest_fp);
int main(int argc, char *argv[])
{
	FILE *fp1, *fp2;

	if (argc < 3) {
		fprintf(stderr, "Usage....\n");
		return 1;
	}

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

	reverse_file(fp1, fp2);

	fclose(fp1);
	fclose(fp2);

	return 0;
ERROR:
	fclose(fp1);
	return 1;
}

/*
 倒序
 */
int reverse_file(FILE *src_fp, FILE *dest_fp)
{
	int size;
	int i;
	int c;

	// 文件流定位到末尾
	fseek(src_fp, 0, SEEK_END);
	// 文件的字节大小
	size = ftell(src_fp);	

	// dest_fp文件流设置在开头
	rewind(dest_fp);

	i = 1;
	while (i <= size) {
		fseek(src_fp, -i, SEEK_END);	
		c = fgetc(src_fp);
		if (EOF == c) {
			if (ferror(src_fp))
				return -1;
			break;
		}
		fputc(c, dest_fp);
		i++;
	}

	return 0;
}






