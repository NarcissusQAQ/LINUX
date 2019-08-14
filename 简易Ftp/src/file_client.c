/*
	简易版ftp ：客户端发送一个文件给服务端
*/
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
int init_socket(char* addr,char* port);
int send_message(int client_fd);
void rcvlist(int client_fd);
int sendfile(int client_fd);
int recvfile(int client_fd);


int init_socket(char* addr,char* port)
{
	
	//1.创建一个套接字  
	int client_fd = socket(AF_INET,SOCK_STREAM,0);
	if(client_fd == -1)
	{
		perror("socket error:");
		return -1;
	}
	
	//设置网络地址结构体
	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(port));
	server_addr.sin_addr.s_addr = inet_addr(addr);
	
	//2.连接服务器
	int re = connect(client_fd,(struct sockaddr *)&server_addr,sizeof(server_addr));
	if(re == -1)
	{     
		perror("connect error:");
		return -1;
	}
	printf("connect success\n");
	
	send_message(client_fd);	
	//4.关闭网络连接
	close(client_fd);
}


/* 
	发出信息函数send_message
*/
int send_message(int client_fd)
{
	char cmd[64]={0};
	while(1)
	{
		printf("******************************\n");
		printf("input cmd\n");
		scanf("%s",cmd);
		write(client_fd,cmd,64);
		if(strcmp(cmd,"ls") == 0)
		{
			rcvlist(client_fd);
		}
		else if (strcmp(cmd,"put") == 0)
		{
			sendfile(client_fd);
		}
		else if (strcmp(cmd,"get") == 0)
		{
			recvfile(client_fd);
		}
		else if (strcmp(cmd,"exit") == 0)
		{
			break;
		}
		else
		{
			printf("cmd cant find plz input again\n");
			continue;
		}
	}
}

/* 
	接收服务器端ls文件名函数 rcvlist
*/
void rcvlist(int client_fd)
{
	int re=0;
	while(1)
	{
		char buf[64]={0};
		re=read(client_fd,buf,64);
		if(re <= 0)
		{
			break;
		}
		if(strcmp(buf,"done")==0)
		{
			break;
		}
		printf("[%s]\n",buf);
		
	}
}




/* 
	发送文件函数sendfile
*/

int sendfile (int client_fd)
{	
	char filename[64] = {0};
	printf("plz input filename\n");
	scanf("%s",filename);
	write(client_fd,filename,64);
	
	struct stat st;
	stat(filename,&st);
	int filesize = st.st_size;
	printf("%d\n",filesize);
	write(client_fd,&filesize,4);
	
	int fd  = open(filename,O_RDONLY);
	if(fd == -1)
	{
		perror("open file error:");
		return -1;
	}	
	char buf[128] = {0};
	int readsize = 0;
	while(1)
	{
		int re = read(fd,buf,128);
		
		if(re > 0)
		{
			write(client_fd,buf,re);
			usleep(1800);
		}
		else
		{
			break;
		}
	}
	printf("send success\n");
}



/*
	接收文件函数recvfile
*/

int recvfile(int client_fd)
{
	printf("which file you want\n");
	char filename[64] = {0};
	scanf("%s",filename);
	write(client_fd,filename,64);
	printf("filename :%s\n",filename);
	
	
	int filesize;
	read(client_fd,&filesize,4);
	printf("filesize :%d\n",filesize);
	
	int fd = open(filename,O_RDWR|O_CREAT,0666);
	if(fd == -1)
	{
		perror("open file error:");
		return -1;
	}	
	
	char buf[128] = {0};
	int readsize = 0;
	while(readsize < filesize)
	{
		int re = read(client_fd,buf,128);
		
		if(re > 0)
		{
			readsize += re;
			write(fd,buf,re);
		}
		else
		{
			break;
		}
	}
	printf("rcv success\n");
	close(fd);
}

void main(int argc,char**argv)
{
	init_socket(argv[1],argv[2]);
}