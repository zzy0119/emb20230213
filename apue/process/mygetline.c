#include <stdio.h>
#include <stdlib.h>

#define STEP 10	

size_t mygetline(char **lineptr, size_t *n, FILE *fp);

int main(void)
{
	char *line = NULL;
	size_t n = 0;

	while (1) {
		if (-1 == mygetline(&line, &n, stdin))
			break;
		printf("存储空间大小n:%ld, line:%s\n", n, line);
	}

	return 0;
}

size_t mygetline(char **lineptr, size_t *n, FILE *fp)
{	
	int i;

	if (*lineptr == NULL && *n == 0) {
		*lineptr = malloc(STEP * sizeof(char));
		if (NULL == *lineptr)
			return -1;
		*n = STEP;
	}
	// 一定有存储空间 读文件一行
	i = 0;
	while (1) {
		// 判断空间是否足够	
		if (i == *n - 1) {
			// 扩容
			*n += STEP;
			*lineptr = realloc(*lineptr, *n * sizeof(char));
			// if error
		}
		(*lineptr)[i] = fgetc(fp);
		if ('\n' == (*lineptr)[i]) {
			// 读完一行
			break;
		}
		if (EOF == (*lineptr)[i]) {
			(*lineptr)[i+1] = '\0';
			return -1;
		}
		i++;
	}
	// 放null-terminal
	(*lineptr)[i+1] = '\0';

	return i+1;
}

