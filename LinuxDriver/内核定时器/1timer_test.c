#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/timer.h>


/* 终端服务函数 */
void	tim_irq(unsigned long arg)
{
	char* p = (char*)arg;
	printk("---enter timer irq__%s---\n", p);	
}


/* 时间结构体定义,不能初始化，否则该结构体不能挂到timer总线上 */
static struct timer_list timer1;


static int __init timer_test_init(void)
{
	setup_timer(&timer1,  tim_irq,  (unsigned long)"keke");
	mod_timer(&timer1, jiffies + 3*HZ);
	printk("-----timer start!!!-----\n");
	return 0;
}

static void __exit timer_test_exit(void)
{
	printk("-----exit!!!-----\n");
	del_timer_sync(&timer1);
}


module_init(timer_test_init);
module_exit(timer_test_exit);
MODULE_LICENSE("GPL");


