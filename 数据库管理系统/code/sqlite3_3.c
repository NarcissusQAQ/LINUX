#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sqlite3.h>

// -lsqlite3

int sqlite_create(sqlite3* ppDb);                       //创建数据库(开机自运行并创建user表和超级用户root)

int sqlite_add_data(sqlite3* ppDb,int permission);					 	 //往对应表中加数据(普通用户,不能往user加入数据)
int sqlite3_graph_data_delete(sqlite3* ppDb,int permission);				 //删除对应表数据（普通用户，不能delete user）
int sqlie3_data_update(sqlite3* ppDb,int permission);					 //数据更新（普通用户，不能update user）
void sqlite3_graph_display(sqlite3* ppDb,int permission);                //显示对应表数据信息（普通用户，不能显示user）
void sqlite_table(sqlite3* ppDb);						 //显示数据库中表名（普通用户）
void sqlite_schema(char* graphname,sqlite3* ppDb);		 //显示对应表名的创建语句（普通用户）
int sqlite3_graph_arrange(char* graphname,char* sortname,sqlite3* ppDb,int location); //排序（普通用户，不能操作user） 按sortname的ascii或者数字大小排序 location为sortname表中哪一列（0开始）

int sqlite_add_graph(sqlite3* ppDb);					 //创建新表 (超级用户）
int sqlite3_user_login(sqlite3* ppDb);					 //用户登录 (判断是否为超级用户)
void sqlite3_user_registered(sqlite3* ppDb);			 //用户注册  (超级用户）
int sqlite3_user_id_arrange(sqlite3* ppDb);	             //使用户表从0开始排序（超级用户）
int sqlite3_graph_delete(sqlite3* ppDb);				 //删除表 （超级用户）

void sqlite3_graph_display_tab(int len);                 //制表符对齐(12字节) 功能函数
int sqlite3_graph_arrange_location(char* graphname,char* sortname,sqlite3* ppDb);  //查找sortname为哪一列 功能函数

int sqlite_create(sqlite3* ppDb)
{
	printf("Entering the management system for the first time\n");
	printf("User graph will be created automatically\n");
	
	char *err;
	int re=sqlite3_exec(ppDb,"create table user(id int unique ,name text primary key not null,password text not null,root int check(root=0 or root=1));\
	insert into user values(0,'root','123456',1);",NULL,NULL,&err);
	if(re!=0)
	{
		printf("err:%s\n",err);
		printf("********************\n");
		return -1;
	}
	printf("User graph created successfully\n");	
	return 1;
} 

