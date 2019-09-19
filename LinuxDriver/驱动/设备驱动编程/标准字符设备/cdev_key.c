#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <asm/uaccess.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>

#define BASEMINOR 	0
#define MINORCNT	5
#define DEVNAME		"cdev_key"

#define GPIO_KEY	5
#define NAME_KEY	"RK3399_KEY"

ssize_t key_cdev_read(struct file *fp, char __user *buffer, size_t len, loff_t *offset)
{
	int value = 0, ret = 0;
	value = gpio_get_value(GPIO_KEY);
	ret = copy_to_user(buffer, &value, sizeof(value));

	return 0;
}

static struct file_operations g_tfops={
	.owner	=	THIS_MODULE,
	.read	=	key_cdev_read,

};

static dev_t g_devnum;
static struct cdev* g_tcdev = NULL;

static int __init cdev_key_init(void)
{
	alloc_chrdev_region(&g_devnum, BASEMINOR, MINORCNT, DEVNAME);
	g_tcdev = cdev_alloc();
	cdev_init( g_tcdev,  &g_tfops);
	cdev_add(g_tcdev, g_devnum, MINORCNT);
	printk("----cdev major: %d   minor:%d-----\n",MAJOR((g_devnum)),MINOR(g_devnum));

	/* 外部资源申请 */
	gpio_request(GPIO_KEY, NAME_KEY);
	gpio_direction_input(GPIO_KEY);
	return 0;
}

static void __exit cdev_key_exit(void)
{
	cdev_del(g_tcdev);
	kfree(g_tcdev);
	unregister_chrdev_region( g_devnum, MINORCNT);
	gpio_free(GPIO_KEY);
}

module_init(cdev_key_init);
module_exit(cdev_key_exit);
MODULE_LICENSE("GPL");

