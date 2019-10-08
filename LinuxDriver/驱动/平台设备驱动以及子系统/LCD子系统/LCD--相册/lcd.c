#include "lcd.h"

struct fb_var_screeninfo var;
struct fb_fix_screeninfo fix;
char* fb_start = NULL, *asc64_start = NULL, *hzk64_start = NULL;
int fd_dev, fd_asc64, fd_hzk64;
int order = 0;							//屏幕显示方式

int GetFbStart(char* name)
{
	int length = 0;
	
	fd_dev = open(name, O_RDWR);
	if( fd_dev < 0 )
	{
		perror("open fail");
		return -1;
	}

	ioctl(fd_dev, FBIOGET_VSCREENINFO, &var);
	printf("---(x,y):(%d,%d)  bpp:%d---\n", var.xres, var.yres, var.bits_per_pixel);

	ioctl(fd_dev, FBIOGET_FSCREENINFO, &fix);
	printf("---fb_phyaddr:0x%x  fb_length:%d  line_length:%d---\n", (unsigned int)fix.smem_start, fix.smem_len, fix.line_length);

	/* 获取显存大小 */
	length = var.xres * var.yres * var.bits_per_pixel / 8;
	fb_start = mmap(NULL, length, PROT_READ| PROT_WRITE, MAP_SHARED, fd_dev, 0);
	return length;
}

void FreeFb(char* fb, int fd, int length)
{
	munmap(fb, length);
	close(fd);
}

/* 
 作用：点亮某一个点【按原有物理坐标】
 参数：
 offsetx	:	横向偏移
 offsety	:	纵向偏移
 color		:	颜色
 */
void lcd_show_point(int offsetx, int offsety, unsigned long color)
{
	char* pen = fb_start + (offsety * var.xres + offsetx)*var.bits_per_pixel / 8;
	unsigned long* pen_32 = (unsigned long*)pen;
	*pen_32 = color;
}

/* 
 * 作用：点亮某一个点【顺时针90°】
 */
void lcd_show_point_clockwise(int offsetx, int offsety, unsigned long color)
{
	lcd_show_point( (var.xres -1 - offsety), offsetx, color);
}

/* 
 * 作用：点亮某一个点【逆时针90°】
 */
void lcd_show_point_counterclockwise(int offsetx, int offsety, unsigned long color)
{
	lcd_show_point( offsety, (var.yres - 1 - offsetx), color);
}

/* 
 * 作用：点亮某一个点【旋转180°】
 */
void lcd_show_point_rotate180(int offsetx, int offsety, unsigned long color)
{
	lcd_show_point( (var.xres - 1 - offsetx), (var.yres - 1 - offsety), color);
}

/* 
 * 作用：选择当前屏幕显示方式
 */
void lcd_show(int x, int y, unsigned long color)
{
	switch(order)
	{
		case ORDER:
			lcd_show_point( x, y, color);
			//printf("enter 0\n");
			break;
			
		case ORDER90P:
			lcd_show_point_clockwise( x, y, color);
			//printf("enter clockwise\n");
			break;
			
		case ORDER90N:
			lcd_show_point_counterclockwise( x, y, color);
			//printf("enter counterclockwise\n");
			break;
			
		case ORDER180:
			lcd_show_point_rotate180( x, y, color);
			//printf("enter 180\n");
			break;
			
		default:
			break;
	}
}

/* 
 作用：点亮一个矩形
 参数：
 offsetx	:	起始地址的x坐标
 offsety	:	起始地址的y坐标
 width		:	矩形宽
 height		:	矩形高
 */
void lcd_show_rect(int offsetx, int offsety, int width, int height, unsigned long color)
{
	int i = 0, j = 0;
	int xend = offsetx + width;
	int yend = offsety + height;
/*
	if( xend >= var.xres )
		xend = var.xres;
		
	if( yend >= var.yres )
		yend = var.yres;
*/

	for( i = offsety; i < yend; i++ )
		for( j = offsetx; j < xend; j++ )
		{
			lcd_show(j, i, color);
		}
}
/* 
 作用：清屏
 参数：
 color		:	清屏颜色
 BUG		:	清屏宽度不能大于等于1280，否则会报错
 */
