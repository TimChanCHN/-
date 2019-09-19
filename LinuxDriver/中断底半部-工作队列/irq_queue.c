#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/timer.h>
#include <linux/miscdevice.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <asm/uaccess.h>
#include <linux/fs.h>
#include <linux/workqueue.h>
#include <linux/delay.h>

static  struct work_struct  work;

/* 线程函数 */
void  work_queue_thread(struct work_struct *work)
{
	int i= 20;
	while(i--)
	{
		printk("***thread %d***\n",i);
		msleep(10);
	}
}

static int __init work_queue_init(void)
{
	int i = 20;

	INIT_WORK(&work, work_queue_thread);
	printk("---ready to work---\n");
	
	schedule_work(&work);
	
	while(i--)
	{
		printk("---module %d---\n",i);
		msleep(10);	
	}
	
	
	return 0;
}

static void __exit work_queue_exit(void)
{
	printk("---exit!---\n");
	cancel_work_sync(&work);
}

module_init(work_queue_init);
module_exit(work_queue_exit);
MODULE_LICENSE("GPL");


