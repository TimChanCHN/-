#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>


static int __init dts_test_init(void)
{
	struct device_node* np = NULL;
	int lenp = 0;
	const char *p = NULL;
	u32 regarr[4]={0};
	const int* value = 0;
	
	/* 获取设备节点 */
	np = of_find_node_by_name(NULL,"z_led-power");

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

	
	/* 获取设备节点 */
	np = of_find_node_by_path("/i2c@ff3c0000/mpu6050@39");

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
		value = of_get_property( np,  "reg",  &lenp);
		printk("---reg:0x%x  lenp:%d---\n",*value, lenp);
	}
	
	return 0;
}

static void __exit dts_test_exit(void)
{
	printk("---exit---\n");

}

module_init(dts_test_init);
module_exit(dts_test_exit);

MODULE_LICENSE("GPL");

