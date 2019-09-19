#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/poll.h>
#include <linux/timer.h>
#include <linux/miscdevice.h>
#include <linux/wait.h>
#include <asm/uaccess.h>

#define GPIO_KEY	5
#define NAME_KEY	"KEY_PWR"

static DECLARE_WAIT_QUEUE_HEAD(wq);
static bool condition = 0;
static char keyname[12] = {0};
static struct  timer_list g_ttimer;


struct keyinfo
{
	unsigned long gpio;
	char name[12];
	int value;
	int irq;
};

struct keyinfo g_tkey={GPIO_KEY, NAME_KEY, 0, 0};

/* 中断服务函数 */
static irqreturn_t irq_key_handler(int irq, void* arg)
{
	if( irq == g_tkey.irq)
	{
		g_ttimer.data = (unsigned long)arg;
		mod_timer(&g_ttimer, jiffies + HZ / 100);
		return IRQ_HANDLED;
	}
	return IRQ_NONE;
}

/* 定时器中断服务函数 */
static void timer_key_handler(unsigned long arg)
{
	struct keyinfo* pkey = (struct keyinfo*)arg;
	strcpy(keyname, pkey->name);
	condition = 1;
	wake_up_interruptible(&wq);
}


/* 文件操作集合 */
static ssize_t misc_key_read(struct file *filp, char __user *buff, size_t cnt, loff_t *offset)
{
	int ret = 0;
	/* 阻塞等待 */
	wait_event_interruptible(wq, condition);
	condition = 0;
	
	ret = copy_to_user(buff, keyname, sizeof(keyname));
	return 0;
}

static unsigned int misc_key_poll(struct file *filp, struct poll_table_struct *wait)
{
	poll_wait( filp, &wq, wait);
	if( condition == 1 )
	{
		return POLLIN| POLLRDNORM;
	}

	return 0;
}

/* 文件操作集合 */
static struct file_operations g_tfops={
	.owner	=	THIS_MODULE,
	.read	=	misc_key_read,
	.poll	=	misc_key_poll,
};

/* 定义杂项字符设备 */
static struct miscdevice g_tmisckey={
	.minor	=	MISC_DYNAMIC_MINOR,
	.name	=	"key_poll",
	.fops	=	&g_tfops,
};

static int __init misc_key_poll_init(void)
{
	int ret = 0;
	printk("---enter!---\n");
	/* 杂项字符设备注册 */
	misc_register(&g_tmisckey);

	setup_timer(&g_ttimer, timer_key_handler, 0);

	/* 设备资源申请 */
	gpio_request(GPIO_KEY, NAME_KEY);
	g_tkey.irq	=	gpio_to_irq(GPIO_KEY);
	ret = request_irq( g_tkey.irq, 
					   irq_key_handler, 
					   IRQF_SHARED| IRQF_TRIGGER_RISING| IRQF_TRIGGER_FALLING, 
					   g_tkey.name, &g_tkey);
	
	return 0;
}

static void __exit misc_key_poll_exit(void)
{
	printk("---exit!---\n");
	misc_deregister(&g_tmisckey);
	gpio_free(GPIO_KEY);
	free_irq(g_tkey.irq, &g_tkey);
	del_timer_sync(&g_ttimer);
}

module_init(misc_key_poll_init);
module_exit(misc_key_poll_exit);
MODULE_LICENSE("GPL");	

