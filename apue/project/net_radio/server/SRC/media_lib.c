#include <glob.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "media_lib.h"
#include "../../proto/proto.h"

#define MEDIA_LIB_PATH	"/home/zhangzongyan/media_lib"
#define DESCR_FILE_NAME	"descr.txt"
#define MP3_NR			200
#define BUFSIZE			128

// 内部接口:解析整个媒体库目录
struct chn_context_st {
	// 列表
	chnid_t chnid;
	char *descr;
	// 数据
	// const char *mp3_path[MP3_NR];// 当前频道下所有mp3文件的路径
	/*
	char **mp3_path; // glob(3)
	int mp3_nr;
	*/
	glob_t mp3_path;
	int fd; // 当前所读的文件的文件描述符	
	int pos; // 文件偏移量
	int cur_mp3_index; // 正在读的音乐文件所在mp3_path数组的下标值
};

static struct chn_context_st *all_chns[CHN_NR+1];
static int sum_chn_nr;

static int __mlib_parse(void);

int mlib_get_chn_list(mlib_t **arr, int *n)
{
	int i = 0, j = 0;
	// 所有频道结构已经读取
	__mlib_parse();

	*arr = calloc(sum_chn_nr, sizeof(mlib_t));
	*n = sum_chn_nr;

	for (i = 0, j = 1; j <= sum_chn_nr; i++, j++) {
		(*arr)[i].chnid = all_chns[j]->chnid;
		(*arr)[i].descr = strdup(all_chns[j]->descr);
	}

	return 0;
}

static int __open_next(chnid_t chnid)
{
	// 关闭之前的
	close((all_chns[chnid]->mp3_path).gl_pathv[all_chns[chnid]->cur_mp3_index]);
	// 得到下一个mp3文件的下标值
	all_chns[chnid]->cur_mp3_index ++;
	if (all_chns[chnid]->cur_mp3_index > sum_chn_nr)
		all_chns[chnid]->cur_mp3_index = 1;
	// 打开下一个文件
	all_chns[chnid]->fd = open((all_chns[chnid]->mp3_path).gl_pathv[all_chns[chnid]->cur_mp3_index], O_RDONLY);
	if (-1 == all_chns[chnid]->fd) {
		perror("open()");
		return -1;
	}
	return 0;
}

ssize_t mlib_read_chn_data(chnid_t chnid, void *buf, size_t size)
{
	/*
		all_chns[chnid]->fd
		读完了-->读下一个 cur_mp3_index++ 关闭当前fd fd=open()下一个文件
	 */
	ssize_t ret;

	while (1) {
		ret = read(all_chns[chnid]->fd, buf, size);
		if (-1 == ret) {
			perror("read()");
			return -1;
		}
		if (0 == ret) {
			// 读下一个mp3文件
			__open_next(chnid);
		} else
			break;
	}

	return ret;
}



/*
 解析某一个频道目录
 */
// "~/media_lib/music/"
static struct chn_context_st *__chn_parse(const char *path)
{
	struct chn_context_st *mychn;	
	static int cur_chn_id = MIN_CHNID;
	char buf[BUFSIZE] = {};
	FILE *fp;
	size_t size = 0;

	// 描述文件	
	strncpy(buf, path, BUFSIZE); // 频道路径
	strncat(buf, "/", BUFSIZE - strlen(buf));
	strncat(buf, DESCR_FILE_NAME, BUFSIZE - strlen(buf)); // 描述文件路径
	fp = fopen(buf, "r");
	if (NULL == fp) {
		return NULL;
	}

	// 开辟
	mychn = malloc(sizeof(struct chn_context_st));
	if (NULL == mychn) {
		return NULL;
	}
	if (-1 == getline(&mychn->descr, &size, fp)) {
		goto ERROR;
	}
	
	// 读所有的音乐文件 如果解析的是一类(同一个后缀名) glob更容易
	memset(buf, 0, BUFSIZE);
	strncpy(buf, path, BUFSIZE);
	strncat(buf, "/*.mp3", BUFSIZE - strlen(buf));
	// "~/media_lib/music/*.mp3"--->pattern
	// 所有与pattern匹配的路径存储到(mychn->mp3_path).gl_pathv指针数组中, gl_pathc
	if (0 != glob(buf, 0, NULL, &mychn->mp3_path))
		goto ERROR;
	mychn->cur_mp3_index = 0;
	mychn->pos = 0;
	// 打开第一个文件，以便于mlib_read_chn_data直接读
	mychn->fd = open((mychn->mp3_path.gl_pathv)[0], O_RDONLY);
	if (-1 == mychn->fd) {
		perror("open()");
		goto ERROR;
	}

	mychn->chnid = cur_chn_id ++;

	return mychn;
ERROR:
	free(mychn);
	return NULL;
}

/*
 解析整个媒体库目录
 */
static int __mlib_parse(void)
{
	DIR *dp;
	struct dirent *entry;
	char buf[BUFSIZE] = {};
	struct chn_context_st *ret;

	dp = opendir(MEDIA_LIB_PATH);
	if (NULL == dp) {
		perror("opendir()");
		return -1;
	}

	while (1) {
		errno = 0;
		entry = readdir(dp);	
		if (NULL == entry) {
			if (errno) {
				perror("readdir()");
				goto ERROR;
			}
			break;
		}
	
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
			continue;

		// child opera music traffic
		memset(buf, 0, BUFSIZE);
		strncpy(buf, MEDIA_LIB_PATH, BUFSIZE);
		strncat(buf, "/", BUFSIZE - strlen(buf));
		strncat(buf, entry->d_name, BUFSIZE - strlen(buf));
		ret = __chn_parse(buf);
		if (NULL == ret)
			continue; // 此频道不是有效频道
		all_chns[ret->chnid] = ret;
		sum_chn_nr++;
	}

	closedir(dp);
	return 0;
ERROR:
	closedir(dp);
	return -1;
}

