#ifndef	__LCD_H__
#define	__LCD_H__

#include <stdio.h>
#include <linux/fb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "jpeglib.h"
#include <setjmp.h>
#include <stdlib.h>

#define BLACK	0x000000
#define WHITE	0xffffff
#define	RED		0xff0000
#define GREEN	0x00ff00
#define BLUE	0x0000ff
#define PURPLE	0x8B008B
#define YELLOW	0xffff00

#define	ORDER		0			//原物理坐标
#define	ORDER90P	1			//顺时针旋转90°
#define ORDER90N	2			//逆时针旋转90°
#define	ORDER180	3			//旋转180
//ORDER90P,ORDER180不能用

void lcd_show_point(int offsetx, int offsety, unsigned long color);
void lcd_show_point_clockwise(int offsetx, int offsety, unsigned long color);
void lcd_show_point_counterclockwise(int offsetx, int offsety, unsigned long color);
void lcd_show_point_rotate180(int offsetx, int offsety, unsigned long color);

void lcd_show_rect(int offsetx, int offsety, int width, int height, unsigned long color);
void lcd_clear(unsigned long color);
int GetFbStart(char* name);
void FreeFb(char* fb, int fd, int length);
int GetASC64(char* name);
void LcdShowASC64(int offsetx, int offsety, char c, unsigned long backcolor, unsigned long color);
int GetHZK64(char* name);
void LcdShowHZK64(int offsetx, int offsety, char* hz, unsigned long backcolor, unsigned long color);
void LcdShowString(int offsetx, int offsety, char* buff, unsigned long backcolor, unsigned long color);
void lcd_show_picture(int  x,  int  y,  int  width,  char*  buf);
void jpeg_decompression(char* picname);


#endif
