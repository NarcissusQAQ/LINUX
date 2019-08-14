#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sqlite3.h>

// -lsqlite3

sqlite3 * ppDb = NULL;
//sqlite3_stmt *ppStmt = NULL;



int sqlite_create(char *filename);                       //创建数据库
int sqlite_add_data(char* filename);					 //往对应表中加数据
int sqlite_add_graph(char *filename);					 //创建新表
void sqlite_table(sqlite3* ppDb);						 //显示数据库中表名
void sqlite_schema(char* graphname,sqlite3* ppDb);		 //显示对应表名的创建语句
int sqlite3_user_login(char* filename);					 //用户登录
void sqlite3_user_registered(char* filename);			 //用户注册
void sqlite3_graph_display_tab(int len);                 //制表符对齐(12字节)
int sqlite3_graph_delete(char* filename);				 //删除对应表数据
int sqlie3_data_update(char* filename);					 //数据更新

int sqlite_create(char *filename)
{
	printf("Entering the management system for the first time\n");
	usleep(1.5E6);
	printf("User graph will be created automatically\n");
	usleep(1.5E6);
 	//打开或创建一个sqliete3数据库，返回一个sqlite3的数据库连接对象	
	int re = sqlite3_open(filename,&ppDb);
	
	char *err;
	re=sqlite3_exec(ppDb,"create table user(id int unique ,name text primary key not null,password text not null,root int check(root=0 or root=1));\
	insert into user values(0,'root','123456',1);",NULL,NULL,&err);
	if(re!=0)
	{
		printf("err:%s\n",err);
		return -1;
	}
	printf("User graph created successfully\n");
	//关闭数据库连接对象
	sqlite3_close(ppDb);	
	return -1;
}

int sqlite_add_data(char* filename)
{	
	//打开或创建一个sqliete3数据库，返回一个sqlite3的数据库连接对象	
	int re = sqlite3_open(filename,&ppDb);
	
	printf("Enter which graph to add object\n");
	
	sqlite_table(ppDb);
	
	char graphname[32];
	fgets(graphname,32,stdin);
	graphname[strlen(graphname)-1]='\0';
	
	sqlite_schema(graphname,ppDb);
	
	printf("Enter the number of objects to be added\n");
	int num,i=0;
	scanf("%d",&num);
	getchar();
	
	while(num--)
	{
		i++;
		printf("Enter the object data to be added\n");
		char insert[64];
		fgets(insert,64,stdin);
		
		char cmd[128];
		sprintf(cmd,"insert into %s values(%s);",graphname,insert);
		
		char *err;
		
		re=sqlite3_exec(ppDb,cmd,NULL,NULL,&err);
		
		if(re!=0)
		{
			printf("err:%s\n",err);
		}	
		printf("Already entered %d个,Still need %d个",i,num);
	}
	//关闭数据库连接对象
	sqlite3_close(ppDb);
	printf("Successful data entry\n");
}

int sqlite_add_graph(char *filename)
{
	//打开或创建一个sqliete3数据库，返回一个sqlite3的数据库连接对象	
	int re = sqlite3_open(filename,&ppDb);

	printf("Enter new graph creation\n");
	char graphname[32];
	fgets(graphname,32,stdin);
	
	printf("Enter create table directive\n");
	char creat[128];
	fgets(creat,128,stdin);
	
	char cmd[160];
	sprintf(cmd,"create table %s%s;",graphname,creat);
	
	char *err;
	
	re=sqlite3_exec(ppDb,cmd,NULL,NULL,&err);
	
	if(re!=0)
	{
		printf("err:%s\n",err);
	}		
	//关闭数据库连接对象
	sqlite3_close(ppDb);	
}

void sqlite_table(sqlite3* ppDb)
{
    char **ppDbresult;
	int i,nrow,ncolumn,index=1;
	int ret = sqlite3_get_table(ppDb,"select name from sqlite_master where type ='table'order by name ",&ppDbresult,&nrow,&ncolumn,NULL);
	if(ret == SQLITE_OK)
	{
		for(int i = 1;i <= nrow; i++)
			{
			    printf("%s\t",ppDbresult[index]);
                index++;
			}
		printf("\n");
		//printf("row=%d,column=%d\n",nrow,ncolumn);
	}
	sqlite3_free_table(ppDbresult);
}

