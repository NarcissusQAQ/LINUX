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

//bmp图像移动函数，a[1]==x轴 b[1]==y轴 将按下的点作为中心点，
//上下左右把图片所占映射空间算出来，超出屏幕不算，少的补黑色。
int bmp_move_display(char *s,int x,int y)
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
	
	int bmp_move[h][w];
	

	//将color收集到二维数组中
	int i,j;
	for(i=h-1;i>=0;i--)
	{
		for(j=0;j<=w-1;j++)
		{
			unsigned char r,g,b,a;
			b = *(p++);
			g = *(p++);
			r = *(p++);
			a = (depth == 24 ? 0 : *(p++)); 
			bmp_move[i][j]= a << 24 | r << 16 | g << 8 |b;
		}
	}
	
	int n,m;
	//240=480/2 y轴一半
	//400=800/2 x轴一半
	int yy=y-240,xx=x-400;  //中心偏移量
	
	//按找中心点画点
	for(n=(yy>0?yy:0);n<h;n++)
	{
		for(m=(xx>0?xx:0);m<w;m++)
		{
			draw_point(n,m,bmp_move[n-yy][m-xx]);
		}
	}
	
 	//移动后空白部分补黑色
	for(n=(yy>0?0:(480+yy));n<(yy>0?yy:h);n++)
	{
		for(m=(xx>0?0:(800+xx));m<(xx>0?xx:w);m++)
		{
			draw_point(n,m,0x0ffffff); //补黑色
		}
	} 
	
	close(bmp_fd);
}
