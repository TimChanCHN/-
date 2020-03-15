#include <linux/module.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/ioport.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/clk.h>
#include <linux/cpufreq.h>

#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/nand_ecc.h>
#include <linux/mtd/partitions.h>

#include <asm/io.h>

#include <plat/regs-nand.h>
#include <plat/nand.h>

struct s3c_nand_regs{
	unsigned long nfconf;
	unsigned long nfcont;
	unsigned long nfcmd;
	unsigned long nfaddr;
	unsigned long nfdata;
	unsigned long nfeccd0;
	unsigned long nfeccd1;
	unsigned long nfeccd;
	unsigned long nfstat;
	unsigned long nfestat0;
	unsigned long nfestat1;
	unsigned long nfmecc0;
	unsigned long nfmecc1;
	unsigned long nfsecc;
	unsigned long nfsblk;
	unsigned long nfeblk;

};

static struct nand_chip* m_nandchip;
static struct mtd_info*  m_mtd;
static struct s3c_nand_regs* m_s3c_nand_regs;

static struct mtd_partition smdk_default_nand_part[] = {
	[0] = {
		.name	= "bootloader",
		.size	= SZ_256K,
		.offset	= 0,
	},
	[1] = {
		.name	= "params",
		.offset = MTDPART_OFS_APPEND,
		.size	= SZ_128K,
	},
	[2] = {
		.name	= "kernel",
		.offset = MTDPART_OFS_APPEND,
		.size	= SZ_4M,
	},
	[3] = {
		.name	= "rootfs",
		.offset	= MTDPART_OFS_APPEND,
		.size	= MTDPART_SIZ_FULL,
	}
};


void s3c2440_select_chip(struct mtd_info *mtd, int chip)
{
	if( chip == -1 )
	{
		/* 取消片选, NFCONT[bit1]为1 */
		m_s3c_nand_regs->nfcont |= (1<<1); 
	}
	else
	{
		/* 选择芯片 */
		m_s3c_nand_regs->nfcont &= ~(1<<1); 
	}

}

void s3c2440_cmd_ctl(struct mtd_info *mtd, int dat, unsigned int ctrl)

{
	if( ctrl & NAND_CLE )
	{
		// 发命令， NFCMD = dat	
		m_s3c_nand_regs->nfcmd = dat;
	}
	else
	{
		// 发地址, NFADDR = dat
		m_s3c_nand_regs->nfaddr = dat;
	}
}

static int s3c2440_dev_ready(struct mtd_info *mtd)
{
	return (m_s3c_nand_regs->nfstat&(1<<0));
}


static int __init s3c_nand_init(void)
{
	struct clk* nand_clk;
	int scan_ret;
	/* 1.分配nand_chip结构体 */
	m_nandchip = kzalloc(sizeof(struct nand_chip), GFP_KERNEL);
	m_s3c_nand_regs = ioremap(0x4E000000, sizeof(struct s3c_nand_regs));
	
	/* 2.设置nand_chip */
	m_nandchip->select_chip =	s3c2440_select_chip;
	m_nandchip->cmd_ctrl	=	s3c2440_cmd_ctl;
	// NFDATA寄存器的虚拟地址
	m_nandchip->IO_ADDR_R	=	&m_s3c_nand_regs->nfdata;
	m_nandchip->IO_ADDR_W	=	&m_s3c_nand_regs->nfdata;
	m_nandchip->dev_ready	=	s3c2440_dev_ready;
	m_nandchip->ecc.mode 	= NAND_ECC_SOFT;
	
	/* 3.硬件相关操作，根据NAND FLASH手册设置时间参数 */
	/* 使能nand时钟 */
	nand_clk = clk_get(NULL, "nand");
	clk_enable(nand_clk);
	
	/* HCLK = 100MHz, 只需设置NFCONF寄存器的TACLS，TWRPH0，TWRPH1
	 * TACLS ： ALE/CLE命令发出多长时间后，才可以发WE命令，有NANDFLASH数据手册可知，
	 * ALE/CLE命令可以和WE命令同时发出,因此 TACLS = 0
	 * TWRPH0 : WE脉冲宽度>=12ns，HCLK*(TWRPH0+1),故TWRPH0=1
	 * TWRPH1 ：WE变为高电平后多长时间ALE/CLE才可以恢复，该时间是>=5ns，故TWRPH1>=0
	 */
	 
#define TACLS  0
#define TWRPH0 1
#define TWRPH1 0
	m_s3c_nand_regs->nfconf = (TACLS<<12) | (TWRPH0 << 8) | (TWRPH1 <<4 );

	/*
	 * NFCONT: 
	 * [bit1] : 设为1，取消片选
	 * [bit0] : 设为1，设能nand flash控制器
	 */
	m_s3c_nand_regs->nfcont = (1<<1) | (1<<0);

	
	/* 4.使用：nand_scan--在linux3以上版本，nand_scan拆分成两个函数 */
	m_mtd = kzalloc(sizeof(struct mtd_info), GFP_KERNEL); 
	m_mtd->priv = m_nandchip;
	m_mtd->owner = THIS_MODULE;
	
	/* 识别nand_flash,构造mtd_info */
	scan_ret = nand_scan_ident(m_mtd, 1, NULL);
	if( scan_ret == 0 )
	{
		nand_scan_tail(m_mtd);
		
		/* 5.add_mtd_partitions */
		/* 分区 */ 
		mtd_device_parse_register(m_mtd, NULL, NULL, smdk_default_nand_part,4);
	}

	return 0;
}

static void __exit s3c_nand_exit(void)
{
	nand_release(m_mtd);
	kfree(m_mtd);
	iounmap(m_s3c_nand_regs);
	kfree(m_nandchip);
}

module_init(s3c_nand_init);
module_exit(s3c_nand_exit);
MODULE_LICENSE("GPL");



