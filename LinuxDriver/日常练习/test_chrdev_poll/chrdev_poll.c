#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/timer.h>
#include <linux/wait.h>
#include <linux/interrupt.h>
#include <linux/poll.h>
#include <asm/uaccess.h>
#include <linux/fs.h>
#include <linux/gpio.h>


#define	GPIO_KEY	5
#define NAME_KEY	"KEY_PWR"


struct keyinfo{
	unsigned int gpio;
	char name[12];
	int value;
	int irq;	
};

static struct keyinfo g_tkey={GPIO_KEY, NAME_KEY, 0, 0};
static struct timer_list g_ttimer;
static DECLARE_WAIT_QUEUE_HEAD(wq); 
static bool condition = 0;
static char keyname[12] = {0};
static unsigned int dev_major = 0;


/* 外部中断服务函数 */
static irqreturn_t chrdev_key_handler(int irq, void *arg)
{
	if( irq == g_tkey.irq )
	{
		g_ttimer.data = (unsigned long)arg;
		mod_timer(&g_ttimer, jiffies + HZ / 100);	
		return IRQ_HANDLED;
	}	
	return IRQ_NONE;
}

/* 定时器中断服务函数 */
static void chrdev_timer_handler(unsigned long arg)
{
	struct keyinfo *pkey = (struct keyinfo*)arg;

	strcpy(keyname, pkey->name);
	condition = 1;
	wake_up_interruptible(&wq);
}

static ssize_t chrdev_key_read(struct file *filp, char __user *buff, size_t cnt, loff_t *offset)
{
	int ret = 0;
	wait_event_interruptible(wq,condition);
	condition = 0;

	g_tkey.value = gpio_get_value(g_tkey.gpio);
	if( g_tkey.value == 0 )
	{
		ret = copy_to_user(buff, keyname, sizeof(keyname));
	}	
	return 0;
}

static unsigned int chrdev_key_poll(struct file *filp, struct poll_table_struct *wait)
{
	poll_wait(filp,  &wq,  wait);  

	if( condition == 1 )
	{
		return POLLIN| POLLRDNORM;
	}

	return 0;
}

static struct file_operations g_tfops={
	.owner	=	THIS_MODULE,
	.read	=	chrdev_key_read,
	.poll	=	chrdev_key_poll,
};

static int __init chrdev_poll_key_init(void)
{
	int ret = 0;
	printk("---enter---\n");
	//早期字符设备注册
	dev_major = register_chrdev(0, NAME_KEY, &g_tfops);
	printk("---major device:%d---\n", dev_major);

	/* 设备资源申请 */
	gpio_request( g_tkey.gpio, g_tkey.name);
	g_tkey.irq = gpio_to_irq(g_tkey.gpio);
	ret = request_irq(g_tkey.irq, chrdev_key_handler, 
					IRQF_SHARED| IRQF_TRIGGER_RISING| IRQF_TRIGGER_FALLING, 
					g_tkey.name, (void *)&g_tkey);
				
	/* 定时器初始化 */
	setup_timer(&g_ttimer, chrdev_timer_handler, 0);
	
	return 0;
}


static void __exit chrdev_poll_key_exit(void)
{
	printk("---exit---\n");
	
	free_irq(g_tkey.irq, (void *)&g_tkey);
	gpio_free(g_tkey.gpio);
	unregister_chrdev(dev_major, NAME_KEY);
	del_timer(&g_ttimer);
}

module_init(chrdev_poll_key_init);
module_exit(chrdev_poll_key_exit);
MODULE_LICENSE("GPL");


