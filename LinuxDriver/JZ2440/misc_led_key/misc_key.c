#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/gpio.h>
#include <asm/uaccess.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/wait.h>
#include <linux/interrupt.h>
#include <linux/timer.h>
#include <linux/sched.h>

// 该模块用于检测按键状态
// 申请外部中断以及定时器中断

// GPIOpx = P x 32 + N;  P: A,B...  
#define KEY_GPF0		(5*32 + 0)
#define NAME_S2			"KEY_S2"
#define KEY_GPF2		(5*32 + 2)
#define NAME_S3			"KEY_S3"
#define KEY_GPG3		(6*32 + 3)
#define NAME_S4			"KEY_S4"

struct keyinfo{
	unsigned int gpio;
	char		 name[12];
	int 		 value;
	int			 irq;
};


static bool condition = 0;
static struct keyinfo g_tkeys2 = {KEY_GPF0, NAME_S2, 0, 0};
static struct keyinfo g_tkeys3 = {KEY_GPF2, NAME_S3, 0, 0};
static struct keyinfo g_tkeys4 = {KEY_GPG3, NAME_S4, 0, 0};
static char keyname[12] = {0};
static struct timer_list g_ttimer;
static DECLARE_WAIT_QUEUE_HEAD(wq);


/* 定时器中断服务函数 */
void irq_timer_handler(unsigned long arg)
{
	struct keyinfo *pkey = (struct keyinfo*) arg;
	if( pkey->value == 0)
	{
		strcpy(keyname, pkey->name);
		condition = 1;
		wake_up_interruptible(&wq);
	}
}

/* 外部中断服务函数 */
static irqreturn_t irq_key_handler(int irq, void* arg)
{
	if( (irq == g_tkeys2.irq) || (irq == g_tkeys3.irq) || (irq == g_tkeys4.irq) )
	{
		g_ttimer.data = (unsigned long)arg;
		mod_timer(&g_ttimer, jiffies + HZ / 100);
		return IRQ_HANDLED;
	}

	return IRQ_NONE;
}

/* 中断申请设置 */
/* BUG:不能把中断号返回，导致退出模块时崩溃 */
static void key_irq_set(struct keyinfo key)
{
	int ret = 0;
	gpio_request(key.gpio, key.name);
	key.irq = gpio_to_irq(key.gpio);
	ret = request_irq(key.irq, irq_key_handler, 
					  IRQF_SHARED| IRQF_TRIGGER_RISING, key.name, &key);
	printk("%s is set\n", key.name);
}

/* 接口函数定义 */
static ssize_t misc_key_read (struct file *filp, char __user *buff, size_t count, loff_t *offset)
{
	int ret = 0;
	wait_event_interruptible(wq, condition);
	condition = 0;
	ret = copy_to_user(buff, keyname, sizeof(keyname));

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
	.name	=	"key_misc",
	.fops	=	&g_tfops,
};

static int __init MISC_KEY_INIT(void)
{
	int ret = 0;
	printk("-----this is KEY module------\n");
	/* 杂项字符设备注册 */
	misc_register(&g_tkeymisc);

	/* 设备资源申请以及初始化 */
	/* 申请KEY资源	*/
	gpio_request(g_tkeys2.gpio, g_tkeys2.name);
	g_tkeys2.irq = gpio_to_irq(g_tkeys2.gpio);
	ret = request_irq(g_tkeys2.irq, irq_key_handler, 
					  IRQF_SHARED| IRQF_TRIGGER_RISING, g_tkeys2.name, &g_tkeys2);

	gpio_request(g_tkeys3.gpio, g_tkeys3.name);
	g_tkeys3.irq = gpio_to_irq(g_tkeys3.gpio);
	ret = request_irq(g_tkeys3.irq, irq_key_handler, 
					  IRQF_SHARED| IRQF_TRIGGER_RISING, g_tkeys3.name, &g_tkeys3);

	gpio_request(g_tkeys4.gpio, g_tkeys4.name);
	g_tkeys4.irq = gpio_to_irq(g_tkeys4.gpio);
	ret = request_irq(g_tkeys4.irq, irq_key_handler, 
					  IRQF_SHARED| IRQF_TRIGGER_RISING, g_tkeys4.name, &g_tkeys4);

	/* 定时器资源初始化 */
	setup_timer(&g_ttimer,  irq_timer_handler,  0);
	

	return 0;
}

static void __exit MISC_KEY_EXIT(void)
{
	printk("-----exit KEY MODULE!!!-------\n");
	free_irq(g_tkeys2.irq, (void *)&g_tkeys2);
	free_irq(g_tkeys3.irq, (void *)&g_tkeys3);
	free_irq(g_tkeys4.irq, (void *)&g_tkeys4);
	
	gpio_free(g_tkeys2.gpio);
	gpio_free(g_tkeys3.gpio);
	gpio_free(g_tkeys4.gpio);
	
	misc_deregister(&g_tkeymisc);
	del_timer_sync(&g_ttimer);
}

module_init(MISC_KEY_INIT);
module_exit(MISC_KEY_EXIT);
MODULE_LICENSE("GPL");


