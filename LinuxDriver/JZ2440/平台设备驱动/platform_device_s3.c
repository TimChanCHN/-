#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>

#define KEY_GPF0		(5*32 + 0)
#define NAME_S2			"KEY_S2"
#define KEY_GPF2		(5*32 + 2)
#define NAME_S3			"KEY_S3"
#define KEY_GPG3		(6*32 + 3)
#define NAME_S4			"KEY_S4"

#define LED_GPF4		(5*32 + 4)
#define NAME_LED1		"LED1"
#define LED_GPF5		(5*32 + 5)
#define NAME_LED2		"LED2"

struct keyinfo{
	unsigned int gpio;
	char		 name[12];
	int 		 value;
	int			 irq;
};

static void	platform_release(struct device *dev)
{}

struct resource g_tresource[] = {
	{	.start	=	KEY_GPF2,
		.end	=	KEY_GPF2,
		.name	=	"button_S3",
		.flags	=	IORESOURCE_IRQ,
	},
};


struct platform_device g_tplatdevice={
	.name		=	"platkey_s3",
	.id			=	-1,
	.dev		={
		.release	=	platform_release,
	},
	.resource	=	g_tresource,
	.num_resources	=	ARRAY_SIZE(g_tresource),
};

static int __init platform_device_init(void)
{
	platform_device_register(&g_tplatdevice);
	return 0;
}

static void __exit platform_device_exit(void)
{
	platform_device_unregister(&g_tplatdevice);
}

module_init(platform_device_init);
module_exit(platform_device_exit);
MODULE_LICENSE("GPL");

