#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include <signal.h>

static  void signal_handler(int arg);
static char buff[12] = {0};
static int fd1;

int main(int argc, char* argv[])
{
	/* fd1:key */
	int old, i = 0;
	char buff[12] = {0};
	fd1 = open(argv[1], O_RDWR);
	
	if( (fd1 < 0) )
	{
		perror("open fail");
		return -1;
	}

	fcntl(fd1,  F_SETOWN,  getpid());
	old = fcntl(fd1, F_GETFL);
	fcntl(fd1, F_SETFL, old | FASYNC);
	signal( SIGIO, signal_handler);
	
	while(1)
	{
		printf("waiting for driver operate...\n");
		sleep(1);
	}

	return 0;
}

static  void signal_handler(int arg)
{
	memset(buff, 0, sizeof(buff));
	read(fd1, buff, sizeof(buff));
	if( !strcmp(buff, "KEY_S2"))
	{
		printf("---111---\n");
	}
	else if( !strcmp(buff, "KEY_S3"))
	{
		printf("---222---\n");
	}
	else if( !strcmp(buff, "KEY_S4") )
	{
		printf("---333---\n");
	}
}