int sqlite3_user_login(sqlite3* ppDb)
{	
	char **ppDbresult;
	int flag,nrow,ncolumn,i,j=0;
	char ch;
	char name[32],password[32],cmd[128];
	while(1)
	{
		while(!j)
		{
			bzero(name,32);
			printf("********************\n");
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
				
				printf("Username doesn't exist,\nPress space to start re-entering or press another key to log out\n");
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
		printf("********************\n");
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
				printf("Password false,\nPress space to start re-entering or press another key to log out\n");
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
	printf("********************\n");
	sqlite3_free_table(ppDbresult);
	return flag;
}

int sqlite_add_data(sqlite3* ppDb,int permission)
{	
	printf("********************\n");
	int re,num,i=0,errnum=0;
	char graphname[32];
	while(1)
	{
		printf("These are the database contains graphs\n");
		sqlite_table(ppDb);
		printf("********************\n");
		printf("Enter which graph to add object\n");
		bzero(graphname,32);
		fgets(graphname,32,stdin);
		graphname[strlen(graphname)-1]='\0';
		if(strcmp(graphname,"user")==0)
		{
			printf("Unable to operate the user graph,Please enter graphname again\n");
			continue;
		}
		else
			break;
	}
	
	sqlite_schema(graphname,ppDb);
	
	printf("********************\n");
	printf("Enter the number of objects to be added\n");
	scanf("%d",&num);
	getchar();
	
	while(num--)
	{
		i++;
		printf("********************\n");
		printf("Enter the object data to be added\n");
		char insert[64];
		fgets(insert,64,stdin);
		char cmd[128];
		sprintf(cmd,"insert into %s values(%s);",graphname,insert);
	
		char *err;
		
		re=sqlite3_exec(ppDb,cmd,NULL,NULL,&err);
		
		if(re!=0)
		{
			errnum++;
			printf("err:%s\n",err);
			printf("********************\n");
		}	
		printf("Already entered %d,need entered %d\nSuccess %d Mistake %d\n",i,num,i-errnum,errnum);
	}
	printf("Successful data entry\n");
	printf("********************\n");
}

void sqlite_table(sqlite3* ppDb)
{
    char **ppDbresult;
	int i,nrow,ncolumn,index=1;
	int ret = sqlite3_get_table(ppDb,"select name from sqlite_master where type ='table'order by name ",&ppDbresult,&nrow,&ncolumn,NULL);
	if(ret == SQLITE_OK)
	{
		for(i = 1;i <= nrow; i++)
			{
			    printf("%s\t",ppDbresult[index]);
                index++;
			}
		printf("\n");
	}
	sqlite3_free_table(ppDbresult);
}

void sqlite_schema(char* graphname,sqlite3* ppDb)
{
	char cmd[128];
	sprintf(cmd,"select sql from sqlite_master where type='table' and name = '%s'",graphname);
	char **ppDbresult;
	int nrow,ncolumn,index=1;
	int ret = sqlite3_get_table(ppDb,cmd,&ppDbresult,&nrow,&ncolumn,NULL);
	if(ret == SQLITE_OK)
	{
		printf("The graph of %s screation statement is\n",graphname);
		printf("%s\n",ppDbresult[index]);
	}	
	sqlite3_free_table(ppDbresult);
}

int sqlite_add_graph(sqlite3* ppDb)
{
	printf("********************\n");
	printf("Enter new graphname\n");
	char graphname[32];
	fgets(graphname,32,stdin);
	
	printf("********************\n");
	printf("Enter create table directive\n");
	char creat[128];
	fgets(creat,128,stdin);
	
	char cmd[160];
	sprintf(cmd,"create table %s%s;",graphname,creat);
	
	char *err;
	
	int re=sqlite3_exec(ppDb,cmd,NULL,NULL,&err);
	
	if(re!=0)
	{
		printf("err:%s\n",err);
		printf("********************\n");
		return -1;
	}			
	printf("Create new graph successfully\n");
	printf("********************\n");
}

void sqlite3_user_registered(sqlite3* ppDb)
{	
	char **ppDbresult;
	int nrow,ncolumn,id,num,i=0,errnum=0;
	id=sqlite3_user_id_arrange(ppDb);//注册前先从0排序并把最后的id值传出
	printf("********************\n");
	printf("Has entered the registration system\n");
	sqlite_schema("user",ppDb);
	
	printf("********************\n");
	printf("Enter the number of objects to be added\n");
	scanf("%d",&num);
	getchar();
	
	while(num--)
	{
		i++,id++;
		printf("********************\n");
		printf("Enter the object data to be added\n");
		char insert[64];
		fgets(insert,64,stdin);
		
		char cmd[128];
		sprintf(cmd,"insert into user values(%d,%s);",id,insert);
		
		char *err;
		int re=sqlite3_exec(ppDb,cmd,NULL,NULL,&err);
		if(re!=0)
		{
			errnum++;
			printf("err:%s\n",err);
			printf("********************\n");
		}	
		printf("Already entered %d,need entered %d\nSuccess %d Mistake %d\n",i,num,i-errnum,errnum);
	}
	//注册把id位从0开始重新排
	printf("User registered successfully\n");
	printf("********************\n");
}

void sqlite3_graph_display(sqlite3* ppDb,int permission)
{
	printf("********************\n");
	char graphname[32];
	while(1)
	{
		printf("These are the database contains graphs\n");
		sqlite_table(ppDb);
		printf("********************\n");
		printf("Enter which graph to display\n");
		bzero(graphname,32);
		fgets(graphname,32,stdin);
		graphname[strlen(graphname)-1]='\0';
		if(permission==0&&strcmp(graphname,"user")==0)
		{
			printf("Unable to operate the user graph,Please enter graphname again\n");
			continue;
		}
		else
			break;
	}
	printf("********************\n");
	sqlite_schema(graphname,ppDb);
	while(1)
	{
		while(1)
		{	
			printf("Enter sortname\n");
			char sortname[32];
			fgets(sortname,32,stdin);
			sortname[strlen(sortname)-1]='\0';
			int location=sqlite3_graph_arrange_location(graphname,sortname,ppDb);
			if(location==-1)
			{
				printf("Please enter again\n");
				continue;
			}
			sqlite3_graph_arrange(graphname,sortname,ppDb,location);
			break;
		}
		
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
		}
		char ch;
		printf("sort again?(y/n)\n");
		scanf("%c",&ch);
		getchar();
		if(ch=='y'||ch=='Y')
		{
			printf("********************\n");
			sqlite3_free_table(ppDbresult);
			continue;
		}
		else
		{
			printf("********************\n");
			sqlite3_free_table(ppDbresult);
			break;
		}
	}
}

void sqlite3_graph_display_tab(int len)
{
	int i;
	for(i=len;i<12;i++)
		printf(" ");
}

int sqlite3_graph_data_delete(sqlite3* ppDb,int permission)
{
	printf("********************\n");
	char graphname[32];
	while(1)
	{
		printf("These are the database contains graphs\n");
		sqlite_table(ppDb);
		printf("********************\n");
		printf("Enter which graph you want delete\n");
		bzero(graphname,32);
		fgets(graphname,32,stdin);
		graphname[strlen(graphname)-1]='\0';
		if(permission==0&&strcmp(graphname,"user")==0)
		{
			printf("Unable to operate the user graph,Please enter graphname again\n");
			continue;
		}
		else
			break;
	}
	
	printf("********************\n");
	printf("Enter the constraints of the delete instruction\n");
	char factor[128];
	fgets(factor,128,stdin);
	factor[strlen(factor)-1]='\0';
			
	char cmd[160];
	sprintf(cmd,"delete from %s where %s;",graphname,factor);
	
	char *err;
	int re=sqlite3_exec(ppDb,cmd,NULL,NULL,&err);
	if(re!=0)
	{
		printf("err:%s\n",err);
		return -1;
	}
	
	if(strcmp(graphname,"user")==0)
	{
		sqlite3_user_id_arrange(ppDb);
	}
	printf("Delete data successfully\n");
	printf("********************\n");
}

int sqlie3_data_update(sqlite3* ppDb,int permission)
{
	printf("********************\n");
	char graphname[32];
	while(1)
	{
		printf("These are the database contains graphs\n");
		sqlite_table(ppDb);
		printf("********************\n");
		printf("Enter which graph's datas you want to update\n");
		bzero(graphname,32);
		fgets(graphname,32,stdin);
		graphname[strlen(graphname)-1]='\0';
		if(permission==0&&strcmp(graphname,"user")==0)
		{
			printf("Unable to operate the user graph,Please enter graphname again\n");
			continue;
		}
		else
			break;
	}
	printf("********************\n");
	char factor[128];
	printf("Enter the constraints of the delete instruction\n");
	fgets(factor,128,stdin);
	factor[strlen(factor)-1]='\0';
	
	printf("********************\n");
	char update[32];
	printf("Enter update cmd\n");
	fgets(update,32,stdin);
	update[strlen(update)-1]='\0';
	
	char cmd[160];
	sprintf(cmd,"update %s set %s where %s;",graphname,update,factor);
	
	char *err;
	int re=sqlite3_exec(ppDb,cmd,NULL,NULL,&err);
	if(re!=0)
	{
		printf("err:%s\n",err);
		printf("********************\n");
		return -1;
	}
	
	if(strcmp(graphname,"user")==0)
	{
		sqlite3_user_id_arrange(ppDb);
	}
	
	printf("Update data successfully\n");
	printf("********************\n");
}

int sqlite3_graph_arrange(char* graphname,char* sortname,sqlite3* ppDb,int location)
{
	printf("********************\n");
	printf("Start sorting\n");
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
		
	 	for(i=0,k=0;i<=strlen(sort[0])-1;i++)
		{
			if(sort[0][i]>='A'&&sort[0][i]<='z')
				k=1;
		} 
		if(k==0)
		{
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
		}
		else
		{
			for(i=0;i<nrow-1;i++)
			{
				for(j=0;j<=nrow-2-i;j++)
				{
					if(strcmp(sort[j],sort[j+1])>0)
					{
						char temp[36];
						strcpy(temp,sort[j]);
						strcpy(sort[j],sort[j+1]);
						strcpy(sort[j+1],temp);
					}
				}
			}
		} 
		sqlite3_get_table(ppDb,cmd_second,&qqDbresult,&row,&column,NULL);
 		char* data[column*row];
		int num=0,flag;
		for(k=0;k<=row-1;k++)
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
			printf("********************\n");
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
				printf("********************\n");
				return -1;
			}  
			//printf("%s\n",cmd_third);
		}
	
	}	
	
	printf("Sorting successfully\n");
	printf("********************\n");
	sqlite3_free_table(ppDbresult);
	sqlite3_free_table(qqDbresult);
}

