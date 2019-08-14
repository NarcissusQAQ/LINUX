#include "lcd.h"
#include "bmp_display.h"
#include "input.h"
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#define background "./bmp/background.bmp"
#define key_off "./bmp/key_off.bmp"
#define key_on "./bmp/key_on.bmp"
char buf[32];

int main()
{
	Init_lcd();
	bmp_display(background);
	bmp_display_piano(key_off,key_on,-1); 	
	int flag[257]={-1};
	int i=1;
	while(1)
	{
		flag[i]=input();
		printf("******************************************************%d\n",flag[i]);
		bmp_display_piano(key_off,key_on,flag[i]);
	 	if((flag[i]!=flag[i-1])&&flag[i]!=-1)
		{
			system("killall -9 madplay");
			printf("*****************************%d\n",flag[i]);
			sprintf(buf,"madplay -Q -a -20 ./music/d%d.mp3 & ",flag[i]+1);
			system(buf);
		}
		i++;
		if(flag[i]==-1)
		{
			i=1;
		} 
	}
	//step3:关闭文件，解映射
	Uinit_lcd();
	return -1;
}