void lcd_clear(unsigned long color)
{
	switch(order)
	{
		case ORDER:
		case ORDER180:
			lcd_show_rect(0, 0, var.xres - 1, var.yres-1, color);
			break;
			
		case ORDER90P:
		case ORDER90N:
			lcd_show_rect(0, 0, var.yres - 1, var.xres-1, color);
			break;
			
		default:
			break;
	}

}

/*
 *		函数功能	：  获得ASC64库的内存映射
 * 		参数：
 *			name	:	ASC64路径名称
 */
int GetASC64(char* name)
{
	int length = 0;
	
	fd_asc64 = open(name, O_RDONLY);
	if( fd_asc64 < 0 )
	{
		perror("open asc64 fail");
		return -1;
	}

	length = lseek(fd_asc64, 0, SEEK_END);
	lseek(fd_asc64, 0, SEEK_SET);
	asc64_start = mmap(NULL, length, PROT_READ, MAP_SHARED, fd_asc64, 0);
	return length;
}

/*
 *		函数功能	：  显示ASC64字符(像素个数：32*64，半字)
 *				   字模的每一个位代表着一个像素点，
 *					在ASC64表中，其ASCII大小表示它在该字库中的地址
 *		参数	：
 *			offsetx		：	x坐标
 *			offsety		：	y坐标
 *			c			:	待显示ASC64
 *			backcolor	:	背景色
 *			color		:	字体颜色
 *		所占字节数：			
 *					32*64/8 = 256
 *		备注：
 *			在ASCII中，从0开始计数
 */
void LcdShowASC64(int offsetx, int offsety, char c, unsigned long backcolor, unsigned long color)
{
	char *cp = asc64_start + c * 256;
	int raw = 0, col = 0;						//行，列
	int i = 0, j = 0;							// i : 该字模的256字节的数据；j : 每个字节数据的每一位

	for( i = 0; i < 256; i++)
	{
		for( j = 0; j < 8; j++ )
		{
			if(cp[i]&(0x80>>j))
			{
				lcd_show(offsetx + raw*8 + j, offsety+col, color);
				continue;
			}
			lcd_show(offsetx + raw*8 + j, offsety+col, backcolor);
		}
		raw++;
		if( raw >= 4 )
		{
			raw = 0;
			col++;
		}

	}
}

/*
 *		函数功能	：  获得汉字库库的内存映射
 * 		参数：
 *			name	:	 HZK64路径名称
 */
int GetHZK64(char* name)
{
	int  length = 0;
	fd_hzk64 = open(name,  O_RDONLY);
	if(fd_hzk64  <  0)
	{
		perror("open failed!");
		return -1;
	}

	length = lseek(fd_hzk64,  0,  SEEK_END);			// fstat();
	lseek(fd_hzk64,  0,  SEEK_SET);
	hzk64_start = mmap(NULL,  length,  PROT_READ,  MAP_SHARED,  fd_hzk64,  0);	
	return length;
}

/*
 *		函数功能	：  显示汉字字符(像素个数：64*64，全字)
 *				   字模的每一个位代表着一个像素点，都需要点亮
 *		参数	：
 *			offsetx		：	x坐标
 *			offsety		：	y坐标
 *			hz			:	待显示汉字
 *			backcolor	:	背景色
 *			color		:	字体颜色
 *		所占字节数：			
 *					64*64/8 = 512
 *		备注：
 *			汉字库中，区码和位码都是从0xa1开始的。汉字库中每一个区都有94个位(都最多存放94个汉字)
 *			对于RK3399，要显示中文，只能在SourceInsight3中编辑的中文才有效
 */
void LcdShowHZK64(int offsetx, int offsety, char* hz, unsigned long backcolor, unsigned long color)
{
	char *hzp = hzk64_start + ((hz[0] - 0xa1) * 94 + (hz[1] - 0xa1))*512;
	int raw = 0, col = 0;						//行，列
	int i = 0, j = 0;							// i : 该字模的256字节的数据；j : 每个字节数据的每一位

	for( i = 0; i < 512; i++)
	{
		for( j = 0; j < 8; j++ )
		{
			if(hzp[i]&(0x80>>j))
			{
				lcd_show(offsetx + raw*8 + j, offsety+col, color);
				continue;
			}
			lcd_show(offsetx + raw*8 + j, offsety+col, backcolor);
		}
		raw++;
		if( raw >= 8 )
		{
			raw = 0;
			col++;
		}

	}
}

