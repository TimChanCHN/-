#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/gpio.h>
#include <asm/uaccess.h>
#

#define KEY_GPIO	5
#define KEY_NAME	"key_misc"

#define CMD_READ	_IOR('k', 0, int)
#define CMD_WRITE	_IOW('k', 1, char*)
#define CMD_PRINT	_IO('k', 2)

/* 接口函数定义 */
static ssize_t misc_key_read (struct file *filp, char __user *buff, size_t count, loff_t *offset)
{
	int value = 0, ret = 0;
	value = gpio_get_value(KEY_GPIO);
	ret = copy_to_user(buff, &value, sizeof(value));

	return 0;
}

long key_misc_ioctl(struct file *fp, unsigned int cmd, unsigned long arg)
{
	switch(cmd)
	{
		//读
		case(CMD_READ):{
			int ret = 0, value = 0;
			value = gpio_get_value(KEY_GPIO);
			ret = copy_to_user((void __user*)arg, &value, sizeof(value));
			printk("-----1   read-----\n");
			break;
		}
		//写
		case(CMD_WRITE):{
			int ret = 0;
			char buffer[32] = {0};
			ret = copy_from_user(buffer, (void __user*)arg, sizeof(buffer));
			printk("-----2   write-----\n");
			printk("buff : %s \n",buffer);
			break;
		}
		//print
		case(CMD_PRINT):{
			printk("-----3  drv-----\n");
			break;
		}
		//其他情况
		default:{
			printk("invalid cmd!\n");
			break;
		}
	}
}

static struct file_operations g_tfops = {
	.owner			=	THIS_MODULE,
	.read			=	misc_key_read,
	.unlocked_ioctl	=	key_misc_ioctl,
};


static struct  miscdevice g_tkeymisc = {
	.minor	=	MISC_DYNAMIC_MINOR,
	.name	=	"MiscKey",
	.fops	=	&g_tfops,
};

static int __init key_ioctl_init(void)
{
	printk("-----this is KEY module:ioctl------\n");
	/* 杂项字符设备注册 */
	misc_register(&g_tkeymisc);

	gpio_request( KEY_GPIO, KEY_NAME);
	gpio_direction_input(KEY_GPIO);

	return 0;
}

static void __exit key_ioctl_exit(void)
{
	printk("-----exit KEY MODULE!!!-------\n");
	gpio_free(KEY_GPIO);
	misc_deregister(&g_tkeymisc);
}

module_init(key_ioctl_init);
module_exit(key_ioctl_exit);
MODULE_LICENSE("GPL");


