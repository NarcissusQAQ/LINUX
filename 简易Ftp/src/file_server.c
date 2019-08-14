/*
	简易版ftp ：客户端发送一个文件给服务端
*/
#include <sys/types.h>         
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
#include <dirent.h>

int init_tcp_server(char*ip,char*port);
int rcv_message(int client_fd);
int recvfile(int client_fd);
void sendlist(int client_fd);
int sendfile (int client_fd);


int init_tcp_server(char*ip,char*port)
{
	//1.创建一个套接字  
	int server_fd= socket(AF_INET,SOCK_STREAM,0);
    if(server_fd == -1)
	{
		perror("socket error:");
		return -1;
	}
	
	//设置网络地址结构体	
	struct sockaddr_in  sa;
	sa.sin_family = AF_INET; 
	sa.sin_port = htons(atoi(port));
	//inet_aton("0.0.0.0",&sa.sin_addr);//自动获取本机ip
	sa.sin_addr.s_addr = htonl(INADDR_ANY);//自动获取本机ip

	//2.绑定服务端的ip地址和端口号到socket上
    int re = bind(server_fd,(struct sockaddr*)&sa,sizeof(sa));
	if(re == -1)
	{
		perror("bind error:");
		return -1;
	}		
	printf("bind success\n");
		
	//3.开启监听
    re = listen(server_fd,5);
	if(re == -1)
	{
		perror("listen error:");
		return -1;
	}		
	printf("listen success\n");
		
	//4.接受来自客户端的连接
	struct sockaddr_in client_addr;
	socklen_t len = sizeof(client_addr);
	
    int client_fd = accept(server_fd,(struct sockaddr*)&client_addr,&len);
	if(client_fd == -1)
	{
		perror("accept error:");
		return -1;
	}		
	printf("accept success\n");
	printf("client ip : %s\n",inet_ntoa(client_addr.sin_addr));	
	
	rcv_message(client_fd);
	close(server_fd);
}

/* 
	接受信息判别函数rcv_message
 */
int rcv_message(int client_fd)
{
	char cmd[64] = {0}; 
	while(1)
	{
		int re = read(client_fd,cmd,64);
		if(strcmp(cmd,"ls") == 0)
		{
			sendlist(client_fd);
		}
		else if (strcmp(cmd,"put") == 0)
		{
			recvfile(client_fd);
		}
		else if (strcmp(cmd,"get") == 0)
		{
			sendfile(client_fd);
		}
		else if (strcmp(cmd,"exit") == 0)
		{
			printf("exit\n");
			break;
		}
		else
		{
			printf("cmd rcv error\n");
		}
		printf("******************************\n");
	}
}


/* 
	ls函数sendlist
*/

void sendlist(int client_fd)
{
	char filename[64] = {0}; 
	DIR*dir = opendir("./");
	struct dirent*p = NULL;
	while(p = readdir(dir))
	{
		if(strcmp(p->d_name,".") == 0 || strcmp(p->d_name,"..") == 0)
		{
			continue;
		}
		else 
		{
			strcpy(filename,p->d_name);
			printf("[%s]\n",filename);
			write(client_fd,filename,strlen(filename));
			usleep(1800);
		}
	}
	write(client_fd,"done",5);
	closedir(dir);
}


/*
	接收文件函数recvfile
*/
int recvfile(int client_fd)
{
	char filename[64] = {0};
	int re = read(client_fd,filename,64);
	if(re == -1)
	{
		perror("read error:");
		return -1;
	}
	
	printf("filename :%s\n",filename);
	
	int filesize;
	re = read(client_fd,&filesize,4);
	if(re == -1)
	{
		perror("read error:");
		return -1;
	}
	
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

/* 
	发送文件函数sendfile 
*/

int  sendfile(int client_fd)
{	
	char filename[64] = {0};
	read(client_fd,filename,64);
	printf("filename :%s\n",filename);
	
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






int main(int argc,char **argv)
{
	//server socket 初始化
	init_tcp_server(argv[1],argv[2]);
}
