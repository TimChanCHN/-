#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/i2c.h>

static int i2c_driver_probe(struct i2c_client *pclient, const struct i2c_device_id *pdevice)
{
	int value;
	printk("---name:%s  addr:%d  plt_data:%s---\n", pclient->name, pclient->addr, (char*)pclient->dev.platform_data);

	// 指定要读的地址
	i2c_smbus_write_byte(pclient, 0x80);
	i2c_smbus_write_byte(pclient, 0x77);
	// 从0x8077中读取数据
	value = i2c_smbus_read_byte(pclient);
	printk("---value: %d---\n",  value);

	return 0;
}

static int i2c_driver_remove(struct i2c_client *pclient)
{
	printk("---exit---\n");
	return 0;
}



/* 从设备名称列表，存储从设备名称用于匹配 */
static struct i2c_device_id g_tarrname[] = {
		{  "i2c_touch", 123},
		{  "i2c_at24c02", 888},
		{  "i2c_nandflash"},
};

/* 驱动端结构体 */
static struct i2c_driver g_ti2cdriver = {
	.probe		=	i2c_driver_probe,
	.remove		=	i2c_driver_remove,
	
	/* 驱动设备结构体，存储驱动设备信息 */
	.driver		=	{
		.name	=	"i2c_driver",
		.owner	=	THIS_MODULE,
	},
	.id_table	=	g_tarrname,
};

static int __init iic_driver_init(void)
{
	i2c_register_driver(THIS_MODULE, &g_ti2cdriver);
	
	return 0;
}

static void __exit iic_driver_exit(void)
{
	i2c_del_driver(&g_ti2cdriver);
}

module_init(iic_driver_init);
module_exit(iic_driver_exit);
MODULE_LICENSE("GPL");