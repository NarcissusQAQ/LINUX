#include "lcd.h"
int *plcd = NULL;
int lcd_fd;
/*
	给LCD屏一一去画点
*/
void draw_point(int x,int y,int color)
{
	*(plcd+x*800+y) = color;
}
/*
	初始化LCD
*/
int Init_lcd()
{
	//step1：打开LCD设备文件
	lcd_fd = open("/dev/fb0",O_RDWR);
	if(lcd_fd == -1)
	{
		perror("open lcd error:");
		return -1;
	}
	
				
	//step2:映射
	plcd = (int*)mmap(NULL,800*480*4,PROT_WRITE|PROT_READ ,MAP_SHARED,lcd_fd,0);
	if(plcd == MAP_FAILED)
	{
		perror("mmap error:");
		close(lcd_fd);
		return -1;
	}
	
}

/*
	step5:解映射，关闭文件
*/
void Uinit_lcd()
{
	munmap(plcd,480*800*4);
	close(lcd_fd);
}

void clear_lcd()
{
	int x,y;//x 横坐标  y   纵坐标
	for(y = 0;y < 480;y++)
	{
		for(x = 0;x<800;x++)
		{
			//画点
			draw_point(y,x,0);
		}
	}
	
}