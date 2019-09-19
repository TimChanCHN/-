#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/gpio.h>
#include <asm/uaccess.h>

#define GPIO_KEY	5
#define NAME_KEY	"RK3399_KEY"

char *p = "hello";
int a = 25;

int add(int a, int b)
{
	return a+b;
}

EXPORT_SYMBOL(a);
EXPORT_SYMBOL(p);
EXPORT_SYMBOL(add);

static int __init misc_key1_init(void)
{
	printk("-----module export enter!-----\n");


	return 0;
}

static void __exit misc_key1_exit(void)
{
	printk("-----module export exit!-----\n");

}


module_init(misc_key1_init);
module_exit(misc_key1_exit);
MODULE_LICENSE("GPL");


