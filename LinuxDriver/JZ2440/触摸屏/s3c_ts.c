#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/input.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/io.h>
#include <linux/timer.h>

#include <plat/adc.h>
#include <plat/regs-adc.h>
#include <plat/ts.h>



static struct ts_regs{
	unsigned long adccon;
	unsigned long adctsc;
	unsigned long adcdly;
	unsigned long adcdat0;
	unsigned long adcdat1;
	unsigned long adcupdn;
};

static struct input_dev* s3c_ts_dev;
static struct ts_regs* s3c_ts_regs;
static struct timer_list	ts_timer;

static void enter_wait_pen_down_mode(void)
{
	s3c_ts_regs->adctsc = 0xd3;
}

static void enter_wait_pen_up_mode(void)
{
	s3c_ts_regs->adctsc = 0x1d3;
}

static void enter_wait_measure_xy_mode(void)
{
	s3c_ts_regs->adctsc |= (1<<3) + (1<<2);					//禁止上拉
}

static void start_adc(void)
{
	s3c_ts_regs->adccon |= (1<<0);
}

static irq_handler_t pen_down_up_handler(int irq, void* arg)
{
	if( s3c_ts_regs->adcdat0 & (1<<15) )
	{
		//printk("pen is up.\n");
		enter_wait_pen_down_mode();
	}
	else
	{
		//printk("pend is down.\n");
		//enter_wait_pen_up_mode();
		enter_wait_measure_xy_mode();
		start_adc();
	}
	return IRQ_HANDLED;
}

static irq_handler_t adc_irq_handler(int irq, void* arg)
{
	static int cnt = 0;
	int adcdat0, adcdat1;
	
	/* 改进2：在ADC还没结束前，发现pen up，丢弃这次结果 */
	adcdat0 = s3c_ts_regs->adcdat0;
	adcdat1 = s3c_ts_regs->adcdat1;

	if( s3c_ts_regs->adcdat0 & (1<<15) )
	{
		enter_wait_pen_down_mode();
	}
	else
	{
		//printk("ts adc cnt = %d, x = %d, y = %d\n", ++cnt, adcdat0 & 0x3ff, adcdat1 & 0x3ff);
		enter_wait_pen_up_mode();
		mod_timer(&ts_timer,jiffies + HZ/100);
	}
	return IRQ_HANDLED;
}

static void ts_timer_handler(unsigned long argc)
{
	if( s3c_ts_regs->adcdat0 & (1<<15))
	{
		//input_report_abs(s3c_ts_dev, ABS_PRESSURE, 0);
		//input_report_key(s3c_ts_dev, BTN_TOUCH, 0);
		
		input_event(s3c_ts_dev, EV_ABS, ABS_X, s3c_ts_regs->adcdat0 & 0x3ff);
		input_event(s3c_ts_dev, EV_ABS, ABS_Y, s3c_ts_regs->adcdat1 & 0x3ff);
		
		input_sync(s3c_ts_dev);
		enter_wait_pen_down_mode();
	}
	else
	{
		enter_wait_measure_xy_mode();
		start_adc();
	}
}

static int __init s3c2440_ts_init(void)
{
	int ret;
	struct clk* ts_clk;
	
	/* 1.分配一个input_dev结构体 */
	s3c_ts_dev = input_allocate_device();
	
	/* 2.设置 */
	s3c_ts_dev->name = "s3c_ts";
	
	/* 2.1能产生那类事件 */
	set_bit(EV_KEY,  s3c_ts_dev->evbit);
	set_bit(EV_ABS,  s3c_ts_dev->evbit);
	
	/* 2.2能产生这类事件的什么事件 */
	set_bit(BTN_TOUCH,  s3c_ts_dev->keybit);
	input_set_abs_params(s3c_ts_dev, ABS_X, 0, 0x3FF, 0, 0);
	input_set_abs_params(s3c_ts_dev, ABS_Y, 0, 0x3FF, 0, 0);
	input_set_abs_params(s3c_ts_dev, ABS_PRESSURE, 0, 1, 0, 0);
	
	
	/* 3.注册 */
	ret = input_register_device(s3c_ts_dev);

	/* 4.硬件相关操作 */
	/* 4.1 使能时钟 */
	ts_clk = clk_get(NULL, "adc");
	clk_enable(ts_clk); 

	/* 4.2 设置S3C2440的ADC/TS相关寄存器设置 	         */
	s3c_ts_regs = ioremap(0x58000000, sizeof(struct ts_regs));

	/*
	 * bit[14] : 使能预分频
 	 * bit[13:6] : 预分频系数，PCLK=50MHz，ADCCLK = PCLK/(49+1) = 1MHZ
 	 * bit[0]	: 打开ADC
	 */
	 s3c_ts_regs->adccon = (1<<14) | (49<<6);

	 request_irq(IRQ_TC, pen_down_up_handler, IRQF_SAMPLE_RANDOM, 
	 			"s3c_ts", NULL);							// 最后一个参数为传参
	 request_irq(IRQ_ADC, adc_irq_handler, IRQF_SAMPLE_RANDOM, 
	 			"ts_adc", NULL);							


	/* 改进1：加入延时，待ADC采样稳定后再读值 */
	s3c_ts_regs->adcdly = 0xffff;

	/* 加上定时器，用于处理长按/滑动的情况 */
	init_timer(&ts_timer);
	ts_timer.function	=	ts_timer_handler;
	add_timer(&ts_timer);

	enter_wait_pen_down_mode();
	
	return 0;
}

static void __exit s3c2440_ts_exit(void)
{
	del_timer(&ts_timer);
	free_irq( IRQ_TC, NULL);
	free_irq( IRQ_ADC, NULL);
	iounmap(s3c_ts_regs);
	input_free_device(s3c_ts_dev);
}

module_init(s3c2440_ts_init);
module_exit(s3c2440_ts_exit);

MODULE_LICENSE("GPL");


