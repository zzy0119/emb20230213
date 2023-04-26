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
	int sd;
	struct sockaddr_un remote_addr;

	sd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (-1 == sd) {
		perror("socket");
		exit(1);
	}

	remote_addr.sun_family = AF_UNIX;
	strncpy(remote_addr.sun_path, PATH, 108);
	connect(sd, (struct sockaddr *)&remote_addr, sizeof(remote_addr));

	write(sd, "hello world", 11);

	close(sd);

	return 0;
ERROR:
	close(sd);
	exit(1);
}

