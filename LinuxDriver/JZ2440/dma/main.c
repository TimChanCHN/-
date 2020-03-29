#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define CP_NO_DMA		'K'
#define MEM_COPY_NO_DMA	 _IOR(CP_NO_DMA, 0, char)
#define MEM_COPY_DMA	 _IOR(CP_NO_DMA, 1, char)

int main(int argc, char* argv[])
{
	int fd;

	fd = open("/dev/s3c_dma", O_RDWR);
	if( fd < 0 )
	{
		perror("open file fail!");
		return -1;
	}

	if( strcmp(argv[1],"nodma") == 0 )
	{
		while(1)
		{
			ioctl(fd, MEM_COPY_NO_DMA);
		}
	}
	if( strcmp(argv[1],"dma") == 0 )
	{
		while(1)
		{
			ioctl(fd, MEM_COPY_DMA);
		}
	}
	else
	{
		printf("cmd error!\n");
	}
}