int sqlite3_graph_arrange_location(char* graphname,char* sortname,sqlite3* ppDb)
{
	char cmd[128];
	sprintf(cmd,"select * from %s",graphname);
	char **ppDbresult;
	int nrow,ncolumn,i;
	int ret = sqlite3_get_table(ppDb,cmd,&ppDbresult,&nrow,&ncolumn,NULL);
	if(ret == SQLITE_OK)
	{
		if(nrow==0||ncolumn==0)
		{
			printf("The graph is empty\n");
			sqlite3_free_table(ppDbresult);
			return -1;
		}
		else
		{
			for(i=0;i<=ncolumn-1;i++)
			{
				if(strcmp(sortname,ppDbresult[i])==0)
				{
					sqlite3_free_table(ppDbresult);
					return i;
				}
			}
			printf("The softname not found\n");
			sqlite3_free_table(ppDbresult);
			return -1;
		}
	}	
	sqlite3_free_table(ppDbresult);
}

int sqlite3_user_id_arrange(sqlite3* ppDb)
{
	char **ppDbresult;
	int nrow,ncolumn,i,j,k;
	
	
	int ret = sqlite3_get_table(ppDb,"select * from user ",&ppDbresult,&nrow,&ncolumn,NULL);
	if(ret == SQLITE_OK)
	{
		int id[nrow];
		id[0]=atoi(ppDbresult[ncolumn]);
		for(i=0;i<nrow-1;i++)
		{
			id[i+1]=id[i]+1;
		}
		for(i=0;i<=nrow-1;i++)
		{
			char tem[36];
			sprintf(tem,"%d",id[i]);
		}
		char* data[ncolumn*nrow];
		int num=0,flag;
		for(i=ncolumn;i<=ncolumn*(nrow+1)-1;i=i+ncolumn)
		{
			for(j=0,flag=i;j<=ncolumn-1;j++,flag++)
			{
				if(flag%ncolumn==0)
					;
				else
					data[num]=ppDbresult[(flag)];
				num++;
			}
		}
		int re;
		char *err;
		char cmd_third[160];
		sqlite3_exec(ppDb,"delete from user",NULL,NULL,&err);
		for(j=0;j<=nrow-1;j++)
		{
			for(i=j*ncolumn;i<=ncolumn*(j+1)-1;i++)
			{
				if(i==j*ncolumn)
				{
					char temp[160];
					bzero(cmd_third,160);
					sprintf(cmd_third,"insert into user values(");
					sprintf(temp,"'%d',",id[j]);
					strcat(cmd_third,temp);
				}
				else if(i==ncolumn*(j+1)-1)
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
				printf("********************\n");
				return -1;
			}   
			//printf("%s\n",cmd_third);
		}
		
		sqlite3_free_table(ppDbresult);
		return  id[nrow-1];
	}
}

