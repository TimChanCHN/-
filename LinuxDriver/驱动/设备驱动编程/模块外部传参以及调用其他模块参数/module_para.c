#include <linux/kernel.h>
#include <linux/module.h>


int a = 10;
char *p = "helloworld";
int val = 20;

module_param(a,       int,  0644);
module_param(p,      charp,  0644);


static int __init misc_key1_init(void)
{
	printk("-----module extern enter!-----\n");
	printk("---a = %d, p = %s, val = %d ---\n",a,p,val);

	return 0;
}

static void __exit misc_key1_exit(void)
{
	printk("-----module extern exit!-----\n");

}


module_init(misc_key1_init);
module_exit(misc_key1_exit);
MODULE_LICENSE("GPL");

