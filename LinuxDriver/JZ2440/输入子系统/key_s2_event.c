#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>

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

#define KEY2_NAME	100

struct keyinfo{
	unsigned int gpio;
	char name[12];
	int  value;
	int irq;
};

struct keyinfo g_tkeys2 = {KEY_GPF0, NAME_S2, 0, 0};

static struct input_dev* g_tinputkeys2 = NULL;

static  irqreturn_t key_irq_handler(int irq, void* arg)
{
	if( irq == g_tkeys2.irq )
	{
		static int cnt = 0;
		int value = 0;
		value = gpio_get_value(g_tkeys2.gpio);
		input_event(g_tinputkeys2,  EV_KEY,  KEY2_NAME,  value);
		cnt++;
		if( cnt == 2 )
		{
			cnt = 0;
			input_sync(g_tinputkeys2);
		}
		return IRQ_HANDLED;
	}
	return IRQ_NONE;
}

static int __init key_input_event_init(void)
{
	int ret = 0;
	/* 初始化空间 */
	g_tinputkeys2 = input_allocate_device();

	/* 初始化输入结构体 */
	g_tinputkeys2->name = "key_s2";
	set_bit(EV_KEY, g_tinputkeys2->evbit);
	set_bit(KEY2_NAME, g_tinputkeys2->keybit);

	/* 注册 */
	 ret = input_register_device(g_tinputkeys2);

	/* 中断设置 */
	gpio_request(g_tkeys2.gpio,   g_tkeys2.name);
	g_tkeys2.irq  =  gpio_to_irq(g_tkeys2.gpio);
	ret = request_irq(g_tkeys2.irq,   key_irq_handler, 
			IRQF_SHARED | IRQF_TRIGGER_RISING| IRQF_TRIGGER_FALLING, g_tkeys2.name,  (void*)&g_tkeys2);
	
	return  0;	
}


static void __exit key_input_event_exit(void)
{
	free_irq(g_tkeys2.irq,(void*)&g_tkeys2);
	gpio_free(g_tkeys2.gpio); 

	input_unregister_device(g_tinputkeys2);
	input_free_device(g_tinputkeys2);
}

module_init(key_input_event_init);
module_exit(key_input_event_exit);
MODULE_LICENSE("GPL");
