#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
	int fd;

	fd = open(argv[1], O_RDONLY);
	if( fd < 0 )
	{
		perror("open file fail!");
		return -1;
	}

	struct  input_event ts_val;

	while(1)
	{
		memset(&ts_val, 0, sizeof(ts_val));
		read(fd, &ts_val, sizeof(ts_val));
		printf("***type:%d  code:%d  value:%d***\n", ts_val.type, ts_val.code, ts_val.value);
	}
}
