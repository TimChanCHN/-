#include "lcd.h"

extern int fd_dev, fd_asc64, fd_hzk64;
extern int order;
extern char* fb_start, *asc64_start , *hzk64_start ;

/* 屏幕大小：800*1280 */
int main(int argc, char* argv[])
{
	int len_dev, len_asc, len_hzk;
	order = ORDER90N;
	
	len_dev = GetFbStart("/dev/fb0");
	len_asc = GetASC64("/demo/ASC64");
	len_hzk = GetHZK64("/demo/HZK64");
	
	lcd_clear(WHITE);
	lcd_show_rect(100, 100, 200, 300, YELLOW);
	LcdShowASC64(100, 100, 'A', YELLOW, RED);
	LcdShowHZK64(100, 200, "��", YELLOW, RED);
	jpeg_decompression(argv[1]);
	
	FreeFb(fb_start, fd_dev, len_dev);
	FreeFb(asc64_start, fd_asc64, len_asc);
	FreeFb(hzk64_start, fd_hzk64, len_hzk);
	
	return 0;
}

