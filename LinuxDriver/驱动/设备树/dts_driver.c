#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/device.h>
#include <linux/of_gpio.h>


static int dts_probe(struct platform_device *pdev)
{
	struct device_node* np = NULL;
	int lenp = 0;
	const char *p = NULL;
	u32 regarr[4]={0};
	
	/* 获取设备节点 */
	np = pdev->dev.of_node;

	/* 验证该节点是否有指定属性值 */
	if( of_find_property(np, "compatible", NULL) )
	{
		/* 获取特定节点指定属性的值 */
		p = of_get_property( np,  "compatible",  &lenp);
		printk("---compatible:%s  lenp:%d---\n",p, lenp);
	}

	/* 验证该节点是否有指定属性值 */
	if( of_find_property(np, "reg", NULL) )
	{
		/* 获取特定节点指定属性的值 */
		p = of_get_property( np,  "reg",  &lenp);
		of_property_read_u32_array( np, "reg", regarr, sizeof(regarr)/sizeof(regarr[0]));
		printk("---a[0]:0x%x a[1]:0x%x a[2]:0x%x a[3]:0x%x---\n",regarr[0], regarr[1],regarr[2],regarr[3]);
	}

	printk("========================================\n");
	return 0;

}

static int dts_remove(struct platform_device *p)
{
	printk("---exit!---\n");
	return 0;
}

static struct of_device_id g_tarrname[]={
	[0] = {.compatible = "keke"},
	[1] = {.compatible = "hehe"},
	[2] = {.compatible = "haha"},
	[3] = {.compatible = "key_pwr "},
	[4] = {.compatible = "led-xyd"},
	[5] = {.compatible = "mpu6050"},
};

static struct platform_driver g_tdtsdri={
	.probe	 =	dts_probe,
	.remove	 =	dts_remove,
	.driver	 =	{
		.name	=	"123",			//驱动名称
		.owner	=	THIS_MODULE,
		.of_match_table	=	g_tarrname,
	},
};

static int __init dts_driver_init(void)
{
	/* 平台设备驱动注册函数 */
	platform_driver_register(&g_tdtsdri);
	return 0;
}

static void __exit dts_driver_exit(void)
{
	/* 平台设备驱动注销函数 */
	platform_driver_unregister(&g_tdtsdri);
}

module_init(dts_driver_init);
module_exit(dts_driver_exit);
MODULE_LICENSE("GPL");

