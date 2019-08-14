#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "lcd.h"
/*
	显示一张bmp图片
*/
int bmp_display(char *s)
{
	//step1:打开这个bmp文件
	int bmp_fd = open(s,O_RDONLY);
	if(bmp_fd == -1)
	{
		perror("open bmp file error:");
		return -1;
	}
	
	//step2:判断是否是一个bmp文件
	char buf[2];
	read(bmp_fd,buf,2);
	if(buf[0] != 0x42 || buf[1] != 0x4d)
	{
		close(bmp_fd);
		printf("no\n");
		return -1;
	}
	printf("yes\n");
	
	//step3:获取位图宽和高
	int w,h;
	lseek(bmp_fd,0x12,SEEK_SET);
	read(bmp_fd,&w,4);
	read(bmp_fd,&h,4);
	
	printf("%d*%d\n",w,h);
	
	int size;
	//step4:像素数组的起始位置
	lseek(bmp_fd,10,SEEK_SET);
	read(bmp_fd,&size,4);
	printf("offset = %d\n",size);
	
	//step5:获取色深
	short depth;
	lseek(bmp_fd,0x1c,SEEK_SET);
	read(bmp_fd,&depth,2);
	printf("depth = %d\n",depth);
	
	//step6:读取像素数组的颜色数据
	char bmpbuf[w*h*depth/8];
	char *p = bmpbuf;
	lseek(bmp_fd,54,SEEK_SET);
	read(bmp_fd,bmpbuf,w*h*depth/8);
	
	//把像素数组的数据一一画点
	int x,y;
	for(x = 0;x < h;x++)
	{
		for(y = 0;y < w;y++)
		{
			unsigned char r,g,b,a;
			int color;
			b = *(p++);
			g = *(p++);
			r = *(p++);
			a = (depth == 24 ? 0 : *(p++)); 
			color = a << 24 | r << 16 | g << 8 |b;
			draw_point(h-1-x,y,color);
		}
	}
	
	close(bmp_fd);
}

int bmp_display_piano(char* s,char* ss,int n)
{
	//step1:打开这个bmp文件
	int bmp_fd_off = open(s,O_RDONLY);
	int bmp_fd_on = open(ss,O_RDONLY);

	//step3:获取位图宽和高
	int w,h;
	lseek(bmp_fd_off,0x12,SEEK_SET);
	read(bmp_fd_off,&w,4);
	read(bmp_fd_off,&h,4);
	
	
	//step5:获取色深
	short depth;
	lseek(bmp_fd_off,0x1c,SEEK_SET);
	read(bmp_fd_off,&depth,2);
	
	
	//step6:读取像素数组的颜色数据
	char bmpbuf_off[w*h*depth/8];
	char *p = bmpbuf_off;
	lseek(bmp_fd_off,54,SEEK_SET);
	read(bmp_fd_off,bmpbuf_off,w*h*depth/8);
	
	char bmpbuf_on[w*h*depth/8];
	char *q = bmpbuf_on;
	lseek(bmp_fd_on,54,SEEK_SET);
	read(bmp_fd_on,bmpbuf_on,w*h*depth/8);
	
	//把像素数组的数据一一画点
	int x,y,z;
	
	for(z=0;z<12;z++)
	{
		unsigned char r,g,b,a;
		int color;
		
		if(z==n)
		{
			for(x = 0;x < h;x++)
			{
				for(y = z*w;y < w*(z+1);y++)
				{
					b = *(q++);
					g = *(q++);
					r = *(q++);
					a = (depth == 24 ? 0 : *(q++)); 
					color = a << 24 | r << 16 | g << 8 |b;
					draw_point(479-x,y+16,color);
			}
			}	
		}
		else
		{
			for(x = 0;x < h;x++)
			{
				for(y = z*w;y < w*(z+1);y++)
				{
					b = *(p++);
					g = *(p++);
					r = *(p++);
					a = (depth == 24 ? 0 : *(p++)); 
					color = a << 24 | r << 16 | g << 8 |b;
					draw_point(479-x,y+16,color);
				}
			}
			p=bmpbuf_off;
		}
	}
	close(bmp_fd_off);
	close(bmp_fd_on);
}

/* int main(int argc ,char** argv)
{
	Init_lcd();
	int i=0;
	while(1)
	{
		bmp_display_piano(argv[1],argv[2],-1);
		bmp_display_piano(argv[1],argv[2],i);
		i++;
		i=i%12;
	}
} */








