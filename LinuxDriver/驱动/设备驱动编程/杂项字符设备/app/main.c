#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>


#define CMD_READ		_IOR('k', 0, int)
#define CMD_WRITE	_IOW('k', 1, char*)
#define CMD_PRINT	_IO('k', 2)


int main(int argc, char *argv[] )
{
	int fd;

	fd = open(argv[1], O_RDWR);
	if( fd < 0 )
	{
		printf("open fail!\n");
		return -1;
	}

	if( strcmp(argv[2], "read") == 0 ){
		int value = 0;
		ioctl(fd, CMD_READ,&value);
		printf("app_read: %d\n",value);
	}
	else if( strcmp(argv[2], "write") == 0 ){
		char buff[] = "APP:hello world!";
		ioctl(fd, CMD_WRITE, buff);
	}else if( strcmp(argv[2], "print") == 0 ){
		ioctl(fd,CMD_PRINT);
	}else{
		ioctl(fd,atoi(argv[2]));
	}
	
	close(fd);
	return 0;
}




