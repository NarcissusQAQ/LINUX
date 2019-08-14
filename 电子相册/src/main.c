#include "lcd.h"
#include "bmp_display.h"
#include "linkedlist.h"
#include "dir.h"
#include "input.h"
#include "jpg_display.h"

Node* p =NULL;

int main(int argc,char**argv)
{
	//step1:初始化LCD
	Init_lcd();

	//step2:创建一条链表，再读取目录
	List* list = Creat_linkedlist();

	//读取目录
	list = search_dir(list,argv[1]);

	p = list->first;
	
	
	while(1)
	{
		puts(p->data);
		if(strcmp(p->data+strlen(p->data)-4,".bmp") == 0)
		{
			bmp_display(p->data);
		}
		else
		{  
			jpg_display(p->data);
		}
		
		input();
	}
	//step3:关闭文件，解映射
	Uinit_lcd();
}
