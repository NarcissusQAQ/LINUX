#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>
#include <unistd.h>
#include "lcd.h"


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
		int i=0;
		
		while(1)
		{
			read(fd,&ev,sizeof(struct input_event));
			
			//x轴 
		 	if(ev.type == 0x03 && ev.code == 0x00  )
			{
				if(i==0)
				{
					a[0]=ev.value;
				}
				else 
				{
					a[1]=ev.value;
				}
					i++;				
			}
			
			//y轴
			if(ev.type == 0x03 && ev.code == 0x01  ) 
			{
					b=ev.value;
			}
			
			if(b>259&&a[0]>16&&a[0]<=783)
			{
					return  (a[0]-16)/65;
			}
			
			if(ev.type == 1 && ev.code == 0x14a && ev.value == 0)
			{
					return -1;
			}
		}
	//（3）关闭文件
		close(fd);
}