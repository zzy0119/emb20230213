int fd;

void *thr1(void *s) 
{
	lseek(fd, 5, SEEK_CUR);	
	read(fd, buf, BUFSIZE);
	// 原子
	pread(fd, buf, BUFSIZE, 5);
}

void *thr2(void *s) 
{
	lseek(fd, 5, SEEK_CUR);	
	read(fd, buf, BUFSIZE);
}

thr1--->lseek()--->thr2-->lseek()
pos==>10
read()20字节;
thr1--->read() pos==>30
 
