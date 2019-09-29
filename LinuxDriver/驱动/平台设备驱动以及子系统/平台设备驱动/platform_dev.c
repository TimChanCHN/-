#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/ioport.h>
#include <linux/platform_device.h>

#define GPIO_KEY	5
#define GPIO_NAME	"PWR_KEY"

/* 设备结构体--release出口函数 */
static void platform_device_release(struct device *dev)
{
	
}

/* 设备资源结构体定义 */
static struct resource g_tarrresource[]={
	[0]={
		.start	=	GPIO_KEY,
		.end	=	GPIO_KEY,
		.name	=	GPIO_NAME,
		.flags	=	IORESOURCE_IRQ,
	},
};

/* 平台设备 设备端结构体定义 */
static struct platform_device g_tplatdev={
	.name			=	"plat_botton",
	.id				= 	-1,
	.dev			=	{
		.release	=	platform_device_release,
	},
	.resource		=	g_tarrresource,
	.num_resources	=	ARRAY_SIZE(g_tarrresource),
};

static int __init platform_dev_init(void)
{
	printk("---enter device  module---\n");
	platform_device_register(&g_tplatdev);
	return 0;
}

static void __exit platform_dev_exit(void)
{
	printk("---exit device  module---\n");
	platform_device_unregister(&g_tplatdev);
}

module_init(platform_dev_init);
module_exit(platform_dev_exit);
MODULE_LICENSE("GPL");

