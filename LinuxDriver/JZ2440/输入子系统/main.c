#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <linux/input.h>

int main(int argc, char* argv[])
{
	/* fd1:key,fd2:led */
	int fd1, value = 0;
	char buff[12] = {0};
	struct  input_event keyvalue;

	
	fd1 = open(argv[1], O_RDWR);
	
	if( (fd1 < 0) )
	{
		perror("open fail");
		return -1;
	}

	while(1)
	{
		memset(&keyvalue, 0, sizeof(keyvalue));
		read(fd1, &keyvalue, sizeof(keyvalue));
		printf("---type:%d, code:%d, value:%d---\n",keyvalue.type, keyvalue.code, keyvalue.value);
	}

	close(fd1);
	return 0;
}
