#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
	int fd = 0;
	fd = open(argv[1],O_RDWR);
	if( fd < 0 )
	{
		perror("open failed!");
		return -1;
	}
		
	fd_set readfds;
	FD_ZERO(&readfds);
	FD_SET(fd, &readfds);
		
	fd_set tmpfds;


	char buf[32] = {0};
	while(1)
	{
		tmpfds = readfds;
		select(fd+1, &tmpfds, NULL, NULL, NULL);	
		if( FD_ISSET(fd, &tmpfds))  
		{	
			memset(buf,0,sizeof(buf));
			read(fd,buf,sizeof(buf));
			printf("---%s is pressed---\n",buf);
		}
	}

}
