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


struct  keyinfo  {
	char  name[12];
	unsigned int  gpio;
	int  value;
	int  irq;
};

static   struct  keyinfo  g_tkey  =  {
	"key_power",   5,   1,   0
};
static   char   keyname[12] = {0};	//存放按下的按键的按键名称	
static   DECLARE_WAIT_QUEUE_HEAD(wq);
static   bool   condition = 0;
static   struct  timer_list   g_ttimer;
static   struct fasync_struct*  fapp = NULL;


/* 外部中断的中断服务函数 -- 当有触发信号产生，触发设备的参数将会作为传参传递给中断服务函数 */
static   irqreturn_t    key_irq_handler(int irq,  void*  data)
{	
	if(irq  ==  g_tkey.irq)
	{
		g_ttimer.data  =  (unsigned long)data;
		mod_timer(&g_ttimer,  jiffies + HZ / 200);
		return   IRQ_HANDLED;
	}
	return   IRQ_NONE;
}

/* 定时器中断服务函数 */
static   void    key_timer_handler(unsigned long  arg)
{
	struct  keyinfo* pkey  =  (struct  keyinfo*)arg;
	strcpy(keyname,  pkey->name);
	
	pkey->value = gpio_get_value(pkey->gpio);
	if(pkey->value == 0)		//按键按下
	{
		kill_fasync(&fapp,   SIGIO,   POLL_IN);		//给进程发信号
		condition = 1;							//唤醒进程时，condition条件必须置1
		wake_up_interruptible(&wq);
	}
}

/* read接口函数的定义 */
static   ssize_t    misc_key_read(struct file*  filp,  char __user*  buff, size_t  count,  loff_t* offset)
{
	int  ret = 0;
	/* 进入read函数之后，直接睡眠阻塞 */
	wait_event_interruptible(wq,  condition);		//实时睡眠
	condition = 0;

	ret = copy_to_user(buff,   keyname,   min(strlen(keyname),  count));
	return   min(strlen(keyname),  count);
}

/* poll文件操作函数定义 */
static   unsigned int   misc_key_poll(struct file*  filp, struct poll_table_struct*  table)
{
	poll_wait(filp,   &wq,   table);	//不管是当前设备触发、还是其他同步管理的事件触发，该睡眠都将被唤醒	

	if(condition == 1)		//是当前设备触发	
	{
		return   POLLIN | POLLRDNORM;		//返回触发事件类型
	}
	return  0;	//不是当前设备触发
}

/* fasync接口函数的定义 -- 接收进程传递下来的进程PID */
static   int   misc_key_fasync(int  fd,  struct file*  filp,  int on)
{
	return   fasync_helper(fd,   filp,   on,   &fapp);
}

/* 文件操作集合定义、初始化 -- 在文件操作集合中声明给应用层提供哪些接口函数 */
static   struct file_operations   g_tfops = {
	.owner	=	THIS_MODULE,
	.read	=	misc_key_read, 
	.poll		=	misc_key_poll,
	.fasync	=	misc_key_fasync,
};

/* 杂项字符设备结构体定义、初始化 */
static   struct miscdevice  g_tmisckey = {
		.minor	=	MISC_DYNAMIC_MINOR,
		.name	=	"key_fasync",
		.fops		=	&g_tfops,	
};

/* 模块入口函数 -- 模块加载时自动调用 */
static  int  __init   key_irq_init(void)
{
	int  ret = 0;
	gpio_request(g_tkey.gpio,  g_tkey.name);
	g_tkey.irq  =  gpio_to_irq(g_tkey.gpio);
	ret = request_irq(g_tkey.irq,   key_irq_handler, 
			IRQF_SHARED | IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING, 
			g_tkey.name,  (void*)&g_tkey);

	misc_register(&g_tmisckey);
	setup_timer(&g_ttimer,  key_timer_handler,  0);
	return  0;
}

static  void  __exit  key_irq_exit(void)
{
	free_irq(g_tkey.irq,  (void*)&g_tkey);
	gpio_free(g_tkey.gpio);
	misc_deregister(&g_tmisckey);
	del_timer_sync(&g_ttimer);
}

module_init(key_irq_init);
module_exit(key_irq_exit);

MODULE_LICENSE("GPL");