void sqlite_schema(char* graphname,sqlite3* ppDb)
{
	char cmd[128];
	sprintf(cmd,"select sql from sqlite_master where type='table' and name = '%s'",graphname);
	//select sql from sqlite_master where type="table" AND name = "查询的表名"
	char **ppDbresult;
	int nrow,ncolumn,index=1;
	int ret = sqlite3_get_table(ppDb,cmd,&ppDbresult,&nrow,&ncolumn,NULL);
	if(ret == SQLITE_OK)
	{
		printf("The graph of %s screation statement is\n",graphname);
		printf("%s\n",ppDbresult[index]);
		//printf("row=%d,column=%d\n",nrow,ncolumn);
	}	
	sqlite3_free_table(ppDbresult);
}
 
int sqlite3_user_login(char* filename)
{	
	sqlite3_open(filename,&ppDb);
	char **ppDbresult;
	int flag,nrow,ncolumn,i,j=0;
	char ch;
	char name[32],password[32],cmd[128];
	while(1)
	{
		while(!j)
		{
			bzero(name,32);
			printf("Please enter username\n");
			fgets(name,32,stdin);
			name[strlen(name)-1]='\0';
			int ret = sqlite3_get_table(ppDb,"select name from user where id>=0;",&ppDbresult,&nrow,&ncolumn,NULL);
			if(ret == SQLITE_OK)
			{
				for(i=ncolumn;i<=ncolumn*(nrow+1)-1;i++)
				{
					if(strcmp(name,ppDbresult[i])==0)
					{
						printf("Username entered successfully\n");
						j=1;
						sqlite3_free_table(ppDbresult);
						break;
					}						
				}
				if(j==1)
					break;
				
				printf("Username doesn't exist,Press space to start re-entering or press another key to log out\n");
				ch=getchar();
				getchar();
				if(ch==' ')
				{
					sqlite3_free_table(ppDbresult);
					continue;
				}
				flag=-1;
				return flag;
			}
		}
		
		bzero(password,32);
		printf("Please enter password\n");
		fgets(password,32,stdin);
		password[strlen(password)-1]='\0';
		
		bzero(cmd,128);
		sprintf(cmd,"select password,root from user where name='%s';",name);
		//打开或创建一个sqliete3数据库，返回一个sqlite3的数据库连接对象	

		int ret = sqlite3_get_table(ppDb,cmd,&ppDbresult,&nrow,&ncolumn,NULL);
		if(ret == SQLITE_OK)
		{
			if(strcmp(password,ppDbresult[ncolumn])==0 && ppDbresult[ncolumn*(nrow+1)-1][0]=='1' )
			{
				printf("Login successful\n");
				printf("Root user\n");
				flag=1;
				break;
			}
			else if(strcmp(password,ppDbresult[ncolumn])==0 && ppDbresult[ncolumn*(nrow+1)-1][0]=='0')
			{
				printf("Login successful\n");
				printf("Normal user\n");
				flag=0;
				break;
			}
			else
			{
				printf("Password false,Press space to start re-entering or press another key to log out\n");
				ch=getchar();
				getchar();
				if(ch==' ')
				{
					sqlite3_free_table(ppDbresult);
					continue;
				}
				flag=-1;
				break;
			}  
		}
	}
	sqlite3_free_table(ppDbresult);
	return flag;
}

void sqlite3_user_registered(char* filename)
{
	char **ppDbresult;
	int nrow,ncolumn,id,num,i=0;
	
	printf("Has entered the registration system\n");
	printf("User graph creation statement is\n");
	int re = sqlite3_open(filename,&ppDb);
	sqlite_schema("user",ppDb);
	
	printf("Enter the number of objects to be added\n");
	scanf("%d",&num);
	getchar();
	
	int ret = sqlite3_get_table(ppDb,"select id from user ",&ppDbresult,&nrow,&ncolumn,NULL);
	if(ret == SQLITE_OK)
		id=atoi(ppDbresult[ncolumn*(nrow+1)-1]);
	sqlite3_free_table(ppDbresult);
	
	while(num--)
	{
		i++,id++;
		printf("Enter the object data to be added\n");
		char insert[64];
		fgets(insert,64,stdin);
		
		char cmd[128];
		sprintf(cmd,"insert into user values(%d,%s);",id,insert);
		
		char *err;
		re=sqlite3_exec(ppDb,cmd,NULL,NULL,&err);
		if(re!=0)
		{
			printf("err:%s\n",err);
		}	
		printf("Already entered %d个,Still need %d个",i,num);
	}
	//关闭数据库连接对象
	sqlite3_close(ppDb);
	printf("User registered successfully\n");
}

