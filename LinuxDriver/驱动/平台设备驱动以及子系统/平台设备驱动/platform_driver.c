#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/miscdevice.h>
#include <asm/uaccess.h>
#include <linux/fs.h>
#include <linux/wait.h>
#include <linux/timer.h>
#include <linux/delay.h>
#include <linux/poll.h>

#define GPIO_KEY	5
#define GPIO_NAME	"PWR_KEY"

struct  keyinfo  {
	char  name[12];
	unsigned int  gpio;
	int  value;
	int  irq;
};
static   struct  keyinfo  g_tkey;

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
		.name	=	"key_plat",
		.fops		=	&g_tfops,	
};

/* 平台管理设备入口函数 */
static int  platdri_probe(struct platform_device *pdev)
{
	int ret = 0;
	struct resource *resp = NULL;
	printk("---%s matched---\n", pdev->name);

	resp = platform_get_resource(pdev,IORESOURCE_IRQ, 0);
	strcpy(g_tkey.name,  resp->name);	
	g_tkey.gpio  =  resp->start;

	gpio_request(g_tkey.gpio,  g_tkey.name);
	g_tkey.irq  =  gpio_to_irq(g_tkey.gpio);
	ret = request_irq(g_tkey.irq,   key_irq_handler, 
			IRQF_SHARED | IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING, 
			g_tkey.name,  (void*)&g_tkey);

	misc_register(&g_tmisckey);
	setup_timer(&g_ttimer,  key_timer_handler,  0);
	return  0;
}

/* 平台管理设备出口函数 */
static int  platdri_remove(struct platform_device *pdev)
{
	printk("---this  is  plat  remove---\n");

	free_irq(g_tkey.irq,  (void*)&g_tkey);
	gpio_free(g_tkey.gpio);
	misc_deregister(&g_tmisckey);
	del_timer_sync(&g_ttimer);

	return  0;
}

/* 设备端名称匹配列表 */
static struct platform_device_id g_tdritbl[]={
	{"plat_botton",12},
	{"plat_led"},
	{"plat_lcd",88},
	{"plat_key"},
};


/* 驱动设备结构体定义 */
/*
static struct device_driver g_tdevdri={
	.name		=	"TIMCHAN",
	.owner		=	THIS_MODULE,
};
*/

/* 平台设备 驱动端结构体定义 */
static struct platform_driver g_tplatdri={
	.probe		=	platdri_probe,
	.remove		=	platdri_remove,
	.driver		=	{
		.name	=	"TIMCHAN",
		.owner	=	THIS_MODULE,
	},
	.id_table	=	g_tdritbl,
};

static int __init platform_dri_init(void)
{
	printk("---enter device  module---\n");
	platform_driver_register(&g_tplatdri);
	return 0;
}

static void __exit platform_dri_exit(void)
{
	printk("---exit device  module---\n");
	platform_driver_unregister(&g_tplatdri);
}

module_init(platform_dri_init);
module_exit(platform_dri_exit);
MODULE_LICENSE("GPL");

