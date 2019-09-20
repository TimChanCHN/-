#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>


void sig_handler(int arg)
{
	printf("--- this is fasync signal handler---\n");
	sleep(1);
}

int main(int argc, char* argv[])
{
	int fd;
	fd = open(argv[1], O_RDWR);
	if( fd < 0 )
	{
		perror("open failed!");
		return -1;
	}

	/* fasync function  */
	int old;
	fcntl(fd, F_SETOWN, getpid());
	old = fcntl(fd, F_GETFL);
	fcntl(fd, F_SETFL, old| FASYNC );		//自动调用设备驱动中的fasync()函数
	/* SIGIO信号自定义处理  */
	signal(SIGIO, sig_handler);


	/* 进程的自由操作  */

	while(1)
	{
		printf("---main job --- \n ");
		sleep(1);
	}


	return 0;
}
