#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <poll.h>

void sendpidtodevice(int fd)
{
	fcntl(fd, F_SETOWN, getpid());
	old = fcntl(fd, F_GETFL);
	fcntl(fd, F_SETFL, old| FASYNC);
}

void fasync_handler(int arg)
{
	printf("***fasync***\n");
}


int main(int argc, char* argv[])
{
	int fd,old;
	char buff[32] = {0};
	fd = open(argv[1], O_RDWR);
	if( fd < 0 )
	{
		perror("open failed!");
		return -1;
	}

	sendpidtodevice(fd);
	signal(SIGIO, fasync_handler);
	while(1)
	{
		printf("---main job---\n");
		sleep(1);
	}

	
	return 0;
}
