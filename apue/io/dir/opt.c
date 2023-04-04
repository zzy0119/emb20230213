#include <stdio.h>
#include <getopt.h>
#include <unistd.h>

void opt_n(void);
int main(int argc, char *argv[])
{
	int c;
	int val;
	struct option opts[] = {
		{"help", no_argument, &val, 'h'}, 
		{"list", no_argument, NULL, 'l'},
		{0, 0, 0, 0}
	};
	
	while (1) {
// 		c = getopt(argc, argv, "-an:h::");
 		c = getopt_long(argc, argv, "-an:h::", opts, NULL);
		if (-1 == c)
			break;
		switch (c) {
			case 0:
				if (val == 'h') {
					printf("长格式选项help...\n");
				}
				break;
			case 'l':
				printf("长格式选项list...\n");
				break;
			case 'a':
				printf("请将-a要实现的功能在这里调用\n");	
				break;
			case 'n':
				opt_n();
				// 带选项的参数n:
				printf("required argument:%s\n", optarg);
				break;
			case 'h':
				// 可选参数 h::
				printf("请将-h要实现的功能在这里调用\n");	
				if (optarg) {
					printf("可选参数:%s\n", optarg);
				}
				break;
			case 1:
				// 非选项参数 --->optstring以'-'开头
				printf("解析到了非选项参数:%s\n", argv[optind-1]);
				break;
			default:
				// ?
				break;
		}
	}

	return 0;
}

/*
 解析到选项'-n'调用的接口
 */
void opt_n(void)
{
	printf("getopt -n...请调用对应功能\n");
}




