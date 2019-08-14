#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sqlite3.h>

// -lsqlite3
int sqlite_create(char *filename)
{
	sqlite3 * ppDb = NULL;
	sqlite3_stmt *ppStmt = NULL;
	
	printf("进入数据库创建\n");
	//打开或创建一个sqliete3数据库，返回一个sqlite3的数据库连接对象	
	int re = sqlite3_open(filename,&ppDb);
	if(re != 0)
	{
		perror("sqlite3_open error:");
		return -1;
	}
	printf("输入要创建表的名字\n");
	char graphname[32];
	fgets(graphname,32,stdin);
	
	printf("输入创建表指令\n");
	char creat[128];
	fgets(creat,128,stdin);
	
	char cmd[160];
	sprintf(cmd,"create table %s%s;",graphname,creat);
	
	re = sqlite3_prepare_v2(ppDb,cmd,-1,&ppStmt,NULL);
	if(re != 0)
	{
		perror("sqlite3_prepare_v2 error:");
		return -1;
	}

	re = sqlite3_step(ppStmt);
	if(re != SQLITE_DONE)
	{
		perror("sqlite3_step error:");
		return -1;
	}		

	re = sqlite3_finalize(ppStmt);
	if(re != 0)
	{
		perror("sqlite3_finalize error:");
		return -1;
	}
	//关闭数据库连接对象
	sqlite3_close(ppDb);	
}

int sqlite_add_data(char* filename)
{
	sqlite3 * ppDb = NULL;
	sqlite3_stmt *ppStmt = NULL;
	
	int num;
	int i=0;
	printf("输入要增加的对象个数\n");
	scanf("%d",&num);
	getchar();
	char graphname[32];
	printf("输入要往哪个表中加对象\n");
	fgets(graphname,32,stdin);
	
	//打开或创建一个sqliete3数据库，返回一个sqlite3的数据库连接对象	
	int re = sqlite3_open(filename,&ppDb);
	if(re != 0)
	{
		perror("sqlite3_open error:");
		return -1;
	}
	
	while(num--)
	{
		i++;
		printf("输入增加对象数据\n");
		char insert[64];
		fgets(insert,64,stdin);
		
		char cmd[128];
		sprintf(cmd,"insert into %s values(%s);",graphname,insert);
	
		re = sqlite3_prepare_v2(ppDb,cmd,-1,&ppStmt,NULL);
		if(re != 0)
		{
			perror("sqlite3_prepare_v2 error:");
			return -1;
		}

		re = sqlite3_step(ppStmt);
		if(re != SQLITE_DONE)
		{
			perror("sqlite3_step error:");
			return -1;
		}		

		re = sqlite3_finalize(ppStmt);
		if(re != 0)
		{
			perror("sqlite3_finalize error:");
			return -1;
		}
		printf("已录入%d个,还差%d个",i,num);
	}
	//关闭数据库连接对象
	sqlite3_close(ppDb);
	printf("录入数据成功\n");
}

int sqlite_add_graph(char *filename)
{
	sqlite3 * ppDb = NULL;
	sqlite3_stmt *ppStmt = NULL;
	
	//打开或创建一个sqliete3数据库，返回一个sqlite3的数据库连接对象	
	int re = sqlite3_open(filename,&ppDb);
	if(re != 0)
	{
		perror("sqlite3_open error:");
		return -1;
	}
	printf("输入新建表名\n");
	char graphname[32];
	fgets(graphname,32,stdin);
	
	printf("输入创建表指令\n");
	char creat[128];
	fgets(creat,128,stdin);
	
	char cmd[160];
	sprintf(cmd,"create table %s%s;",graphname,creat);
	
	re = sqlite3_prepare_v2(ppDb,cmd,-1,&ppStmt,NULL);
	if(re != 0)
	{
		perror("sqlite3_prepare_v2 error:");
		return -1;
	}

	re = sqlite3_step(ppStmt);
	if(re != SQLITE_DONE)
	{
		perror("sqlite3_step error:");
		return -1;
	}		

	re = sqlite3_finalize(ppStmt);
	if(re != 0)
	{
		perror("sqlite3_finalize error:");
		return -1;
	}
	//关闭数据库连接对象
	sqlite3_close(ppDb);
}

int main(int argc,char *argv[])
{
	int fd = open(argv[1],O_RDONLY );
	if(fd<0)
	{
		close(fd);
		sqlite_create(argv[1]);
	}
	else
	{
		close(fd);
		printf("********已进入数据库管理系统***\n");
		printf("********按1进入数据库录入******\n");
		printf("********按2创建新的表**********\n");
		printf("********按3退出数据库管理系统**\n");
		int flag;
		scanf("%d",&flag);
		getchar();
		
		if(flag==1)
		{
		   sqlite_add_data(argv[1]);
		}
		else if (flag==2)
		{
			sqlite_add_graph(argv[1]);
		}
		else if (flag==3)
		{
			printf("已退出录入系统\n");
		}
	}
}