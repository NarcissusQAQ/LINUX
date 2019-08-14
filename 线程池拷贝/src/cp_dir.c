/*
	用线程的方式实现一个目录的拷贝（只考虑目录和普通文件的情况）
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <pthread.h>
#include <stdlib.h>
#include "thread_pool.h"
/*
	cp_file :拷贝普通文件
*/

//创建一个线程池
struct thread_pool pool;

typedef struct data {
	char file1[257];
	char file2[257];
}Data;


void* cp_file(void*arg)
{
	char* file1=((Data*)arg)->file1;
	printf("%s\n",file1);
	char* file2=((Data*)arg)->file2;
	printf("%s\n",file2);
	
	int fd1 = open(file1,O_RDONLY);//对这个文件进行读
	if(fd1 == -1)
	{
		perror("open fd1 error:");
		return  NULL;
	}
	
	int fd2 = open(file2,O_WRONLY|O_CREAT,0666);//对这个文件进行写
	if(fd2 == -1)
	{
		perror("open fd2 error:");
		return NULL;
	}
	
	while(1)
	{
		char buf[32];
		int r = read(fd1,buf,32);
		if(r == 0)
		{
			break;
		}
		
		write(fd2,buf,r);
	}
	
	close(fd1);
	close(fd2);
	free(arg);
}
/*
		拷贝目录
	 把src这个目录拷贝到dest这个路径下
*/		
void cp_dir(char*src,char*dest)
{
	//在dest这个目录下创建一个src这个文件夹
	char*p = src+strlen(src)-1;
	while(*p != '/')
	{
		p--;
	}
	char buf[128] = {0};
	sprintf(buf,"%s%s",dest,p);
	
	mkdir(buf,0777);//目录一定要有执行权限
	
	//打开src这个目录
	DIR*dir = opendir(src);
	struct dirent*dirent = NULL;
	while(dirent = readdir(dir))//读取目录
	{
		char file1[257] = {0};
		char file2[257] = {0};
		sprintf(file1,"%s/%s",src,dirent->d_name);
		sprintf(file2,"%s/%s",buf,dirent->d_name);
		
		//判断文件类型
		if(dirent->d_type == DT_REG)//普通文件：拷贝
		{
			Data* document=(Data*)malloc(sizeof(Data));
			strcpy(document->file1,file1);
			strcpy(document->file2,file2);
			
			add_task(&pool,cp_file,document);
			
			
		}
		else if(dirent->d_type == DT_DIR)//子目录
		{
			if(!strcmp(dirent->d_name,".") || !strcmp(dirent->d_name,".."))
			{
				continue;
			}
			
			cp_dir(file1,buf);
		}		
	}
	
	closedir(dir);
}

int main(int argc,char**argv)
{
		

	
	//线程池的初始化
	init_pool(&pool,4);
	
	//投放任务

	cp_dir(argv[1],argv[2]);
	
		//线程池的销毁
	destroy_pool(&pool);
}