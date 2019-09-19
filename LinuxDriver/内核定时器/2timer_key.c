#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/timer.h>
#include <linux/miscdevice.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <asm/uaccess.h>
#include <linux/fs.h>

#define GPIO_KEY	5
#define NAME_KEY	"PowerKey"

static DECLARE_WAIT_QUEUE_HEAD(wq);
static bool condition = 0;
static char keybuff[12] = {0};

typedef struct keyinfo{
	unsigned long gpio;
	char name[12];
	int value;
	unsigned int irq;
}KeyInfo;

/* 按键结构体 */
KeyInfo g_tkey={GPIO_KEY, NAME_KEY,0, 0}; 

/* 定时器结构体 */
static struct  timer_list g_ttimer;

/* 定时器中断服务函数 */
static void key_timer_handler(unsigned long arg)
{
	KeyInfo* key = (KeyInfo*)arg; 
	
	strcpy(keybuff,key->name);
	condition = 1;
	wake_up_interruptible(&wq);
	
}

/* 外部中断服务函数 */
static irqreturn_t key_misc_handler(int irq, void* data)
{
	if( irq == g_tkey.irq )
	{
		//接收按键信息
		g_ttimer.data = (unsigned long)data;
		mod_timer(&g_ttimer, jiffies+HZ/200);
		return IRQ_HANDLED;
	}
	return IRQ_NONE;
}



/* fops操作函数 */
static ssize_t timer_key_read (struct file *filp, char __user *buff, size_t cnt, loff_t *offset)
{
	int ret = 0;	
	wait_event_interruptible(wq, condition);
	condition = 0;
	ret = copy_to_user(buff, keybuff, sizeof(keybuff));
	return 0;
}


struct file_operations g_tfops={
	.owner	=	THIS_MODULE,
	.read	=	timer_key_read,
};

static struct miscdevice g_tmisckey={
	.minor	=	MISC_DYNAMIC_MINOR,
	.name	=	"key_timer",
	.fops	=	&g_tfops,
};

static int __init timer_key_init(void)
{
	int ret = 0;
	printk("---enter---\n");
	setup_timer(&g_ttimer, key_timer_handler, 0);
	
	gpio_request(g_tkey.gpio, g_tkey.name);
	g_tkey.irq = gpio_to_irq(g_tkey.gpio);
	ret = request_irq(g_tkey.irq, 
					  key_misc_handler,
					  IRQF_SHARED| IRQF_TRIGGER_RISING| IRQF_TRIGGER_FALLING, 
					  g_tkey.name, (void *)&g_tkey);
	
	misc_register(&g_tmisckey);
	
	return 0;
}

static void __exit timer_key_exit(void)
{
	printk("---exit---\n");
	del_timer_sync(&g_ttimer);
	misc_deregister(&g_tmisckey);
	free_irq(g_tkey.irq, (void *)&g_tkey);
	gpio_free(g_tkey.gpio);
}

module_init(timer_key_init);
module_exit(timer_key_exit);
MODULE_LICENSE("GPL");

