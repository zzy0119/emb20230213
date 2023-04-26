#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>          /* See NOTES */

#include "unix_proto.h"

int main(void)
{
	int sd, new_sd;
	struct sockaddr_un local_addr;
	char buf[MAXMSG] = {};
	int cnt;

	sd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (-1 == sd) {
		perror("socket");
		exit(1);
	}

	local_addr.sun_family = AF_UNIX;
	strncpy(local_addr.sun_path, PATH, 108);
	if (-1 == bind(sd, (struct sockaddr *)&local_addr, sizeof(local_addr))) {
		perror("bind()");
		goto ERROR;
	}

	listen(sd, 5);

	new_sd = accept(sd, NULL, NULL);
	if (-1 == new_sd) {
		perror("accept()");
		goto ERROR;
	}
	while (1) {
		cnt = read(new_sd, buf, MAXMSG);	
		if (-1 == cnt) {
			perror("read()");
			goto ERROR;
		}
		if (0 == cnt)
			break;
		write(1, buf, cnt);
	}

	close(new_sd);
	close(sd);

	return 0;
ERROR:
	close(sd);
	exit(1);
}

