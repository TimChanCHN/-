#include <linux/kernel.h>
#include <linux/module.h>
#include <asm/uaccess.h>
#include <linux/gpio.h>
#include <linux/fs.h>

#define GPIO_KEY	5
#define NAME_KEY	"RK3399_KEY"


ssize_t key_chrdev_read(struct file *fp, char __user *buffer, size_t len, loff_t *offset)
{
	int value = 0, ret = 0;
	value = gpio_get_value(GPIO_KEY);
	ret = copy_to_user(buffer, &value, sizeof(value));

	return 0;
}


struct file_operations g_tfops={
	.owner	=	THIS_MODULE,
	.read	=	key_chrdev_read,
};

static int major = 0;

static int __init chrdev_key_init(void)
{
	printk("-----chrdev module enter!-----\n");
	/* 早期字符设备申请 */
	major = register_chrdev(0, "chrdev key", &g_tfops);
	printk("-----major :   %d -----\n",major);
	
	/* 外部资源申请 */
	gpio_request(GPIO_KEY, NAME_KEY);
	gpio_direction_input(GPIO_KEY);
	
	return 0;
}

static void __exit chrdev_key_exit(void)
{
	printk("-----chrdev module exit!-----\n");
	gpio_free(GPIO_KEY);
	unregister_chrdev( major, "chrdev key");
}

module_init(chrdev_key_init);
module_exit(chrdev_key_exit);
MODULE_LICENSE("GPL");

