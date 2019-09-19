#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/gpio.h>
#include <asm/uaccess.h>
#include <linux/delay.h>

#define LED_GPIO	13
#define LED_NAME	"LED"
#define OFF			0
#define ON			1

/*	作用：
 *	把内核中的数据提供给应用层
 *
 */
ssize_t misc_led_write (struct file *fp, const char __user *buff, size_t count, loff_t *offset)
{
	int ret = 0, value = 0;

	ret = copy_from_user(&value,buff,sizeof(value));
	gpio_set_value( LED_GPIO, value);

	return 0;
}


/* 文件操作集合定义以及初始化 */
static struct file_operations g_tfops={
	.owner	=	THIS_MODULE,
	.write	=	misc_led_write	,	
};



/* 杂项字符设备定义以及初始化 */
static struct  miscdevice g_tmiscled={
	.minor	=	MISC_DYNAMIC_MINOR,
	.name	=	"MiscLed",
	.fops	=	&g_tfops,
};

static int __init misc_led_init(void)
{
	printk("-----this is LED module------\n");
	/* 杂项字符设备注册 */
	misc_register(&g_tmiscled);

	/* 设备资源申请以及处理 */
	gpio_request(LED_GPIO, LED_NAME);
	gpio_direction_output(LED_GPIO, OFF);
	
	return 0;
}

static void __exit misc_led_exit(void)
{
	printk("-----exit LED MODULE!!!-------\n");
	/* 杂项字符设备取消注册 */
	misc_deregister(&g_tmiscled);

	/* 释放设备资源 */
	gpio_free(LED_GPIO);
}


module_init(misc_led_init);
module_exit(misc_led_exit);
MODULE_LICENSE("GPL");

