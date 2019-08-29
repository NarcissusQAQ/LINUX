
 
#ifndef __SOCKETMATTRANSMISSIONCLIENT_H__
#define __SOCKETMATTRANSMISSIONCLIENT_H__
 
#include "opencv2/opencv.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
 
using namespace cv;
using namespace std;
 
//待传输图像默认大小为 640*480，可修改
#define IMG_WIDTH 320	// 需传输图像的宽
#define IMG_HEIGHT 240	// 需传输图像的高
#define PACKAGE_NUM 2
//默认格式为CV_8UC3
#define BUFFER_SIZE IMG_WIDTH*IMG_HEIGHT*3/PACKAGE_NUM

/*接收端
	-------------------------------------------------------------------*/
#define PACKAGE_NUM 2
 
#define IMG_WIDTH 320
#define IMG_HEIGHT 240
 
#define BLOCKSIZE IMG_WIDTH*IMG_HEIGHT*3/PACKAGE_NUM
 
struct recvBuf
{
	char buf[BLOCKSIZE];
	int flag;
	int endflag;
};
/*-----------------------------------------------------------*/

struct sentbuf
{
	char buf[BUFFER_SIZE];
	int flag;
	int endflag;
};
 
class SocketMatTransmissionClient
{
public:
	SocketMatTransmissionClient(void);
	~SocketMatTransmissionClient(void);
 
private:
	int sockClient;
	struct sentbuf snd_data;
	
	
	struct recvBuf rcv_data;
	int needRecv;
	int count;
	
 
public:
 
	// 打开socket连接
	// params :	IP		服务器的ip地址
	//			PORT	传输端口
	// return : -1		连接失败
	//			1		连接成功
	int socketConnect(const char* IP, int PORT);
 
 
	// 传输图像
	// params : image 待传输图像
	// return : -1		传输失败
	//			1		传输成功
	int transmit(cv::Mat image);
	
	// 断开socket连接
	void socketDisconnect(void);
	
	//静态成员标志位
	static int flag;
	
	// 传输图像
	// params : image	待接收图像
	//		image	待接收图像
	// return : -1		接收失败
	//			1		接收成功
	int receive(cv::Mat& image);
};

void* send_image(void*arg);

void* recv_image(void*arg);
 
#endif