int sqlite3_graph_drop(sqlite3* ppDb)
{
	printf("********************\n");
	printf("These are the database contains graphs\n");
	sqlite_table(ppDb);
	
	printf("********************\n");
	printf("Enter which graph to delete\n");
	char graphname[32];
	fgets(graphname,32,stdin);
	graphname[strlen(graphname)-1]='\0';
	
	char cmd[160];
	sprintf(cmd,"drop table %s",graphname);
	
	char *err;
	int re=sqlite3_exec(ppDb,cmd,NULL,NULL,&err);
	if(re!=0)
	{
		printf("err:%s\n",err);
		printf("********************\n");
		return -1;
	}
	printf("********************\n");
	return 1;
}

int main(int argc,char *argv[])
{		
	int ret=access(argv[1],F_OK);
	
	sqlite3 * ppDb = NULL;
	sqlite3_open(argv[1],&ppDb);
	
	if(ret==-1)
	{                                                                                                  
		sqlite_create(ppDb);
	}
	printf("*****************Has entered the management system of %s******************\n",argv[1]);
	printf("Press 1 to enter the user login system\n");
	printf("Press X to exit the database management system\n");
	
	int mark;
	char flag;
	scanf("%c",&flag);
	getchar();
	
	char graphname[32];
	if(flag=='1')
	{
	   mark=sqlite3_user_login(ppDb);
	   if(mark==1)
	   {
		   while(1)
		   {
			   printf("*********Normal***********\n");
			   printf("Press 1 to enter the database collect\n");                                                                                                                                                                                                                                                                     
			   printf("Press 2 to enter the data delete system\n");
			   printf("Press 3 to enter the update data system\n");
			   printf("Press 4 to enter the graph display system\n");
			   printf("Press 5 to .table database\n");
			   printf("Press 6 to .schema graph\n");
			   printf("*********ROOT***********\n");
			   printf("Press 7 to create a new graph\n"); 
			   printf("Press 8 to enter the user registration system\n");
			   printf("press 9 to drop table\n");
			   printf("Press X to exit the database management system\n");
			   scanf("%c",&flag);
			   getchar();
			   if(flag=='1')
			   {
				   sqlite_add_data(ppDb,mark);
			   }
			   else if(flag=='2')
			   {
				   sqlite3_graph_data_delete(ppDb,mark);
			   }	
			   else if(flag=='3')
			   {
				   sqlie3_data_update(ppDb,mark);
			   }		
			   else if(flag=='4')
			   {
				   sqlite3_graph_display(ppDb,mark);
				   printf("********************\n");
			   }
			   else if(flag=='5')
			   {
				   printf("********************\n");
				   sqlite_table(ppDb);
				   printf("********************\n");
			   }
			   else if(flag=='6')
			   {
				   printf("********************\n");
				   printf("These are the database contains graphs\n");
				   sqlite_table(ppDb);
				   printf("Enter which graph to add object\n");
				   char graphname[32];
				   fgets(graphname,32,stdin);
				   graphname[strlen(graphname)-1]='\0';
				   sqlite_schema(graphname,ppDb);
				   printf("********************\n");
			   }
			   else if(flag=='7')
			   {
				   sqlite_add_graph(ppDb);
			   }
			   else if(flag=='8')
			   {
				   sqlite3_user_registered(ppDb); 
			   }
			   else if(flag=='9')
			   {
				   sqlite3_graph_drop(ppDb);
			   }
			   else
			   {
				   break;
			   }
		   }
	   }
	   else if (mark==0)
	   {
		   while(1)
		   {
			   printf("*********Normal***********\n");
			   printf("Press 1 to enter the database collect\n");                                                                                                                                                                                                                                                                     
			   printf("Press 2 to enter the data delete system\n");
			   printf("Press 3 to enter the update data system\n");
			   printf("Press 4 to enter the graph display system\n");
			   printf("Press 5 to .table database\n");
			   printf("Press 6 to .schema graph\n");
			   scanf("%c",&flag);
			   getchar();
			   if(flag=='1')
			   {
				   sqlite_add_data(ppDb,mark);
			   }
			   else if(flag=='2')
			   {
				   sqlite3_graph_data_delete(ppDb,mark);
			   }	
			   else if(flag=='3')
			   {
				   sqlie3_data_update(ppDb,mark);
			   }		
			   else if(flag=='4')
			   {
				   sqlite3_graph_display(ppDb,mark);
				   printf("********************\n");
			   }
			   else if(flag=='5')
			   {
				   printf("********************\n");
				   sqlite_table(ppDb);
				   printf("********************\n");
			   }
			   else if(flag=='6')
			   {
				   char graphname[32];
				   while(1)
				   {
					   printf("These are the database contains graphs\n");
					   sqlite_table(ppDb);
					   printf("********************\n");
					   printf("Enter which graph to display\n");
					   bzero(graphname,32);
					   fgets(graphname,32,stdin);
					   graphname[strlen(graphname)-1]='\0';
					   if(strcmp(graphname,"user")==0)
					   {
						   printf("Unable to operate the user graph,Please enter graphname again\n");
						   continue;
					   }
					   else
							break;
				   }
				   sqlite_schema(graphname,ppDb);
				   printf("********************\n");
			   }
			   else
			   {
				   break;
			   }
		   }
	   }
	}
	else
	{
		printf("Has exited the entry system\n");
	}  
	sqlite3_close(ppDb);
}