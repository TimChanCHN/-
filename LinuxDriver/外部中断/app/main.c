#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(int argc, char* argv[])
{
	int fd;
	char buff[32] = {0};
	fd = open(argv[1], O_RDWR);
	if( fd < 0 )
	{
		perror("open fail!")
		return -1;
	}

	while(1)
	{
		memset(buff, 0, sizeof(buff));
		read(fd, buff, sizeof(buff));
		printf("---%s is pressed---\n",buff);
	}

	close(fd);
	return 0;
}
