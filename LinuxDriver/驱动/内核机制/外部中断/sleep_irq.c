#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <asm/uaccess.h>

#define GPIO_KEY	5

struct keyinfo{
	char name[12];
	unsigned long gpio;
	int value;
	unsigned int irq;
};

static int g_ispress = 0;
static char g_keyname[12] = {0};

static struct keyinfo g_tkey = {
	"key_power", GPIO_KEY, 1, 0
};

/* 中断服务函数 */
static irqreturn_t key_irq_handler(int irq,void* data)
{
	struct keyinfo *pkey = (struct keyinfo*)data;

	if( irq == g_tkey.irq )
	{
		g_ispress = 1;
		strcpy(g_keyname, pkey->name);
		return IRQ_HANDLED;
	}
	return IRQ_NONE;
}

/* 文件操作函数 */
ssize_t key_irq_read (struct file *filp, char __user *buff, size_t cnt, loff_t *offset)
{
	int ret = 0;
	
	while(!g_ispress)
	{
		msleep(10);
	}
	g_ispress = 0;
	
	ret = copy_to_user(buff, g_keyname, strlen(g_keyname));
	return 0;
}


/* 文件操作集合定义 */
static struct file_operations g_tfops = {
	.owner	=	THIS_MODULE,
	.read	=	key_irq_read,
};


/* 杂项字符设备定义 */
static struct miscdevice g_tkeymisc={
	.minor	=	MISC_DYNAMIC_MINOR,
	.name	=	"key_irq",
	.fops	=	&g_tfops,
};

static int __init module_irq_init(void)
{
	int ret = 0;
	gpio_request(g_tkey.gpio,g_tkey.name);
	g_tkey.irq = gpio_to_irq(g_tkey.gpio );
	ret = request_irq(g_tkey.irq, key_irq_handler, 
					IRQF_SHARED|IRQF_TRIGGER_RISING|IRQF_TRIGGER_FALLING, 
					g_tkey.name, (void*)&g_tkey);
	misc_register(&g_tkeymisc);

	return 0;
}

static void __exit module_irq_exit(void)
{
	free_irq(g_tkey.irq, (void*)&g_tkey);
	gpio_free(g_tkey.gpio);
	misc_deregister(&g_tkeymisc);
}


module_init(module_irq_init);
module_exit(module_irq_exit);
MODULE_LICENSE("GPL");


