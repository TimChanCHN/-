
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <poll.h>
#include <sys/select.h>
#include <sys/time.h>
#include <string.h>
#include <linux/input.h>
#include <sys/ioctl.h>


#define EV_KEY			0x01
#define MPU6050_READ	'K'

union mpu6050_data{
	struct{
		short x;
		short y;
		short z;
	}accel;
	struct{
		short x;
		short y;
		short z;
	}gyro;
};

#define READ_ACCEL _IOR(MPU6050_READ, 0, union mpu6050_data)
#define READ_GYROS _IOR(MPU6050_READ, 1, union mpu6050_data)

int main(int argc, char* argv[])
{
	int fd, ret, value = 0;
	char buff[12] = {0};
	
	fd = open(argv[1], O_RDWR);
	if( fd < 0 )
	{
		perror("open fail");
		return -1;
	}

	union mpu6050_data data = {{0}};
	while(1)
	{
		printf("---------------------------------\n");
		ioctl(fd, READ_ACCEL, &data);
		printf("accel.x:%d, accel.y:%d, accel.z:%d\n",data.accel.x, data.accel.y, data.accel.z);
		ioctl(fd, READ_GYROS, &data);
		printf("gyros.x:%d, gyros.y:%d, gyros.z:%d\n", data.gyro.x, data.gyro.y, data.gyro.z);
		printf("\n");
		sleep(1);
	}

	close(fd);
}

