#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/wait.h>
#include<pthread.h>
#include<bits/pthreadtypes.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<sys/shm.h>
#include<sys/sem.h>
#include<signal.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<linux/socket.h>
#include<errno.h>
#include<dirent.h>
#include<sys/mman.h>
#include<linux/input.h>
#include<time.h>
#include<linux/fb.h>
#include"lcd.h"
#include"touch.h"

int *p;//屏幕的第一个显存地址
#define W 24
#define H 40
#define WAIT_TIME 500

//定义direction的每个值代表的方向
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3
#define HIGH 20  //每一节蛇身的高度
#define WEIGHT 20 //每一节蛇身的宽度

int a[H][W];        //地图数组
int s[H*W][2];      //蛇身坐标数组
int sLength;        //蛇的长度
int direction;      //蛇的方向
int eated=0;        //标记是否吃到食物
int score;          //记录分数

void init()         //程序开始时的初始化操作
{
    srand((unsigned)time(NULL));            //设置随机数种子为现在的时间
    int i,j;
    for(i=0; i<H; i++)
    {
        a[i][0]=1;          //让第一列为1
        a[i][W-1]=1;        //让最后一列为1
    }
    for(j=0; j<W; j++)
    {
        a[0][j]=1;      //让第一行为1
        a[H-1][j]=1;    //让最后一行为1
    }
    sLength=4;          //让蛇的最初长度为4
    s[0][0]=H/2;
    s[0][1]=W/2;        //给蛇头坐标赋值
    for(i=1; i<4; i++)
    {
        s[i][0]=s[0][0]+i;
        s[i][1]=s[0][1];  //给刚开始的蛇身几个初始坐标
    }
    direction=UP;
}

int check(int ii,int jj)        //判断这个点能不能放食物，可以放返回1，不能放返回0
{
    if(a[ii][jj]==1)        //如果有障碍物，返回0
        return 0;
    int i;
    for(i=0; i<sLength; i++)
    {
        if(ii==s[i][0]&&jj==s[i][1])      //如果和其中一个蛇身重合，就返回0
            return 0;
    }
    if(ii==0||ii==H-1||jj==0||jj==W-1)      //如果在边界上面，返回0
        return 0;
    return 1;                       //最后筛选 过后的是符合条件的点
}

void food()
{
    int i,j;
    do
    {
        i=rand()%H;                 //生成0~H-1之间的一个数
        j=rand()%W;
    }
    while(check(i,j)==0);           //生成点直到满足条件
    a[i][j]=-1;                     //标记为食物
    display_bmp2("shiwu.bmp",i*20,j*20);                   //画出食物
}

void drawMap()                  //画地图
{
    //gotoxy(0,0);
    int i,j;
    for(i=0; i<H; i++)
    {
        for(j=0; j<W; j++)          //两重for循环遍历数组
        {
            if(a[i][j]!=0)          //为0输出空格
            display_bmp2("shiwu.bmp",i*20,j*20);
        }
    }
}

void move()
{
    int i;
    display_bmp2("heise.bmp",s[sLength-1][0]*20,s[sLength-1][1]*20);  //蛇尾用黑色填充                          
    if(eated)               //如果吃到了食物
    {
        sLength++;
        eated=0;        //设置为0，不然无限变长
    }
    for(i=sLength-1; i>0; i--)  //从尾巴开始，每一个点的位置等于它前面一个点的位置
    {
        s[i][0]=s[i-1][0];
        s[i][1]=s[i-1][1];
    }
    switch(direction)
    {
    case UP:
        s[0][0]--;
        break;
    case DOWN:
        s[0][0]++;
        break;
    case LEFT:
        s[0][1]--;
        break;
    case RIGHT:
        s[0][1]++;
        break;
    }

}

void drawSnake()                //画蛇
{
    int i;
    for(i=0; i<sLength; i++)
    {
        display_bmp2("shiwu.bmp",s[i][0]*20,s[i][1]*20);                   //在这个位置画蛇
    }
}

void *key(void*data)
{

    while(1)
    {
        int i=direction_panduan();
        switch(i)
        {
           
        case 3:
            if(direction!=DOWN)         //不能缩头吧。。。
                direction=UP;
            break;
        
        case 4:
            if(direction!=UP)
                direction=DOWN;
            break;
        
        case 1:
            if(direction!=RIGHT)
                direction=LEFT;
            break;
        
        case 2:
            if(direction!=LEFT)
                direction=RIGHT;
            break;
        }
    }
}

int gameOver()
{
    int isGameOver=0;
    int sX=s[0][0],sY=s[0][1];      //蛇头的x坐标和y坐标
    if(sX==0||sX==H-1||sY==0||sY==W-1)
        isGameOver=1;
    int i;
    for(i=1; i<sLength; i++)    //判断有没有吃到自己
    {
        if(s[i][0]==sX&&s[i][1]==sY)
            isGameOver=1;
    }
    return isGameOver;
}

int main()
{
    init(); //程序开始时的初始化操作
    int fd=lcd_init(); 
    display_bmp2("start.bmp",0,0);
    click_panduan(0,0,800,480);
    display_bmp2("black.bmp",0,0);                 //背景为黑色
    drawMap();                 //画地图
    food();
    pthread_t pt;
    pthread_create(&pt, NULL, key, NULL);
    while(1)
    {
        drawSnake();                //画蛇
        usleep(300000);           //等待一段时间
        move();                     //移动蛇（修改蛇身数组的数据）
        if(gameOver())
        {             
            printf("Game Over\n");
            display_bmp2("end.bmp",0,0);
            break;
        }
        if(a[s[0][0]][s[0][1]]==-1) //如果蛇头碰到食物，就重新投放食物，并且把食物点重置为0
        {
            eated=1;             //标记已经吃到食物
            score+=10;
            food();
            a[s[0][0]][s[0][1]]=0;  //去掉食物
        }
        

    }
    pthread_join(pt,NULL);
    close(fd);
    return 0;
}