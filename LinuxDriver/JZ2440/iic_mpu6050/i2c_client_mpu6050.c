#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/i2c.h>

/* 客户端结构体 */
static struct i2c_client* m_pti2cmpu6050 = NULL;

/* 客户端信息结构体 */
static struct i2c_board_info m_tmpu6050info = {
	.type			=	"i2c_mpu6050",
	.addr			=	0x68,
	.platform_data	=	"This mpu6050",
};

static int __init iic_client_mpu6050_init(void)
{
	/* 获取适配器 */
	struct i2c_adapter* adapter = NULL;
	adapter = i2c_get_adapter(0);
	m_pti2cmpu6050 = i2c_new_device(adapter, &m_tmpu6050info);
	i2c_put_adapter(adapter);
	return 0;
}

static void __exit iic_client_mpu6050_exit(void)
{
	i2c_unregister_device(m_pti2cmpu6050);
}

module_init(iic_client_mpu6050_init);
module_exit(iic_client_mpu6050_exit);
MODULE_LICENSE("GPL");


