#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/serial_core.h>

#include <mach/map.h>
#include <mach/dma.h>
#include <linux/dma-mapping.h>

#include <plat/cpu.h>
#include <plat/dma-s3c24xx.h>
#include <linux/miscdevice.h>
#include <plat/regs-serial.h>
#include <mach/regs-gpio.h>
#include <plat/regs-ac97.h>
#include <plat/regs-dma.h>
#include <mach/regs-mem.h>
#include <mach/regs-lcd.h>
#include <mach/regs-sdi.h>
#include <plat/regs-iis.h>
#include <plat/regs-spi.h>

#define CP_NO_DMA	'K'

#define MEM_COPY_NO_DMA	 _IOR(CP_NO_DMA, 0, char)
#define MEM_COPY_DMA	 _IOR(CP_NO_DMA, 1, char)

#define BUFF_SIZE		(512*1024)
#define DISRC0		0x4B000000
#define DISRC1		0x4B000040
#define DISRC2		0x4B000080
#define DISRC3		0x4B0000C0


struct s3c_dma_regs{
	unsigned long disrc;
	unsigned long disrcc;
	unsigned long didst;
	unsigned long didstc;
	unsigned long dcon;
	unsigned long dstat;
	unsigned long dcsrc;
	unsigned long dcdst;
	unsigned long dmasktrig;
};

static char *src;
static u32 src_phy;

static char *des;
static u32 des_phy;

static int major = 0;						// 主设备号
static struct class *cls;
static struct s3c_dma_regs* dma_regs;
static DECLARE_WAIT_QUEUE_HEAD(dma_waitq);
static int ev_dma = 0;

static long dma_ioctl (struct file *fp, unsigned int cmd, unsigned long args)
{
	int i; 
	
	memset(src, 0xAA, BUFF_SIZE);
	memset(des, 0x55, BUFF_SIZE);
	
	switch(cmd)
	{
		case MEM_COPY_NO_DMA:
			for( i = 0; i < BUFF_SIZE; i++ )
				des[i] = src[i];
			if( memcmp(des, src, BUFF_SIZE) == 0)
			{
				printk("copy no dma success!\n");
			}
			else
			{
				printk("copy no dma fail!\n");
			}
			break;

		case MEM_COPY_DMA:
			ev_dma = 0;
			dma_regs->disrc 	= 	src_phy;
			dma_regs->disrcc	=	(0 << 1) | (0 << 0);
			dma_regs->didst 	= 	des_phy;
			dma_regs->didstc	=	(0 << 2) | (0 << 1) | (0 << 0);
			dma_regs->dcon		=	(1<<30) | (1<<29) | (1<<28) | (1<<27) | (0<<23) | (0<<20) | BUFF_SIZE;
			dma_regs->dmasktrig	=	(1<<1) | (1<<0);

			// 设置成功后，需要进入休眠态，等待传输完成
			wait_event_interruptible(dma_waitq, ev_dma);
			
			if( memcmp(des, src, BUFF_SIZE) == 0)
			{
				printk("copy with dma success!\n");
			}
			else
			{
				printk("copy with dma fail!\n");
			}
			break;

		default:
			printk("cmd error\n");
			break;
	}	
	return 0;
}

irqreturn_t dma_irq_handler(int irq, void *args)
{
	ev_dma = 1;
	wake_up_interruptible(&dma_waitq);
	return IRQ_HANDLED;
}

static struct file_operations m_tdma_fops={
	.owner 	= 	THIS_MODULE,
	.unlocked_ioctl	=	dma_ioctl,
};

static struct miscdevice m_tdma={
	.minor	=	MISC_DYNAMIC_MINOR,
	.name	=	"s3c_dma",
	.fops	=	&m_tdma_fops,
};

static int __init s3c_dma_init(void)
{
	/* 分配中断，等DMA传输成功后，通知CPU */
	if( request_irq(IRQ_DMA3, dma_irq_handler, 0, "s3c_dma", NULL) )
	{
		printk("can't request irq for dma.\n");
		return -EBUSY;
	}

	// 分配内存空间给src/des缓存区
	src = dma_alloc_writecombine(NULL, BUFF_SIZE, &src_phy, GFP_KERNEL);
	if( NULL == src )
	{
		printk(" dma_alloc for src fail.\n");
		dma_free_writecombine(NULL, BUFF_SIZE, src, src_phy);
		free_irq(IRQ_DMA3, NULL);
		return -ENOMEM;
	}

	//des = kzalloc(size_t size, gfp_t flags)
	des = dma_alloc_writecombine(NULL, BUFF_SIZE, &des_phy, GFP_KERNEL);
	if( NULL == des )
	{
		printk("dma_alloc for des fail.\n");
		dma_free_writecombine(NULL, BUFF_SIZE, des, des_phy);
		free_irq(IRQ_DMA3, NULL);
		return -ENOMEM;
	}

	/* 注册对应的杂项字符设备 */
	misc_register(&m_tdma);

	dma_regs = ioremap(DISRC3, sizeof(struct s3c_dma_regs));
	return 0;
}

static void __exit s3c_dma_exit(void)
{
	iounmap(dma_regs);
	misc_deregister(&m_tdma);
	dma_free_writecombine(NULL, BUFF_SIZE, des, des_phy);
	dma_free_writecombine(NULL, BUFF_SIZE, src, src_phy);
	free_irq(IRQ_DMA3, NULL);
}

module_init(s3c_dma_init);
module_exit(s3c_dma_exit);
MODULE_LICENSE("GPL");


