#include "lcd.h"
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <linux/input.h>

#define MIDDLELINE 400

extern int fd_dev;
extern char* fb_start;

int main(int argc, char* argv[])
{
	int cnt = 0, len, len_dev, fd; 			// 统计.jpg文件个数
	DIR* dirp = NULL;
	char buff[100] = {0}, dir[100] = {0};	//存储文件个数
	char picname[10][100] = {0};
	
	struct dirent *filp = NULL;
	struct input_event keyval;

	len_dev = GetFbStart("/dev/fb0");
	fd = open("/dev/input/event0", O_RDWR);
	dirp = opendir(argv[1]);
	if( dirp == NULL )
	{
		perror("open dir fail!");
		return -1;
	}

	while(1)
	{
		memset(buff, 0, sizeof(buff));
		filp = readdir(dirp);
		if( filp == NULL )	//文件读取完毕,重新读文件
		{
			closedir(dirp);
			break;
		}
		len = strlen(filp->d_name);

		/* 判断是否为.jpg文件 */
		if( !strcmp(&filp->d_name[len-4], ".jpg" ) )
		{
			sprintf(buff, "%s/%s", argv[1],filp->d_name);
			strcpy(picname[cnt++], buff);			
		}
	}

	for( int i = 0; i < cnt; i++ )
	{
		printf("%s\n", picname[i]);
	}
	
	int i = 0;
	while(1)
	{
		lcd_clear(WHITHE);
		jpeg_decompression(picname[i]);
		sleep(1);
		memset(&keyval, 0, sizeof(keyval));
		read(fd, &keyval, sizeof(keyval));
		if( keyval.type ==  EV_ABS )
		{
			if( keyval.code ==  ABS_MT_POSITION_X )
			{
				printf("key value: %d \n", keyval.value);
				
				if( keyval.value <=  MIDDLELINE )
				{
					i--;
					if( i < 0 )
						i = cnt - 1;
				}
				else
				{
					i++;
					if( i >= cnt )
						i = 0;
				}
			}
		}
	}

	FreeFb(fb_start, fd_dev, len_dev);
	close(fd);
	return 0;


	
}




