#include <linux/module.h>
#include <linux/errno.h>
#include <linux/interrupt.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/timer.h>
#include <linux/genhd.h>
#include <linux/hdreg.h>
#include <linux/ioport.h>
#include <linux/init.h>
#include <linux/wait.h>
#include <linux/blkdev.h>
#include <linux/mutex.h>
#include <linux/blkpg.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/gfp.h>

#include <asm/uaccess.h>
#include <asm/dma.h>

#define RAMBLOCK_SIZE (1024*1024)

static struct gendisk* ramblock_disk;
struct request_queue*  ramblock_queue;
static DEFINE_SPINLOCK(ramblock_lock);
static int major;
static char* ramblock_buff;

/* 该fops支持建立分区 */
static int ramblock_getgeo(struct block_device *bdev, struct hd_geometry *geo)
{
	geo->heads = 2;
	geo->sectors = 32;
	geo->cylinders = RAMBLOCK_SIZE/2/32/512;
	return 0;
}


static const struct block_device_operations ramblock_fops = {
	.owner	= THIS_MODULE,
	.getgeo	= ramblock_getgeo,
};

static void do_ramblock_request(struct request_queue * q)
{
	static int r_cnt = 0;
	static int w_cnt = 0;
	struct request* req;
	
	req = blk_fetch_request(q);
	while (req) {
	/* 数据传输三要素：源, 目的, 长度  */
	/* 源 */
	unsigned long offset = blk_rq_pos(req) * 512;

	/* 目的 */
	// req->buffer
	
	/* 长度 */
	unsigned long len  = blk_rq_cur_bytes(req);


	if( rq_data_dir(req) == READ )
	{
		memcpy(req->buffer, ramblock_buff+offset, len);
		//printk("read cnt: %d\n", ++r_cnt);
	}
	else
	{
		memcpy(ramblock_buff+offset,req->buffer,  len);
		//printk("write cnt: %d\n", ++w_cnt);
	}

	if (!__blk_end_request_cur(req, 0))
		req = blk_fetch_request(q);
	}
}


static int __init ramblock_init(void)
{
	/* 1. 分配gendisk结构体 */
	ramblock_disk = alloc_disk(1);			/* 次设备号个数：分区格式+1 */
	
	/* 2. 设置 */
	/* 2.1 分配/设置队列：提供读写能力 */
	ramblock_queue = blk_init_queue(do_ramblock_request, &ramblock_lock);
	ramblock_disk->queue = ramblock_queue;
	
	/* 2.2 设置其他属性：如容量 */
	major = register_blkdev(0, "ramblock");
	ramblock_disk->major 			= major;
	ramblock_disk->first_minor 		= 0;
	sprintf(ramblock_disk->disk_name, "ramblock");
	ramblock_disk->fops = &ramblock_fops;
	set_capacity(ramblock_disk, RAMBLOCK_SIZE/512);			/* 对块设备操作是以扇区为单位，内核认为每个扇区都是512byte */

	/* 3. 硬件相关操作：分配内存 */
	ramblock_buff = kzalloc(RAMBLOCK_SIZE, GFP_KERNEL);

	/* 4. 注册   */
	add_disk(ramblock_disk);

	return 0;
}

static void __exit ramblock_exit(void)
{
	unregister_blkdev(major, "ramblock");
	del_gendisk(ramblock_disk);
	put_disk(ramblock_disk);
	blk_cleanup_queue(ramblock_queue);
	kfree(ramblock_buff);
}

module_init(ramblock_init);
module_exit(ramblock_exit);
MODULE_LICENSE("GPL");


