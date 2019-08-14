#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h> /*perror, errno ,...*/
#include <stdlib.h>/*abs的头文件*/
#include <stdio.h> /*printf*/
#include <sys/mman.h>
#include <linux/input.h>
#include <stdlib.h>
#include <time.h> /*time */
#include <string.h>
#include <linux/fb.h>
#include <pthread.h>
#include <dirent.h> 
#include "lcd.h"
extern int* p;
void draw_point(int x,int y,int color)
{
	*(p+(y*800)+x) = color;
}

int display_bmp(char*bmp_name)
{
	//打开bmp图片文件
	int bmp_fd = open(bmp_name,O_RDONLY);
	if(bmp_fd == -1)
	{
		perror("open error:");
		return -1;
	}
	
	char buf[2];
	read(bmp_fd,buf,2);
	if(buf[0] != 0x42 || buf[1] != 0x4d)
	{
		printf("NO\n");
		close(bmp_fd);
		return -1;
	}
	
	//获取宽和高
	int w,h;
	lseek(bmp_fd,0x12,SEEK_SET);
	read(bmp_fd,&w,4);
	read(bmp_fd,&h,4);
	
	//获取bmp的像素数据
	char bmp_buf[w*h*3];
	lseek(bmp_fd,54,SEEK_SET);
	read(bmp_fd,bmp_buf,w*h*3);
	
	//一一画点
	int i = 0;
	int x,y;//x :横坐标  y :纵坐标
	for(y = 0;y < h;y++)
	{
		for(x = 0;x < w;x++)
		{
			unsigned char a,r,g,b;
			b = bmp_buf[i++];
			g = bmp_buf[i++];
			r = bmp_buf[i++];
			a = 0;
			
			int color = b | g<<8|r <<16 | a << 24;
			
			draw_point(x,h-1-y,color);
		}
	}
	
	//关闭bmp文件
	close(bmp_fd);
}

int display_bmp2(char*bmp_name,int x0,int y0)
{
	//打开bmp图片文件
	int bmp_fd = open(bmp_name,O_RDONLY);
	if(bmp_fd == -1)
	{
		perror("open error:");
		return -1;
	}
	
	char buf[2];
	read(bmp_fd,buf,2);
	if(buf[0] != 0x42 || buf[1] != 0x4d)
	{
		printf("NO\n");
		close(bmp_fd);
		return -1;
	}
	
	//获取宽和高
	int w,h;
	lseek(bmp_fd,0x12,SEEK_SET);
	read(bmp_fd,&w,4);
	read(bmp_fd,&h,4);
	
	//获取bmp的像素数据
	char bmp_buf[w*h*3];
	lseek(bmp_fd,54,SEEK_SET);
	read(bmp_fd,bmp_buf,w*h*3);
	
	//一一画点
	int i = 0;
	int x,y;//x :横坐标  y :纵坐标
	for(y = 0;y < h;y++)
	{
		for(x = 0;x < w;x++)
		{
			unsigned char a,r,g,b;
			b = bmp_buf[i++];
			g = bmp_buf[i++];
			r = bmp_buf[i++];
			a = 0;
			
			int color = b | g<<8|r <<16 | a << 24;
			
			draw_point(x+x0,y0+h-1-y,color);
		}
	}
	
	//关闭bmp文件
	close(bmp_fd);
}


int lcd_init()
{
	//打开LCD设备
	int fd = open("/dev/fb0",O_RDWR);
	if(fd == -1)
	{
		perror("open lcd error: ");
		return -1;
	}
	
	
	//获取信息
	struct fb_var_screeninfo fbinfo;
	ioctl(fd,FBIOGET_VSCREENINFO,&fbinfo);
	
	printf("xres = %d yres = %d pixel = %d\n",fbinfo.xres,fbinfo.yres,fbinfo.bits_per_pixel);
	printf("read = %d %d\n",fbinfo.red.offset,fbinfo.red.length);
	printf("green = %d %d\n",fbinfo.green.offset,fbinfo.green.length);
	printf("blue = %d %d\n",fbinfo.blue.offset,fbinfo.blue.length);
	printf("transp = %d %d\n",fbinfo.transp.offset,fbinfo.transp.length);
	
	//mmap映射整个屏幕
	p = mmap(NULL,800*480*4,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    if(p == MAP_FAILED)
	{
		perror("mmap error:");
		return -1;
	}
	
	return fd;
}
/*
	lcd_uinit:释放LCD资源：关闭文件。解映射
*/

void lcd_uinit(int fd)
{
	close(fd);
	munmap(p,480*800*4);
}
/*
	clear_lcd :清屏
*/
void clear_lcd()
{
	int x,y;//x 横坐标  y   纵坐标
	for(y = 0;y < 480;y++)
	{
		for(x = 0;x<800;x++)
		{
			//画点
			draw_point(x,y,0x00f0f0f0);
		}
	}
	
}