void sqlite3_graph_display(char*filename)
{
	int re = sqlite3_open(filename,&ppDb);
	printf("Enter which graph to dispaly\n");
	sqlite_table(ppDb);
	
	char graphname[32];
	fgets(graphname,32,stdin);
	graphname[strlen(graphname)-1]='\0';
	
	char cmd[128];
	sprintf(cmd,"select * from %s ",graphname);
	char **ppDbresult;
	int nrow,ncolumn,i,j,len;
	int ret = sqlite3_get_table(ppDb,cmd,&ppDbresult,&nrow,&ncolumn,NULL);
	if(ret == SQLITE_OK)
	{
 		for(i=0;i<=ncolumn*(nrow+1)-1;i++)
		{
			if(i<=ncolumn-1)
			{
				printf("%s",ppDbresult[i]);
				sqlite3_graph_display_tab(strlen(ppDbresult[i]));
				if(i==ncolumn-1)
				{	
					printf("\n");
					for(j=0;j<=ncolumn-1;j++)
						printf("----------  ");
					printf("\n");
				}
			}
			else
			{
				ppDbresult[i][10]='\0';
				printf("%s",ppDbresult[i]);
				sqlite3_graph_display_tab(strlen(ppDbresult[i]));
				if((i+1)%ncolumn==0)
					printf("\n");
			}
			
		} 
		//printf("row=%d,column=%d\n",nrow,ncolumn);
	}	
	sqlite3_free_table(ppDbresult);
	sqlite3_close(ppDb);
}

void sqlite3_graph_display_tab(int len)
{
	int i;
	for(i=len;i<12;i++)
		printf(" ");
}

int sqlite3_graph_delete(char* filename)
{
	int re = sqlite3_open(filename,&ppDb);
	printf("Enter which graph's datas you want to delete\n");
	sqlite_table(ppDb);
	
	char graphname[32];
	fgets(graphname,32,stdin);
	graphname[strlen(graphname)-1]='\0';
	
	char factor[128];
	printf("Enter the constraints of the delete instruction\n");
	fgets(factor,128,stdin);
	factor[strlen(factor)-1]='\0';
			
	char cmd[160];
	sprintf(cmd,"delete from %s where %s;",graphname,factor);
	
	char *err;
	re=sqlite3_exec(ppDb,cmd,NULL,NULL,&err);
	if(re!=0)
	{
		printf("err:%s\n",err);
		return -1;
	}
	
	//sqlite3_graph_arrange(graphname,"id",ppDb);
	//按id或者唯一标识符num 排序函数
	//*********************
	printf("Delete data successfully\n");
	sqlite3_close(ppDb);
}

int sqlie3_data_update(char* filename)
{
	int re = sqlite3_open(filename,&ppDb);
	printf("Enter which graph's datas you want to update\n");
	sqlite_table(ppDb);
	
	char graphname[32];
	fgets(graphname,32,stdin);
	graphname[strlen(graphname)-1]='\0';
	
	char factor[128];
	printf("Enter the constraints of the delete instruction\n");
	fgets(factor,128,stdin);
	factor[strlen(factor)-1]='\0';
	
	char update[32];
	printf("Enter update cmd\n");
	fgets(update,32,stdin);
	update[strlen(update)-1]='\0';
	
	char cmd[160];
	sprintf(cmd,"update %s set %s where %s;",graphname,update,factor);
	
	char *err;
	re=sqlite3_exec(ppDb,cmd,NULL,NULL,&err);
	if(re!=0)
	{
		printf("err:%s\n",err);
		return -1;
	}
	
	printf("Update data successfully\n");
	sqlite3_close(ppDb);
}

