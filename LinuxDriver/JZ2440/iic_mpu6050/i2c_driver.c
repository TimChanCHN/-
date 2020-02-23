#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/i2c.h>
#include <linux/types.h>
#include <linux/miscdevice.h>
#include <asm/uaccess.h>
#include <linux/fs.h>

#define	SMPLRT_DIV		0x19	//陀螺仪采样率，典型值：0x07(125Hz), 1khz
#define	CONFIG			0x1A	//低通滤波频率，典型值：0x06(5Hz)
#define	GYRO_CONFIG		0x1B	//陀螺仪自检及测量范围，典型值：0x18(不自检，2000deg/s)
#define	ACCEL_CONFIG	0x1C	//加速计自检、测量范围及高通滤波频率，典型值：0x01(不自检，2G，5Hz)
#define	ACCEL_XOUT_H	0x3B
#define	ACCEL_XOUT_L	0x3C
#define	ACCEL_YOUT_H	0x3D
#define	ACCEL_YOUT_L	0x3E
#define	ACCEL_ZOUT_H	0x3F
#define	ACCEL_ZOUT_L	0x40
#define	TEMP_OUT_H		0x41
#define	TEMP_OUT_L		0x42
#define	GYRO_XOUT_H		0x43
#define	GYRO_XOUT_L		0x44	
#define	GYRO_YOUT_H		0x45
#define	GYRO_YOUT_L		0x46
#define	GYRO_ZOUT_H		0x47
#define	GYRO_ZOUT_L		0x48
#define	PWR_MGMT_1		0x6B	//电源管理，典型值：0x00(正常启用)
#define	WHO_AM_I		0x75	//IIC地址寄存器(默认数值0x68，只读)

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

struct i2c_client* m_pclient = NULL;

/* mpu6050初始化    */
static void init_mpu6050(struct i2c_client* pclient)
{
	i2c_smbus_write_byte_data(pclient, PWR_MGMT_1, 0x00);
	i2c_smbus_write_byte_data(pclient, SMPLRT_DIV, 0x07);
	i2c_smbus_write_byte_data(pclient, CONFIG, 0x06);
	i2c_smbus_write_byte_data(pclient, GYRO_CONFIG, 0x18);
	i2c_smbus_write_byte_data(pclient, ACCEL_CONFIG, 0x01);
}

/* 读数据指令 */
static u16 MPU6050_GetData(struct i2c_client* pclient, u8 REG_ADDRESS)
{
	u8 H, L;
	i2c_smbus_write_byte(pclient, REG_ADDRESS);
	H = i2c_smbus_read_byte(pclient);
	i2c_smbus_write_byte(pclient, REG_ADDRESS+1);
	L = i2c_smbus_read_byte(pclient);

	return (H<<8)+L;
}
static long mpu6050_ioctl (struct file* fp, unsigned int cmd, unsigned long arg)
{
	int ret = 0;
	union mpu6050_data data = {{0}};
	
	switch(cmd)
	{
		case READ_ACCEL:
			data.accel.x = MPU6050_GetData(m_pclient, ACCEL_XOUT_H);
			data.accel.y = MPU6050_GetData(m_pclient, ACCEL_YOUT_H);
			data.accel.z = MPU6050_GetData(m_pclient, ACCEL_ZOUT_H);
			break;

		case READ_GYROS:
			data.gyro.x = MPU6050_GetData(m_pclient, GYRO_XOUT_H);
			data.gyro.y = MPU6050_GetData(m_pclient, GYRO_YOUT_H);
			data.gyro.z = MPU6050_GetData(m_pclient, GYRO_ZOUT_H);
			break;

		default:
			break;
	}
	ret = copy_to_user((void*)arg, &data, sizeof(data));
	
	return 0;	
}

static struct file_operations g_tfops = {
	.owner			=	THIS_MODULE,
	.unlocked_ioctl	=	mpu6050_ioctl,
};

static struct miscdevice g_tmisckey={
	.minor	=	MISC_DYNAMIC_MINOR,
	.name	=	"mpu6050",
	.fops	=	&g_tfops,
};

static int i2c_driver_probe(struct i2c_client *pclient, const struct i2c_device_id *pdevice)
{
	m_pclient = pclient;
	printk("---i2c matched---\n");
	printk("---name:%s  addr:%d  plt_data:%s---\n", pclient->name, pclient->addr, (char*)pclient->dev.platform_data);
	
	init_mpu6050(pclient);
	printk("mpu6050 init success!\n");

	/* 注册一个杂项字符设备，供给应用层调用 */
	misc_register(&g_tmisckey);

	return 0;
}

static int i2c_driver_remove(struct i2c_client *pclient)
{
	printk("---exit---\n");
	misc_deregister(&g_tmisckey);
	return 0;
}



static struct i2c_device_id g_tarrname[] = {
		{  "i2c_touch", 123	},
		{  "i2c_at24c02", 888	},
		{  "i2c_nandflash"	},
		{  "i2c_eeprom"	},
		{  "i2c_mpu6050"  },
};


static struct i2c_driver g_ti2cdriver = {
	.probe		=	i2c_driver_probe,
	.remove		=	i2c_driver_remove,
	
	.driver		=	{
		.name	=	"i2c_driver",
		.owner	=	THIS_MODULE,
	},
	.id_table	=	g_tarrname,
};

static int __init iic_driver_init(void)
{
	i2c_register_driver(THIS_MODULE, &g_ti2cdriver);
	
	return 0;
}

static void __exit iic_driver_exit(void)
{
	i2c_del_driver(&g_ti2cdriver);
}

module_init(iic_driver_init);
module_exit(iic_driver_exit);
MODULE_LICENSE("GPL");
