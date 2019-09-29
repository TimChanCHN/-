#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
	/* fd1:key,fd2:led */
	int fd1, value = 0;
	char buff[12] = {0};
	fd1 = open(argv[1], O_RDWR);
	
	if( (fd1 < 0) )
	{
		perror("open fail");
		return -1;
	}

	while(1)
	{
		memset(buff, 0, sizeof(buff));
		read(fd1, buff, sizeof(buff));
		printf("---key:%s is pressed---\n",buff);

	}

	return 0;
}
