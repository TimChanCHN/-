#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/gpio.h>
#include <asm/uaccess.h>
#include <linux/delay.h>

#define LED_GPF4		(5*32 + 4)
#define NAME_LED1		"LED1"
#define LED_GPF5		(5*32 + 5)
#define NAME_LED2		"LED2"
#define LED_GPF6		(5*32 + 6)
#define NAME_LED4		"LED4"

struct ledinfo{
	unsigned int gpio;
	char 	 name[12];
	int 	 value;
};

struct ledinfo g_tled1 = {LED_GPF4, NAME_LED1, 1};
struct ledinfo g_tled2 = {LED_GPF5, NAME_LED2, 1};
struct ledinfo g_tled4 = {LED_GPF6, NAME_LED4, 1};

/* 接口函数定义 */
static ssize_t misc_led_write(struct file *filp, const char __user *buff, size_t count, loff_t *offset)
{
	int ret, value;
	ret = copy_from_user(&value, buff, sizeof(value));

	switch(value)
	{
		case 1:
			g_tled1.value = !g_tled1.value;
			gpio_set_value(g_tled1.gpio, g_tled1.value);
			break;
		case 2:
			g_tled2.value = !g_tled2.value;
			gpio_set_value(g_tled2.gpio, g_tled2.value);
			break;
		case 4:
			g_tled4.value = !g_tled4.value;
			gpio_set_value(g_tled4.gpio, g_tled4.value);
			break;
		default:
			printk("wrong response\n");
			break;
	}
	return 0;
}

/* 文件操作集合定义 */
static struct file_operations g_tfops = {
	.owner	=	THIS_MODULE,
	.write 	=	misc_led_write,
};


/* 杂项字符设备变量定义以及初始化 */
static struct  miscdevice g_tledmisc = {
	.minor	=	MISC_DYNAMIC_MINOR,
	.name	=	"led_misc",
	.fops	=	&g_tfops,
};

static int __init MISC_LED_INIT(void)
{
	printk("-----this is LED module------\n");
	/* 杂项字符设备注册 */
	misc_register(&g_tledmisc);

	/* 申请LED资源	*/
	gpio_request( g_tled1.gpio, g_tled1.name);
	gpio_direction_output(g_tled1.gpio, 1);

	gpio_request( g_tled2.gpio, g_tled1.name);
	gpio_direction_output( g_tled2.gpio, 1);

	gpio_request( g_tled4.gpio, g_tled4.name);
	gpio_direction_output( g_tled4.gpio, 1);

	return 0;
}

static void __exit MISC_LED_EXIT(void)
{
	printk("-----exit LED MODULE!!!-------\n");
	gpio_free(g_tled1.gpio);
	gpio_free(g_tled2.gpio);
	gpio_free(g_tled4.gpio);
	misc_deregister(&g_tledmisc);
}



module_init(MISC_LED_INIT);
module_exit(MISC_LED_EXIT);
MODULE_LICENSE("GPL");