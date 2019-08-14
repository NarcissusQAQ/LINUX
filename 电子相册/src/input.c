#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>
#include <unistd.h>
#include "linkedlist.h"
#include "lcd.h"
extern Node* p ;

int input()
{
	//（1）打开文件 触摸屏的驱动文件：/dev/input/event0
		int fd = open("/dev/input/event0",O_RDONLY);
		if(fd == -1)
		{
			perror("open error:");
			return -1;
		}
		
	//（2）读取触摸屏文件
		struct input_event ev;
		
		int a[2]={0};  //x轴
		int b=0;		//y轴
		int c[2]={0};	//时间轴 
		int i=0;
		
		while(1)
		{
			read(fd,&ev,sizeof(struct input_event));
			
			//x轴 时间轴
		 	if(ev.type == 0x03 && ev.code == 0x00  )
			{
				if(i==0)
				{
					a[0]=ev.value;
					c[0]=(ev.time.tv_sec&0xffffffff); //ev.time.tv_sec 为long类型 或运算使得为32位，代表位秒
				}
				else 
				{
					a[1]=ev.value;
					c[1]=(ev.time.tv_sec&0xffffffff);
				}
					i++;				
			}
			
			//y轴
			if(ev.type == 0x03 && ev.code == 0x01  ) 
			{
					b=ev.value;
			}
			
			//滑动判断
			if(ev.type == 1 && ev.code == 0x14a && ev.value == 0)
			{
				if(a[1]>a[0]+100)			//右滑 为下一个
				{
					p= p->next;
					//return 1;
					return 1;
				}
				else if(a[0]>a[1]+100)  	//左滑为上一个
				{
					p= p->prev;
					//return 0;
					return 0;
				}
				else
				{
					return -1;
				}
			}
			
			//时间轴判断  当超过三秒的时候进入else if
			if((c[1]-c[0]>3)&&!(ev.type == 1 && ev.code == 0x14a && ev.value == 0))
			{
				printf("star move\n");
				//clear_lcd();
				//sleep(0.5);
				bmp_move_display(p->data,a[1],b); //bmp图像移动函数，a[1]==x轴 b[1]==y轴 将按下的点作为中心点，
													//上下左右把图片所占映射空间算出来，超出屏幕不算，少的补黑色。			
				a[0]=a[1];
			}
		}
	//（3）关闭文件
		close(fd);
}