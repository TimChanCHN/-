1、misc_fops.c	:	验证file结构体的private私有化数据可以在模块中充当全局变量的角色，
			模拟read/write函数中，文件偏移移动的情况
2、misc_key.c	:	按键的杂项字符设备驱动
3、misc_led.c	:	LED的杂项字符设备驱动
4、iocontrol.c	:	iocontrol可以通过外部传参调用相关函数，外部传参是传递命令。app中的main函数专门用于该设备驱动