int sqlite3_graph_arrange(char* graphname,char* sortname,sqlite3* ppDb,int location)
{
	printf("开始排序\n");
	char cmd[128];
	sprintf(cmd,"select %s from %s",sortname,graphname);
	char **ppDbresult;
	int nrow,ncolumn,i,j,k;
	
	char cmd_second[128];
	sprintf(cmd_second,"select * from %s",graphname);
	char **qqDbresult;
	int row,column;
	
	int ret = sqlite3_get_table(ppDb,cmd,&ppDbresult,&nrow,&ncolumn,NULL);
	if(ret == SQLITE_OK)
	{
		char* sort[nrow];
		for(i=ncolumn,j=0;i<=ncolumn*(nrow+1)-1;i++,j++)
		{
			sort[j]=ppDbresult[i];
		}
		for(i=0;i<nrow-1;i++)
		{
			for(j=0;j<=nrow-2-i;j++)
			{
				if(atoi(sort[j])>atoi(sort[j+1]))
				{
					char temp[36];
					strcpy(temp,sort[j]);
					strcpy(sort[j],sort[j+1]);
					strcpy(sort[j+1],temp);
				}
			}
		}
		

		sqlite3_get_table(ppDb,cmd_second,&qqDbresult,&row,&column,NULL);
 		char* data[column*row];
		int num=0,flag;
		for(k=0;k<=6;k++)
		{
			for(i=column+location;i<=column*(row+1)-1;i=i+column)
			{
				if(strcmp(sort[k],qqDbresult[i])==0)
				{
					for(j=0,flag=i;j<=column-1;j++,flag++)
					{
						data[num]=qqDbresult[(flag-location)];
						num++;
					}
				}
			}
		}
 	/* 	for(i=0;i<=column*row-1;i++)
			printf("%s\n",data[i]);   */
		
		int re;
		char *err;
		char cmd_third[160];
		sprintf(cmd_third,"delete from %s;",graphname);
		sqlite3_exec(ppDb,cmd_third,NULL,NULL,&err);
		for(j=0;j<=row-1;j++)
		{
			for(i=j*column;i<=column*(j+1)-1;i++)
			{
				if(i==j*column)
				{
					char temp[160];
					bzero(cmd_third,160);
					sprintf(cmd_third,"insert into %s values(",graphname);
					sprintf(temp,"'%s',",data[i]);
					strcat(cmd_third,temp);
				}
				else if(i==column*(j+1)-1)
				{
					char temp[160];
					sprintf(temp,"'%s');",data[i]);
					strcat(cmd_third,temp);
				}
				else
				{
					char temp[160];
					sprintf(temp,"'%s',",data[i]);
					strcat(cmd_third,temp);
				}
			}
	 	 	re=sqlite3_exec(ppDb,cmd_third,NULL,NULL,&err);
			if(re!=0)
			{
				printf("err:%s\n",err);
				return -1;
			}  
			printf("%s\n",cmd_third);
		}
	
	}	
	sqlite3_free_table(ppDbresult);
	sqlite3_free_table(qqDbresult);
	sqlite3_close(ppDb);
}

int main(int argc,char *argv[])
{
	int ret=access(argv[1],F_OK);
	if(ret==-1)
	{                                                                                                  
		sqlite_create(argv[1]);
	}
	
	printf("*****************Has entered the management system of %s******************\n",argv[1]);
	printf("Press 1 to enter the user login system\n");
	printf("Press 2 to exit the database management system\n");
	
 	sqlite3_open(argv[1],&ppDb);
	sqlite3_graph_arrange("user","id",ppDb,0);
	sqlite3_close(ppDb); 
	
/* 	int flag,mark;
	scanf("%d",&flag);
	getchar();

	if(flag==1)
	{
	   mark=sqlite3_user_login(argv[1]);
	   if(mark==1)
	   {
		   while(1)
		   {
			   printf("Press 1 to enter the database collect\n");                                                                                                                                                                                                                                                                     
			   printf("Press 2 to create a new graph\n"); 
			   printf("Press 3 to enter the user registration system\n");
			   printf("Press 4 to enter the graph display system\n");
			   printf("Press 5 to enter the data deletion system\n");
			   printf("Press 6 to enter the update data system\n");
			   printf("Press X to exit the database management system\n");
			   scanf("%d",&flag);
			   getchar();
			   if(flag==1)
			   {
				   sqlite_add_data(argv[1]);
			   }
			   else if(flag==2)
			   {
				   sqlite_add_graph(argv[1]);
			   }
			   else if(flag==3)
			   {
				   sqlite3_user_registered(argv[1]); 
			   }
			   else if(flag==4)
			   {
				   sqlite3_graph_display(argv[1]);
			   }
			   else if(flag==5)
			   {
				   sqlite3_graph_delete(argv[1]);
			   }
			   else if(flag==6)
			   {
				   sqlie3_data_update(argv[1]);
			   }
		   }
	   }
	   else if (mark==0)
	   {
		   printf("Press 1 to enter the database collect\n");    
	   }
	   else
	   {
		   printf("Has exited the entry system\n");
	   }
	}
	else if(flag==2)
	{
		printf("Has exited the entry system\n");
	}   */
}