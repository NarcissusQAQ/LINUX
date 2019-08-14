#ifndef _LCD_H
#define _LCD_H 
void draw_point(int x,int y,int color);


int display_bmp(char*bmp_name);

int display_bmp2(char *bmp_name, int x0, int y0);


/*
	draw_point:画点
*/


/*
	lcd_init:申请LCD资源（打开LCD文件，获取信息之后映射）
*/
int lcd_init();


/*
	lcd_uinit:释放LCD资源：关闭文件。解映射
*/

void lcd_uinit(int fd);
void clear_lcd();
#endif