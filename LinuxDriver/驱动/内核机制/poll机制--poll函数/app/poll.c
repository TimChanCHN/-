#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>

int main(int argc, char* argv[])
{
	int fd = 0;
	fd = open(argv[1],O_RDWR);
	if( fd < 0 )
	{
		perror("open failed!");
		return -1;
	}

	struct pollfd  fds[1]={
		{fd, POLLIN, 0},
	};


	char buf[32] = {0};
	int i = 0;
	while(1)
	{
		poll(fds,  sizeof(fds)/sizeof(fds[0]), -1);
		for( i = 0; i < sizeof(fds)/sizeof(fds[0]); i++ )
		{
			if( fds[i].revents )  
			{	
				memset(buf,0,sizeof(buf));
				read(fds[i].fd,buf,sizeof(buf));
				printf("---%s is pressed---\n",buf);
			}
		}
	}

}
