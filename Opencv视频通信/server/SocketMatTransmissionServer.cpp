#include "SocketMatTransmissionServer.h"

int SocketMatTransmissionServer::flag = 0;

SocketMatTransmissionServer::SocketMatTransmissionServer(void)
{
}
 
 
SocketMatTransmissionServer::~SocketMatTransmissionServer(void)
{
}
 
 
int SocketMatTransmissionServer::socketConnect(int PORT)
{
	int server_sockfd = socket(AF_INET,SOCK_STREAM, 0);
	
	int opt = 1;
    setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
 
	struct sockaddr_in server_sockaddr;
	server_sockaddr.sin_family = AF_INET;
	server_sockaddr.sin_port = htons(PORT);
	server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
 
	if(bind(server_sockfd,(struct sockaddr *)&server_sockaddr,sizeof(server_sockaddr))==-1)
	{
		perror("bind");
		return -1;
	}
 
	if(listen(server_sockfd,5) == -1)
	{
		perror("listen");
		return -1;
	}
 
	struct sockaddr_in client_addr;
	socklen_t length = sizeof(client_addr);
 
	sockConn = accept(server_sockfd, (struct sockaddr*)&client_addr, &length);
	if(sockConn<0)
	{
		perror("connect");
		return -1;
	}
	else
	{
		printf("connect successful!\n");
		return 1;
	}
	
	close(server_sockfd);
}
 
 
void SocketMatTransmissionServer::socketDisconnect(void)
{
	close(sockConn);
}
 
int SocketMatTransmissionServer::receive(cv::Mat& image)
{
	if(rcv_data.endflag==1)
	{
		return -2;
	}
	
	int returnflag = 0;
	cv::Mat img(IMG_HEIGHT, IMG_WIDTH, CV_8UC3, cv::Scalar(0));
	needRecv = sizeof(recvBuf);
	count = 0;
	memset(&rcv_data,0,sizeof(rcv_data));
 
	for (int i = 0; i < PACKAGE_NUM; i++)
	{
		int pos = 0;
		int len0 = 0;
 
		while (pos < needRecv)
		{
			len0 = recv(sockConn, (char*)(&rcv_data) + pos, needRecv - pos, 0);
			if (len0 < 0)
			{
				printf("Server Recieve Data Failed!\n");
				break;
			}
			pos += len0;
		}
 
		count = count + rcv_data.flag;
 
		int num1 = IMG_HEIGHT / PACKAGE_NUM * i;
		for (int j = 0; j < IMG_HEIGHT / PACKAGE_NUM; j++)
		{
			int num2 = j * IMG_WIDTH * 3;
			uchar* ucdata = img.ptr<uchar>(j + num1);
			for (int k = 0; k < IMG_WIDTH * 3; k++)
			{
				ucdata[k] = rcv_data.buf[num2 + k];
			}
		}
 
		if (rcv_data.flag == 2)
		{
			if (count == PACKAGE_NUM + 1)
			{
				image = img;
				returnflag = 1;
				count = 0;
			}
			else
			{
				count = 0;
				i = 0;
			}
		}
	}
	if(returnflag == 1)
		return 1;
	else
		return -1;
}

/*发送端
	-----------------*/
int SocketMatTransmissionServer::transmit(cv::Mat image)
{
	snd_data.endflag=flag;
	if (image.empty())
	{
		printf("empty image\n\n");
		return -1;
	}
 
	if(image.cols != IMG_WIDTH || image.rows != IMG_HEIGHT || image.type() != CV_8UC3)
	{
		printf("the image must satisfy : cols == IMG_WIDTH（%d）  rows == IMG_HEIGHT（%d） type == CV_8UC3\n\n", IMG_WIDTH, IMG_HEIGHT);
		return -1;
	}
 
	for(int k = 0; k < PACKAGE_NUM; k++) 
	{
		int num1 = IMG_HEIGHT / PACKAGE_NUM * k;
		for (int i = 0; i < IMG_HEIGHT / PACKAGE_NUM; i++)
		{
			int num2 = i * IMG_WIDTH * 3;
			uchar* ucdata = image.ptr<uchar>(i + num1);
			for (int j = 0; j < IMG_WIDTH * 3; j++)
			{
				snd_data.buf[num2 + j] = ucdata[j];
			}
		}
 
		if(k == PACKAGE_NUM - 1)
			snd_data.flag = 2;
		else
			snd_data.flag = 1;
 
		if (send(sockConn, (char *)(&snd_data), sizeof(snd_data), 0) < 0)
		{
			printf("send image error: %s(errno: %d)\n", strerror(errno), errno);
			return -1;
		}
	}
}
