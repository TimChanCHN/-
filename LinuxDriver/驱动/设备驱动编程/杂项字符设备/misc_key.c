#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/gpio.h>
#include <asm/uaccess.h>
#include <linux/delay.h>

#define KEY_GPIO	5
#define KEY_NAME	"key_misc"

/* 接口函数定义 */
static ssize_t misc_key_read (struct file *filp, char __user *buff, size_t count, loff_t *offset)
{
	int value = 0, ret = 0;
	value = gpio_get_value(KEY_GPIO);
	ret = copy_to_user(buff, &value, sizeof(value));

	return 0;
}

/* 文件操作集合定义 */
static struct file_operations g_tfops = {
	.owner	=	THIS_MODULE,
	.read	=	misc_key_read,
};

/* 杂项字符设备变量定义以及初始化 */
static struct  miscdevice g_tkeymisc = {
	.minor	=	MISC_DYNAMIC_MINOR,
	.name	=	"MiscKey",
	.fops	=	&g_tfops,
};

static int __init MISC_KEY_INIT(void)
{
	printk("-----this is KEY module------\n");
	/* 杂项字符设备注册 */
	misc_register(&g_tkeymisc);

	/* 设备资源申请以及初始化 */
	/* 申请KEY/LED资源	*/
	gpio_request( KEY_GPIO, KEY_NAME);
	gpio_direction_input(KEY_GPIO);

	return 0;
}

static void __exit MISC_KEY_EXIT(void)
{
	printk("-----exit KEY MODULE!!!-------\n");
	gpio_free(KEY_GPIO);
	misc_deregister(&g_tkeymisc);
}

module_init(MISC_KEY_INIT);
module_exit(MISC_KEY_EXIT);
MODULE_LICENSE("GPL");

