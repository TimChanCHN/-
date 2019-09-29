#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/i2c.h>

/* 客户端结构体 */
static struct i2c_client* g_ti2cclient = NULL;

/* 设备信息结构体 */
static struct i2c_board_info g_clientinfo = {
	.type			=	"i2c_touch",
	.addr			=	0x5d,
	.platform_data	=	"This is i2c",
};

static int __init iic_client_init(void)
{
	/* 获取适配器 */
	struct i2c_adapter*  adapter = NULL;
	adapter = i2c_get_adapter(4);
	g_ti2cclient = i2c_new_device(adapter, &g_clientinfo);
	i2c_put_adapter(adapter);
	
	return 0;
}

static void __exit iic_client_exit(void)
{
	i2c_unregister_device(g_ti2cclient);
}

module_init(iic_client_init);
module_exit(iic_client_exit);
MODULE_LICENSE("GPL");