#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/gpio.h>
#include <asm/uaccess.h>

#define GPIO_KEY	5
#define NAME_KEY	"RK3399_KEY"

extern char *p;
extern int a;
extern int add(int,int);
static int __init misc_key1_init(void)
{
	printk("-----module extern enter!-----\n");
	printk("---a = %d, p = %s, add(2+3) = %d ---\n",a,p,add(2,3));

	return 0;
}

static void __exit misc_key1_exit(void)
{
	printk("-----module extern exit!-----\n");

}


module_init(misc_key1_init);
module_exit(misc_key1_exit);
MODULE_LICENSE("GPL");



