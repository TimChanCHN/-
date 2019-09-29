/*-------------------------------------------------------------------------
    	存在问题：
    	当thread程序执行结束后才会发送数据到应用层(前两次才会这样）
-------------------------------------------------------------------------*/
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
#include <linux/delay.h>


// 该模块用于调用工作队列
// 利用按键S2，中断程序中控制LED1亮，
// 同时一进入中断，利用中断底半部，使LED2闪烁10次

// GPIOpx = P x 32 + N;  P: A,B...  
#define KEY_GPF0		(5*32 + 0)
#define NAME_S2			"KEY_S2"
#define KEY_GPF2		(5*32 + 2)
#define NAME_S3			"KEY_S3"
#define KEY_GPG3		(6*32 + 3)
#define NAME_S4			"KEY_S4"

#define LED_GPF4		(5*32 + 4)
#define NAME_LED1		"LED1"
#define LED_GPF5		(5*32 + 5)
#define NAME_LED2		"LED2"

struct keyinfo{
	unsigned int gpio;
	char		 name[12];
	int 		 value;
	int			 irq;
};

static void work_queue_thread(struct work_struct *work);
static bool condition = 0;
static struct keyinfo g_tkeys2 = {KEY_GPF0, NAME_S2, 0, 0};
static char keyname[12] = {0};
static struct timer_list g_ttimer;
static DECLARE_WAIT_QUEUE_HEAD(wq);
static 	 DECLARE_WORK(work,work_queue_thread);

/* 中断底半部，队列处理函数 */
/* 处理LED2闪烁 */
static void work_queue_thread(struct work_struct *work)
{
	int value_led2 = 1, i = 10;

	while( i -- )
	{
		value_led2 = !value_led2;
		gpio_set_value( LED_GPF5, value_led2);
		mdelay(500);
	}

}

/* 定时器中断服务函数 */
void irq_timer_handler(unsigned long arg)
{
	struct keyinfo *pkey = (struct keyinfo*) arg;
	static int value_led1 = 1;

	if( pkey->value == 0)
	{
		/* 启动中断底半部并点亮LED1 */
		value_led1 = !value_led1;
		if( value_led1 == 0 )
		{
			schedule_work(&work);
		}
		else
		{
			cancel_work_sync(&work);
		}
		gpio_set_value( LED_GPF4, value_led1);

		/* 启动发送数据至应用层 */
		strcpy(keyname, pkey->name);
		condition = 1;
		wake_up_interruptible(&wq);
	}
	printk("exit irq!\n");
}

/* 外部中断服务函数 */
static irqreturn_t irq_key_handler(int irq, void* arg)
{
	if( (irq == g_tkeys2.irq) )
	{
		g_ttimer.data = (unsigned long)arg;
		mod_timer(&g_ttimer, jiffies + HZ / 100);
		return IRQ_HANDLED;
	}

	return IRQ_NONE;
}


/* 接口函数定义 */
static ssize_t misc_key_read (struct file *filp, char __user *buff, size_t count, loff_t *offset)
{
	int ret = 0;
	wait_event_interruptible(wq, condition);
	condition = 0;
	printk("send data finish!---\n");
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
	.name	=	"key_workqueue",
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

	gpio_request(LED_GPF4, NAME_LED1);
	gpio_request(LED_GPF5, NAME_LED2);
	gpio_direction_output( LED_GPF4, 1);
	gpio_direction_output( LED_GPF5, 1);

	/* 定时器资源初始化 */
	setup_timer(&g_ttimer,  irq_timer_handler,  0);

	 printk("ready to work_queue!\n");
	

	return 0;
}

static void __exit MISC_KEY_EXIT(void)
{
	printk("-----exit KEY MODULE!!!-------\n");
	free_irq(g_tkeys2.irq, (void *)&g_tkeys2);

	
	gpio_free(g_tkeys2.gpio);
	gpio_free(LED_GPF4);
	gpio_free(LED_GPF5);
	misc_deregister(&g_tkeymisc);
	del_timer_sync(&g_ttimer);
}

module_init(MISC_KEY_INIT);
module_exit(MISC_KEY_EXIT);
MODULE_LICENSE("GPL");