/*
 *		函数功能	：  显示包含中英字符的汉字
 *		参数	：
 *			offsetx		：	x坐标
 *			offsety		：	y坐标
 *			buff		:	待显示汉字
 *			backcolor	:	背景色
 *			color		:	字体颜色
 *		备注：
 *			在包含了中英文的字符串中，通过大小来判断是ASC64还是HZK64，小于128的为ASC库
 *			由于中文是字符串，因此当遇到中文字符时，需要遍历两次，每一次遍历半字（32个像素点）
 */

void LcdShowString(int offsetx, int offsety, char* buff, unsigned long backcolor, unsigned long color)
{
	int index = 0;
	while( buff[index] != '\0' )
	{
		if( buff[index] < 128 )
			LcdShowASC64(offsetx + index * 32, offsety, buff[index], backcolor, color);	
		else
		{
			LcdShowHZK64(offsetx + index * 32, offsety, buff+index, backcolor, color);	
			index++;
		}
		index++;
	}
	
}

/*
 *		函数功能	：  解压图片
 *		参数	：
 *			picname		:		指定图片路径
 * 
 */
void jpeg_decompression(char* picname)
{
	/* 1.分配并初始化解压对象 */
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);

	/* 2.指定待解压的图片 */
	FILE * infile;
	if ((infile = fopen(picname, "rb")) == NULL) {
		fprintf(stderr, "can't open %s\n", picname);
		exit(1);
	}
	jpeg_stdio_src(&cinfo, infile);

	/* 3.调用函数获得图片信息 */
	jpeg_read_header(&cinfo, TRUE);
/*
	printf("----image info-----\n");
	printf("----image width:%d----\n",cinfo.image_width);
	printf("----image length:%d----\n",cinfo.image_height);
	printf("----image bpp:%d----\n",cinfo.num_components);
*/
	
	/* 4.设置解压参数 */
/*
	printf("pls enter scale<x/y>:");
	fflush(stdout);
	scanf("%d/%d",&cinfo.scale_num, &cinfo.scale_denom);
*/
	cinfo.scale_num = 1;
	cinfo.scale_denom = 1;

	/* 5.开始解压，得到RGB数据 */
	jpeg_start_decompress(&cinfo);
/*
	printf("----output info-----\n");
	printf("----output width:%d----\n",cinfo.output_width);
	printf("----output length:%d----\n",cinfo.output_height);
	printf("----output bpp:%d----\n",cinfo.out_color_components);
*/

	/* 6.读取RGB数据，一次只能读取一行数据 */
	JSAMPARRAY  buffer;		/* 输出行缓存 */
  	int  row_stride;			/* 一行图片解压得到的RGB数据的长度 */
	row_stride = cinfo.output_width * cinfo.output_components;
  	buffer = (*cinfo.mem->alloc_sarray) ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);
	while (cinfo.output_scanline < cinfo.output_height) 	//output_scanline每次调用自加1
	{
    		(void) jpeg_read_scanlines(&cinfo, buffer, 1);		//读取解压得到的一行RGB数据
    		lcd_show_picture(0,  cinfo.output_scanline-1,  cinfo.output_width,  buffer[0]);
  	}
  	
	/* 7.完成解压 */
	jpeg_finish_decompress(&cinfo);
	
	/* 8.释放解压对象结构体变量 */
	jpeg_destroy_decompress(&cinfo);
	
}


/*
 *	函数功能：显示一行图像
 *	形参列表:
 *		x,y : 一行图像的起始点坐标
 *		width: 行的宽度
 *		buf: 这一行图像的RGB数据首地址 
 */
void   lcd_show_picture(int  x,  int  y,  int  width,  char*  buf)
{
	int  i = 0;
	int  xend  =  x + width;

	if((x >= 800) || (y >= 1280))
	{
		return;
	}
	if(xend  >= 800)
	{
		xend  = 800;
	}

	int   n = 0;
	unsigned  long  color = 0;
	for(i = x;  i  <  xend; i++, n++)
	{
		color  =  ((unsigned long)buf[3*n] << 16) | (buf[3*n+1] << 8) | buf[3*n+2];
		lcd_show(i,  y,  color);
	}
}




