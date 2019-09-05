#ifndef _CHAT_H_
#define _CHAT_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <time.h>


#define PORT    8888

#define SUCCESS 10000
#define FAILURE 10001
#define ERROR   10002
#define FAULT   10003
#define YES     10004
#define NO      10005
#define OK  	10006

#define LOGO    1                       //登录
#define SIGN    2                       //注册
#define FORGET  3                       //忘记密码
#define QUIT    4                       //退出

#define SHOW    5                       //显示在线昵称
#define PCHAT   6                       //私聊
#define SHOWNUM 7                       //显示在线人数
#define GCHAT   8                       //群聊
#define BACK    9                       //注销
#define CHANGE  10                      //修改密码
#define GETNICK 11                      //获得当前昵称
#define MEMBER  12                      //会员
#define CLOSE   13                      //禁言
#define DOWN    14                      //踢人
#define GIVE    15                      //传输文件


struct data
{
    int  id;                            //帐号
    char pswd[20];                      //密码
    char nickname[20];                  //昵称
    char sex;                           //性别
    char phone[20];                     //手机号
    int flag;                           //判断标志位
    char message[200];                  //消息
    char fromname[20];                  //发送人
    char toname[20];                    //接受人
    int num;                            //在线人数
    int endflag;                        //结束标志位
    int time_year;                      //年
    int time_mon;                       //月
    int time_mday;                      //日
    int time_hour;                      //时
    int time_min;                       //分
    int time_sec;                       //秒
};
typedef struct data DATA;

struct trans
{
    char text[4096];                    //文件传送
    char filename[64];                  //文件名
    char fromname[20];                  //发送人
    char toname[20];                    //接收人
    int flag;                           //判断标志位
    int endflag;                        //文件结束标志
};
typedef struct trans TRAN;

struct status
{
    int id;                             //帐号
    int fd;                             //sockfd
    char nickname[20];                  //昵称
    int member_flag;                    //会员 0不是 1是会员
    int nospeak;                        //禁言 0   1禁
    struct status * next;
};
typedef struct status Status;
typedef Status *STATUS;

/*********************************************************服务器****************************************************/

void server_logo(int aim, DATA RecvBuf);
void server_sign(int aim, DATA RecvBuf);
int compare(void *para, int columnCount, char **columnValue, char **columnName);
void server_forget(int aim, DATA RecvBuf);
int getpswd(void *para, int columnCount, char **columnValue, char **columnName);
void server_quit(int aim);

void server_show(int aim);
void server_pchat(int aim, DATA RecvBuf);
void server_gchat(int aim, DATA RecvBuf);
void server_back(int aim);
void server_change(int aim, DATA RecvBuf);
int find(void *para, int columnCount, char **columnValue, char **columnName);
//void server_give(int aim, TRAN RecvBuf);

/*********************************************************客户端****************************************************/

void show_welcome();
void client_sign(int sockfd);
void show_sign();
int client_logo(int sockfd);
void show_logo();
void client_forget(int sockfd);
void show_forget();
void client_quit(int sockfd);
void show_quit();

void show_load();
void show_menu();
void *client_menu(void *arg);
void *Recv(void *arg);
void show_people(int sockfd);
void private_chat(int sockfd);
void group_chat(int sockfd);
void change_pswd(int sockfd);
void client_back(int sockfd);
void client_change(int sockfd);
void handel(int sig);
void delay();


void show_time();
void save_message(DATA *s);
void client_read();
void read_pchat();
void read_gchat();
void client_member(int sockfd);
void client_close(int sockfd);
void client_down(int sockfd);
void client_transform(int sockfd);
void client_receive();


#endif