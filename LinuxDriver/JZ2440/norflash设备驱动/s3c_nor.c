#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/map.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/physmap.h>
#include <linux/mtd/concat.h>
#include <linux/io.h>

#define NOR_BASE_ADDRESS	0

static struct map_info* s3c_nor_map;
static struct mtd_info* s3c_nor_mtd;

static struct mtd_partition s3c_nor_part[] = {
	[0] = {
		.name	= "bootloader_nor",
		.size	= 0x00040000,
		.offset	= 0,
	},
	[1] = {
		.name	= "rootfs_nor",
		.offset	= MTDPART_OFS_APPEND,
		.size	= MTDPART_SIZ_FULL,
	}
};


static int __init s3c_nor_init(void)
{
	/* 1. 分配map_info结构体 */
	s3c_nor_map  = kzalloc(sizeof(struct map_info), GFP_KERNEL);
	
	/* 2. 设置：物理基地址，大小，位宽，虚拟基地址 */
	s3c_nor_map->name = "s3c_nor";
	s3c_nor_map->phys = 0;
	s3c_nor_map->size = 0x1000000;				/* 16M,要大于nor   flash 的实际大小      */
	s3c_nor_map->bankwidth = 2;					/* 最小位宽是8，此处填8的倍数 */
	s3c_nor_map->virt = ioremap(s3c_nor_map->phys, s3c_nor_map->size);
	simple_map_init(s3c_nor_map);
	
	/* 3. 使用NOR FLASH协议层提供的函数来识别nor flash */
	printk("using cfi \n");
	s3c_nor_mtd = do_map_probe("cfi_probe", s3c_nor_map);
	if( !s3c_nor_mtd )
	{
		printk("turn to using jedec \n");
		s3c_nor_mtd = do_map_probe("jedec_probe", s3c_nor_map);
	}

	if( !s3c_nor_mtd )
	{
		printk("do map probe fail\n");
		iounmap(s3c_nor_map->virt);
		kfree(s3c_nor_map);
		return -EIO;
	}
	
	/* 4. add_mtdpartition */
	mtd_device_parse_register(s3c_nor_mtd, NULL, NULL, s3c_nor_part, 2);

	return 0;
}

static void __exit s3c_nor_exit(void)
{
	mtd_device_unregister(s3c_nor_mtd);
	iounmap(s3c_nor_map->virt);
	kfree(s3c_nor_map);
}

module_init(s3c_nor_init);
module_exit(s3c_nor_exit);
MODULE_LICENSE("GPL");



