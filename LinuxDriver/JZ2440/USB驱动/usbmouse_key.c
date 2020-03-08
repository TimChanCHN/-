#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/usb/input.h>
#include <linux/hid.h>
#include <linux/input.h>

static struct input_dev* usbkey_dev;
static int len;
static char* buffer;
static dma_addr_t usb_buffer_phys;
static struct urb* usb_urb;

static struct usb_device_id usbmouse_key_id_table [] = {
	{ USB_INTERFACE_INFO(USB_INTERFACE_CLASS_HID, USB_INTERFACE_SUBCLASS_BOOT,
		USB_INTERFACE_PROTOCOL_MOUSE) },
	{ }	/* Terminating entry */
};

static void usbmouse_key_irq_handler(struct urb* urb)
{
	static unsigned char preval;
#if 0
	int i;
	static int cnt = 0;
	printk("data cnt %d:", ++cnt);

	for( i = 0; i < len; i++ )
	{
		printk("%02x ", buffer[i]);
	}
	printk("\n");
#endif
	if( (preval & (1<<0)) != (buffer[0] & (1<<0)) )
	{
		// 左键发生变化
		input_event(usbkey_dev, EV_KEY, KEY_L, (buffer[0] & (1<<0))?1:0);
		input_sync(usbkey_dev);
	}
	if( (preval & (1<<1)) != (buffer[0] & (1<<1)) )
	{
		// 右键发生变化
		input_event(usbkey_dev, EV_KEY, KEY_S, (buffer[0] & (1<<1))?1:0);
		input_sync(usbkey_dev);
	}
	if( (preval & (1<<2)) != (buffer[0] & (1<<2)) )
	{
		// 中键发生变化
		input_event(usbkey_dev, EV_KEY, KEY_ENTER, (buffer[0] & (1<<2))?1:0);
		input_sync(usbkey_dev);
	}

	preval = buffer[0];
	/* 重新提交urb */
	/* 类似一个刷新buffer的作用 */
	usb_submit_urb(usb_urb, GFP_KERNEL);
}

static int usbmouse_key_probe(struct usb_interface *intf, const struct usb_device_id *id)
{
	
	struct usb_device *dev = interface_to_usbdev(intf);	
	struct usb_host_interface *interface;
	struct usb_endpoint_descriptor *endpoint;
	int pipe;

	interface = intf->cur_altsetting;
	endpoint = &interface->endpoint[0].desc;
	/* 1. 分配input_dev */
	usbkey_dev = input_allocate_device();

	/* 2. 设置 */
	/* 2.1 设置哪类事件 */
	set_bit(EV_KEY, usbkey_dev->evbit);
	set_bit(EV_REL, usbkey_dev->evbit);
	
	/* 2.2 设置哪些事件 */
	set_bit(KEY_L, usbkey_dev->keybit);
	set_bit(KEY_S, usbkey_dev->keybit);
	set_bit(KEY_ENTER, usbkey_dev->keybit);

	/* 3. 注册 */
	input_register_device(usbkey_dev);

	/* 4. 硬件相关操作--关键是构造出urb */
	/* 数据传输3要素：源、目的、长度 */
	/* 源：获得USB的某个端点 */
	pipe = usb_rcvintpipe(dev, endpoint->bEndpointAddress);

	/* 长度 */
	len = endpoint->wMaxPacketSize;

	/* 目的地址 */
	buffer = usb_alloc_coherent(dev, len, GFP_ATOMIC, &usb_buffer_phys);

	/* 使用 */
	/* 分配usb request block */
	usb_urb = usb_alloc_urb(0, GFP_KERNEL);

	/* 利用3要素设置urb */
	usb_fill_int_urb(usb_urb, dev, pipe, buffer, len,
			 usbmouse_key_irq_handler, NULL, endpoint->bInterval);
	usb_urb->transfer_dma = usb_buffer_phys;
	usb_urb->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;

	/* 使用urb */
	usb_submit_urb(usb_urb, GFP_KERNEL);
	
	printk("usb mouse is connect!\n");
	
	return 0;
}

static void usbmouse_key_disconnect(struct usb_interface *intf)
{
	struct usb_device *dev = interface_to_usbdev(intf);	

	usb_kill_urb(usb_urb);
	usb_free_urb(usb_urb);
	usb_free_coherent(dev, len, buffer, usb_buffer_phys);
	input_unregister_device(usbkey_dev);
	input_free_device(usbkey_dev);
	
	printk("usb mouse is disconnect!\n");
}

static struct usb_driver usbmouse_key_driver = {
	.name		= "usbmouse_key",
	.probe		= usbmouse_key_probe,
	.disconnect	= usbmouse_key_disconnect,
	.id_table	= usbmouse_key_id_table,
};

static int __init usbmouse_key_init(void)
{
	usb_register(&usbmouse_key_driver);
	
	return 0;
}

static void __exit usbmouse_key_exit(void)
{	
	usb_deregister(&usbmouse_key_driver);
}

module_init(usbmouse_key_init);
module_exit(usbmouse_key_exit);
MODULE_LICENSE("GPL");



