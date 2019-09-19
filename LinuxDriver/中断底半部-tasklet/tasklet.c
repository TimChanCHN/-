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

static  struct tasklet_struct  tasklet;

/* 线程函数 */
static void thread_func(unsigned long arg)
{
	int i = 20;
	while(i--)
	{
		printk("***task %d***\n",i);
		//msleep(10);	
	}
}
.poll
static int __init task_let_init(void)
{
	int i = 20;

	tasklet_init(&tasklet,  thread_func,  0);
	printk("---ready to work---\n");
	
	tasklet_schedule(&tasklet);
	
	while(i--)
	{
		printk("---module %d---\n",i);
		msleep(10);	
	}
	
	return 0;
}

static void __exit task_let_exit(void)
{
	tasklet_kill(&tasklet);
}

module_init(task_let_init);
module_exit(task_let_exit);
MODULE_LICENSE("GPL");



