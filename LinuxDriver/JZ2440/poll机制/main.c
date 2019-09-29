#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>

int main(int argc, char* argv[])
{
	/* fd1:key */
	int fd1, fd2, fd3, i = 0;
	char buff[12] = {0};
	fd1 = open(argv[1], O_RDWR);
	fd2 = open(argv[2], O_RDWR);
	fd3 = open(argv[3], O_RDWR);
	
	if( (fd1 < 0)|| (fd2 < 0) || (fd3 < 0)  )
	{
		perror("open fail");
		return -1;
	}

	struct pollfd arrevent[]={
		[0] = {fd1, POLLIN, 0},
		[1] = {fd2, POLLIN, 0},
		[2] = {fd3, POLLIN, 0},
	};
	while(1)
	{
		poll(arrevent,  sizeof(arrevent)/sizeof(arrevent[0]),  -1);
		for( i = 0; i < 3; i++ )
		{
			if( arrevent[i].revents)
			{
				memset(buff, 0, sizeof(buff));
				read(arrevent[i].fd, buff, sizeof(buff));
				printf("---%s is pressed---\n", buff);
			}
		}
	}

	return 0;
}
