#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/i2c.h>

static int i2c_driver_probe(struct i2c_client *pclient, const struct i2c_device_id *pdevice)
{
	int value;
	printk("---name:%s  addr:%d  plt_data:%s---\n", pclient->name, pclient->addr, (char*)pclient->dev.platform_data);

	// ָ��Ҫ���ĵ�ַ
	i2c_smbus_write_byte(pclient, 0x80);
	i2c_smbus_write_byte(pclient, 0x77);
	// ��0x8077�ж�ȡ����
	value = i2c_smbus_read_byte(pclient);
	printk("---value: %d---\n",  value);

	return 0;
}

static int i2c_driver_remove(struct i2c_client *pclient)
{
	printk("---exit---\n");
	return 0;
}



/* ���豸�����б��洢���豸��������ƥ�� */
static struct i2c_device_id g_tarrname[] = {
		{  "i2c_touch", 123},
		{  "i2c_at24c02", 888},
		{  "i2c_nandflash"},
};

/* �����˽ṹ�� */
static struct i2c_driver g_ti2cdriver = {
	.probe		=	i2c_driver_probe,
	.remove		=	i2c_driver_remove,
	
	/* �����豸�ṹ�壬�洢�����豸��Ϣ */
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