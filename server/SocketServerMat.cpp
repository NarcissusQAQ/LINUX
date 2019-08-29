#include "SocketMatTransmissionServer.h"
 
int main()
{
	SocketMatTransmissionServer socketMat;
	if (socketMat.socketConnect(6666) < 0)
	{
		return 0;
	}
	pthread_t tid[2];
	
	pthread_create(&tid[0],NULL,send_image,&socketMat);
	pthread_create(&tid[1],NULL,recv_image,&socketMat);
	
	pthread_join(tid[0],NULL);
	pthread_join(tid[1],NULL);
	cv::destroyAllWindows();
	socketMat.socketDisconnect();
	cout<<"已经关闭"<<endl;
}

void* send_image(void*arg)
{
 	cv::VideoCapture capture(0);
	
    capture.set(CV_CAP_PROP_FRAME_WIDTH, 320);  
    capture.set(CV_CAP_PROP_FRAME_HEIGHT, 240); 
	
	cv::Mat image;
	
	SocketMatTransmissionServer*p = static_cast<SocketMatTransmissionServer*>(arg);
	
	while (!p->flag)
	{
		if (!capture.isOpened())
			return 0;
		
		capture >> image;
		if (image.empty())
			return 0;
 
		cv::imshow("local_send_server",image);
		
		if(waitKey(30)>=0)  
		{
			p->flag=1;
		}
		
		p->transmit(image);
		cout<<p->flag<<endl;
	}
	
	capture.release();
}

void* recv_image(void*arg)
{
	SocketMatTransmissionServer*p = static_cast<SocketMatTransmissionServer*>(arg);
 
	cv::Mat image;
	while (!p->flag)
	{
		int q=p->receive(image);
		if(q > 0)
		{
			cout<<"recv***********************"<<endl;
			cv::imshow("local_recv_server",image);
			cv::waitKey(30);
		}
		else if(q==-2)
		{
			std::cout<<"已收到退出信号"<<endl;
			p->flag=1;
			break;
		}
		else
		{
			cout<<"异常退出"<<endl;
			p->flag=1;
			break;
		}
	}
}
