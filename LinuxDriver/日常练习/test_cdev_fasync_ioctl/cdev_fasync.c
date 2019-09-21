#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/miscdevice.h>
#include <asm/uaccess.h>
#include <linux/fs.h>
#include <linux/wait.h>
#include <linux/timer.h>
#include <linux/delay.h>
#include <linux/poll.h>
#include <linux/cdev.h>
#include <linux/slab.h>

#define BASEMINOR	0
#define COUNT		5
#define NAME_DEV	"CDEV_KEY"

#define GPIO_KEY	5
#define NAME_KEY	"PWR_KEY"

#define CMD_READ	_IOR('z',0,int)
#define CMD_WRITE	_IOW('z',1,char*)
#define CMD_PRINT	_IO('z',2)

static dev_t g_cdev;
static struct cdev *g_tcdevkey = NULL;
static DECLARE_WAIT_QUEUE_HEAD(wq);
static bool condition = 0;

struct keyinfo{
	unsigned int gpio;
	char name[12];
	int value;
	int irq;
};

static struct keyinfo g_tkey={GPIO_KEY, NAME_KEY, 0, 0};
static struct timer_list g_ttimer;
static char keyname[12] = {0};
static struct fasync_struct* fapp=NULL;


/* 外部中断服务函数 */
static irqreturn_t cdev_key_handler(int irq, void *arg)
{
	if( irq == g_tkey.irq )
	{
		g_ttimer.data = (unsigned long)arg;
		mod_timer(&g_ttimer, jiffies + HZ/100);
		return IRQ_HANDLED;
	}
	return IRQ_NONE;
}

/* 定时器中断服务函数 */
static void cdev_timer_handler(unsigned long arg)
{
	struct keyinfo *pkey = (struct keyinfo*)arg;
	strcpy(keyname, pkey->name);
	pkey->value = gpio_get_value(pkey->gpio);
	if( pkey->value == 0 )
	{	
		kill_fasync(&fapp, SIGIO, POLL_IN);
		condition = 1;
		wake_up_interruptible(&wq);

	}

}


/* fops */
static ssize_t cdev_read(struct file *filp, char __user *buff, size_t cnt, loff_t *offset)
{
	int ret = 0;
	
	wait_event_interruptible(wq, condition);
	condition = 0;

	g_tkey.value = gpio_get_value(g_tkey.gpio);
	ret = copy_to_user(buff, keyname, sizeof(keyname));

	return 0;
}

static int cdev_fasync(int fd, struct file *filp, int on)
{
	return fasync_helper(fd, filp, on, &fapp);
}

static long cdev_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	int ret = 0;
	char buff[] = "hello application!";
	char string[100] = {0};
	switch(cmd)
	{
		case(CMD_READ):{
			ret = copy_to_user((void __user *)arg, buff, sizeof(buff));
			printk("---111---\n");
			break;
		}	
		case(CMD_WRITE):{
			ret = copy_from_user( string, (void __user *)arg, sizeof(string));
			printk("***%s***\n",string);
			printk("---222---\n");
			break;
		}
		case(CMD_PRINT):{
			printk("---333---\n");
			break;
		}
		default:{
			printk("---error cmd!---\n");
			break;
		}
	}
	return 0;
}

static struct file_operations g_tfops={
	.owner			=	THIS_MODULE,
	.read			=	cdev_read,
	.fasync			=	cdev_fasync,
	.unlocked_ioctl	=	cdev_ioctl,
};

static int __init cdev_fasync_init(void)
{
	int ret = 0;
	//标准字符设备注册
	alloc_chrdev_region(&g_cdev,  BASEMINOR,  COUNT,  NAME_DEV);
	g_tcdevkey =  cdev_alloc();
	cdev_init(g_tcdevkey,  &g_tfops);
	cdev_add(g_tcdevkey, g_cdev, COUNT);
	printk("---major:%d  minor:%d---\n",MAJOR(g_cdev),MINOR(g_cdev));

	/* 设备申请 */
	gpio_request(g_tkey.gpio, g_tkey.name);
	g_tkey.irq = gpio_to_irq(g_tkey.gpio);
	ret = request_irq(g_tkey.irq, cdev_key_handler, 
					  IRQF_SHARED| IRQF_TRIGGER_RISING| IRQF_TRIGGER_FALLING, 
					  g_tkey.name, (void *)&g_tkey);

	/* 定时器初始化 */
	setup_timer(&g_ttimer, cdev_timer_handler, 0);

	
	return 0;
}

static void __exit cdev_fasync_exit(void)
{
	/* 释放定时器 */
	del_timer(&g_ttimer);

	/* 标准字符设备注销 */
	cdev_del(g_tcdevkey);
	kfree(g_tcdevkey);
	unregister_chrdev_region(g_cdev, COUNT);

	/* 外设资源释放 */
	free_irq(g_tkey.gpio, (void *)&g_tkey);
	gpio_free(g_tkey.gpio);
}

module_init(cdev_fasync_init);
module_exit(cdev_fasync_exit);
MODULE_LICENSE("GPL");

