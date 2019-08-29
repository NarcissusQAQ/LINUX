#include "chat.h"

extern pthread_t tid[2];
extern int sign;
extern int cfd;

char my_nickname[20];


void show_welcome()
{
    printf("\n\n\n\n\n\t\t\t\t\t欢迎来到聊天室!\n");
    printf("\t\t\t\t      WELCOME TO CHAT ROOM\n");
    printf("\n\n\t\t1. 登录\n");
    printf("\n\t\t2. 注册\n");
    printf("\n\t\t3. 忘记密码\n");
    printf("\n\t\t4. 退出\n");
    printf("\n\n\n\t请输入您的选择:\n");
}



/*****************************************注册****************************************/

void show_sign()
{
    printf("\n\n\n\n\n\t\t\t\t\t注册帐号\n\n\n");
    printf("\t\t请输入您的帐号:\n\n");
    printf("\t\t请输入您的密码:\n\n");
    printf("\t\t请输入您的昵称:\n\n");
    printf("\t\t请输入您的性别(帅哥m,美女f):\n\n");
    printf("\t\t请输入您的手机号码:\n\n");
}

void client_sign(int sockfd)
{
    int ret;
    system("clear");
    show_sign();
    DATA SendBuf;
    memset(&SendBuf, 0, sizeof(SendBuf));
    scanf("%d %s %s %c %s", &SendBuf.id, SendBuf.pswd, SendBuf.nickname, &SendBuf.sex, SendBuf.phone);
    SendBuf.flag = SIGN;

    ret = send(sockfd, &SendBuf, sizeof(SendBuf), 0);
    if(-1 == ret)
    {
        perror("send");
        exit(1);
    }

    DATA RecvBuf;
    memset(&RecvBuf, 0, sizeof(RecvBuf));
    ret = recv(sockfd, &RecvBuf, sizeof(RecvBuf), 0);
    if(-1 == ret)
    {
        perror("recv");
        exit(1);
    }

    if(RecvBuf.flag == FAILURE)
    {
        printf("\n\t\t\t注册失败,该用户已存在!\n\n");
        sleep(1);
    }
    else if(RecvBuf.flag == SUCCESS)
    {
        system("clear");
        printf("\n\n\n\n\t注册中...\n");
        system("clear");
        printf("\n\n\n\n\t注册成功!\n\n");
    }
}

/***********************************************登录******************************************/

void show_logo()
{
    printf("\n\n\n\n\n\t\t\t\t\t登录帐号\n\n\n");
    printf("\t\t请输入您的帐号:\n\n");
    printf("\t\t请输入您的密码:\n\n");
}

int client_logo(int sockfd)
{
    int ret;
    system("clear");
    show_logo();
    DATA SendBuf;
    memset(&SendBuf, 0, sizeof(SendBuf));
    scanf("%d %s", &SendBuf.id, SendBuf.pswd);
    SendBuf.flag = LOGO;

    ret = send(sockfd, &SendBuf, sizeof(SendBuf), 0);
    if(-1 == ret)
    {
        perror("send");
        exit(1);
    }

    DATA RecvBuf;
    memset(&RecvBuf, 0, sizeof(RecvBuf));
    ret = recv(sockfd, &RecvBuf, sizeof(RecvBuf), 0);
    if(-1 == ret)
    {
        perror("recv");
        exit(1);
    }

    if(RecvBuf.flag == SUCCESS)
    {
        system("clear");
        printf("\n\n\n\n\t登录中...\n");
        sleep(1);
        return SUCCESS;
    }
    else if(RecvBuf.flag == FAILURE)
    {
        printf("登录失败,该用户已上线!\n");
        sleep(1);
    }
    else if(RecvBuf.flag == ERROR)
    {
        printf("登录失败,密码不正确!\n");
        sleep(1);
    }
    else if(RecvBuf.flag == FAULT)
    {
        printf("登录失败,用户未注册!\n");
        sleep(1);
    }
    return FAILURE;
}

/*********************************************忘记密码********************************************/

void show_forget()
{
    printf("\n\n\n\n\n\t\t\t\t\t忘记密码\n\n\n");
    printf("\t\t请输入您的帐号:\n\n");
    printf("\t\t请输入您的昵称:\n\n");
    printf("\t\t请输入您的性别(帅哥m,美女f):\n\n");
    printf("\t\t请输入您的手机号码:\n\n");
}

void client_forget(int sockfd)
{
    char choice[20];
    int ret;
    system("clear");
    show_forget();
    DATA SendBuf;
    memset(&SendBuf, 0, sizeof(SendBuf));
    scanf("%d %s %c %s", &SendBuf.id, SendBuf.nickname, &SendBuf.sex, SendBuf.phone);
	printf("%d\n%s\n%c\n%s\n",SendBuf.id,SendBuf.nickname,SendBuf.sex,SendBuf.phone);
    SendBuf.flag = FORGET;
    ret = send(sockfd, &SendBuf, sizeof(SendBuf), 0);
    if(-1 == ret)
    {
        perror("send");
        exit(1);
    }
	usleep(2000);

    DATA RecvBuf;
    memset(&RecvBuf, 0, sizeof(RecvBuf));
    ret = recv(sockfd, &RecvBuf, sizeof(RecvBuf), 0);
    if(-1 == ret)
    {
        perror("recv");
        exit(1);
    }

    if(RecvBuf.flag == SUCCESS)
    {
        system("clear");
        printf("\n\n\n\n\n密码找回中:\n\n");
        sleep(1);
        printf("该用户的密码是: %s\n", RecvBuf.pswd);
        while(1)
        {
            printf("请输入yes,进行下一步:\n");
            scanf("%s", choice);
            if(!strcmp(choice, "yes"))
            {
                break;
            }
        }
    }
    else if(RecvBuf.flag == FAILURE)
    {
        printf("该用户已上线!\n");
        sleep(1);
    }
    else if(RecvBuf.flag == ERROR)
    {
        printf("该用户未注册!\n");
        sleep(1);
    }
    return;
}

/***************************************************退出*************************************************/

void client_quit(int sockfd)
{
    int ret;
    DATA SendBuf;
    memset(&SendBuf, 0, sizeof(SendBuf));
    SendBuf.flag = QUIT;
    ret = send(sockfd, &SendBuf, sizeof(SendBuf), 0);
    if(-1 == ret)
    {
        perror("send");
        exit(1);
    }

    return;
}

void show_quit()
{
    system("clear");
    printf("\n\n\n\n\n\n\n\n");
    printf("                                                                                     =============================================\n");
    printf("                                                                                                  且将新火试新茶,诗酒趁年华\n");
    printf("                                                                                     =============================================\n");
    sleep(2);
    system("clear");
}

/**************************************************LOADING**************************************************/

void show_load()
{
    printf("\n\n\n\n\t登录成功!\n\n\n\n\n\n\n");
    sleep(1);
    printf("                                                                                     =============================================\n");
    printf("                                                                                                       载入中...\n");
    printf("                                                                                     =============================================\n");
}

/************************************************客户端菜单*************************************************/

void show_menu()
{
    show_time();
    printf("\n\n\t\t\t\t\t\t主菜单MAIN MENU\n");
    printf("\n\n\t\t1. 私聊\t\t\t2. 群聊\n\n");
    printf("\t\t3. 修改密码\t\t4. 刷新\n\n");
    printf("\t\t5. 查询消息记录\t\t6. 会员充值\n\n");
    printf("\t\t7. 禁言\t\t\t8. 踢人\n\n");
    printf("\t\t9. 注销\t\t\t10 .退出\n\n");
    printf("\t\t11. 文件传输\n\n");
}

void *client_menu(void *arg)
{
    bzero(&my_nickname, sizeof(my_nickname));
    sign = 5;
    char choice[10] = {0};

    system("clear");
    while(1)
    {
        system("clear");
        memset(&choice, 0, sizeof(choice));
        show_menu();
        show_people(*(int *)arg);
        printf("\n\n请输入您的选择:\n");

        scanf("%s", choice);
        switch(atoi(&choice[0]))
        {
            case 1:
                private_chat(*(int *)arg);
                break;
            case 2:
                group_chat(*(int *)arg);
                break;
            case 3:
                change_pswd(*(int *)arg);
                break;
            case 4:
                break;
            case 5:
                client_read();
                break;
            case 6:
                client_member(*(int *)arg);
                break;
            case 7:
                client_close(*(int *)arg);
                break;
            case 8:
                client_down(*(int *)arg);
                break;
            case 9:
                client_back(*(int *)arg);
                sign = 1;
                break;
            case 10:
                client_back(*(int *)arg);
                sign = 2;
                break;
            case 11:
                client_transform(*(int *)arg);
                break;
            default:
                printf("请重输:\n");
                break;
        }

        if(1 == sign || 2 == sign)
        {
            pthread_cancel(tid[1]);
            return;
        }
    }
}



/************************************************接受消息**************************************************/

void *Recv(void *arg)
{
    static recv_count;
    int ret, oldtype;
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &oldtype);

    DATA RecvBuf;
    while(1)
    {
        memset(&RecvBuf, 0, sizeof(RecvBuf));
        ret = recv(*(int *)arg, &RecvBuf, sizeof(RecvBuf), 0);
        if(-1 == ret)
        {
            perror("recv");
            exit(1);
        }

		//printf("flag:%d, endflag:%d, nickname:%s, message:%s\n", RecvBuf.flag, RecvBuf.endflag, RecvBuf.nickname, RecvBuf.message);
        if(RecvBuf.flag == PCHAT)
        {
            if(RecvBuf.endflag == FAILURE)
            {
                delay();
                printf("消息发送失败,您已被禁言!\n");
            }
            else if(RecvBuf.endflag == NO)
            {
                delay();
                printf("\n\t消息发送失败,目标用户不在线!\n");
            }
            else if(RecvBuf.endflag == FAULT)
            {
                delay();
                printf("\n\t消息发送失败,用户不能发消息给自己!\n");
            }
            else if(RecvBuf.endflag == YES)
            {
                printf("\n\n\n\t您有新消息!\n");
                printf("\t\t%s对您说%s\n\n\n", RecvBuf.fromname, RecvBuf.message);
                strcpy(RecvBuf.nickname, my_nickname);
                save_message(&RecvBuf);
            }
        }
        else if(RecvBuf.flag == GCHAT)
        {
			printf("recvbuf.endflag=%d\n",RecvBuf.endflag );
            if(RecvBuf.endflag == FAILURE)
            {
                //delay();
                printf("消息发送失败,您已被禁言!\n");
            }
            else
            {
                printf("\n\n\n\t您有新的群消息!\n");
                printf("\t\t%s对您说:%s\n\n\n", RecvBuf.fromname, RecvBuf.message);
                strcpy(RecvBuf.toname, RecvBuf.fromname);
                strcpy(RecvBuf.fromname, "all");
                strcpy(RecvBuf.nickname, my_nickname);
                save_message(&RecvBuf);
            }
        }
        else if(RecvBuf.flag == ERROR)
        {
            delay();
            printf("\n\t消息发送失败,当前只有您一人在线!\n");
        }
        else if(RecvBuf.flag == SHOWNUM)
        {
            printf("\n\t当前有%d个用户在线:\n\n", RecvBuf.num);
        }
        else if(RecvBuf.flag == SHOW)
        {
            if(RecvBuf.endflag == NO)
            {
                printf("\t\t%d. %s\n", recv_count + 1, RecvBuf.nickname);
                recv_count++;
            }
            else if(RecvBuf.endflag == YES)
            {
                printf("\t\t%d. %s\n", recv_count + 1, RecvBuf.nickname);
                recv_count = 0;
            }
        }
        else if(RecvBuf.flag == CHANGE)
        {
            if(RecvBuf.endflag == FAILURE)
            {
                delay();
                printf("\n\n\t修改失败,旧密码输入不正确!\n");
            }
            else if(RecvBuf.endflag == SUCCESS)
            {
                delay();
                printf("\n\n\t修改密码成功!\n");
            }
        }
        else if(RecvBuf.flag == GETNICK)
        {
            strcpy(my_nickname, RecvBuf.fromname);
            printf("\n\n\n\t当前用户:%s\n", my_nickname);
        }
        else if(RecvBuf.flag == MEMBER)
        {
            if(RecvBuf.endflag == SUCCESS)
            {
                delay();
                printf("\n\n\t会员充值成功!\n");
            }
            else if(RecvBuf.endflag == YES)
            {
                delay();
                printf("\n\n\t您已经是会员了!\n");
            }
            else if(RecvBuf.endflag == FAILURE)
            {
                delay();
                printf("\n\n\t会员充值失败!\n");
            }
        }
        else if(RecvBuf.flag == CLOSE)
        {
            if(RecvBuf.endflag == SUCCESS)
            {
                delay();
                printf("\n\n\t禁言成功,该用户已经不能发言了!\n");
            }
            else if(RecvBuf.endflag == FAILURE)
            {
                delay();
                printf("\n\n\t禁言失败,该用户不在线!\n");
            }
            else if(RecvBuf.endflag == ERROR)
            {
                delay();
                printf("\n\n\t禁言失败,您不是会员!\n");
            }
            else if(RecvBuf.endflag == FAULT)
            {
                delay();
                printf("\n\n\t禁言失败,对方也是会员!\n");
            }
            else if(RecvBuf.endflag == NO)
            {
                delay();
                printf("\n\n\t禁言失败,您不能禁言您自己!\n");
            }
            else if(RecvBuf.endflag == YES)
            {
                delay();
                printf("\n\n\t您被禁言了!\n");
            }
        }
        else if(RecvBuf.flag == DOWN)
        {
            if(RecvBuf.endflag == FAILURE)
            {
                delay();
                printf("\n\n\t踢人失败,您不是会员!\n");
            }
            else if(RecvBuf.endflag == ERROR)
            {
                delay();
                printf("\n\n\t踢人失败,您不能踢会员!\n");
            }
            else if(RecvBuf.endflag == SUCCESS)
            {
                delay();
                printf("\n\n\t踢人成功!\n");
            }
            else if(RecvBuf.endflag == NO)
            {
                delay();
                printf("\n\n\t踢人失败,您不能踢你自己!\n");
            }
            else if(RecvBuf.endflag == YES)
            {
                printf("\n\n\t您被踢了!\n");
                sleep(1);
                DATA SendBuf;
                memset(&SendBuf, 0, sizeof(SendBuf));
                SendBuf.flag = BACK;
                ret = send(cfd, &SendBuf, sizeof(SendBuf), 0);
                if(-1 == ret)
                {
                    perror("send");
                    exit(1);
                }
                pthread_cancel(tid[0]);
                return;
            }
        }
        else if(RecvBuf.flag == GIVE)
        {
            if(RecvBuf.endflag == FAILURE)
            {
                delay();
                printf("\n\n\t文件传输失败,目标用户不在线!\n");
            }
            else if(RecvBuf.endflag == ERROR)
            {
                delay();
                printf("\n\n\t文件传输失败,不能将文件传给自己!\n");
            }
            else if(RecvBuf.endflag == OK)
            {
                delay();
                printf("\n\n\t文件发送成功!\n");
            }
            else if(RecvBuf.endflag == YES)
            {
                client_receive();
            }
        }
    }
}

/************************************************显示在线人数,昵称****************************************/

void show_people(int sockfd)
{
    int ret;
    DATA SendBuf;
    memset(&SendBuf, 0, sizeof(SendBuf));
    SendBuf.flag = SHOW;
    ret = send(sockfd, &SendBuf, sizeof(SendBuf), 0);
    if(-1 == ret)
    {
        perror("send");
        exit(1);
    }
}


/*************************************************私聊****************************************************/

void private_chat(int sockfd)
{
    system("clear");
    int ret;
    DATA SendBuf;

    while(1)
    {
        memset(&SendBuf, 0, sizeof(SendBuf));
        printf("\n\n\t请输入您聊天对象的昵称(输入end退出聊天):\n");
        scanf("%s", SendBuf.nickname);
        if(!strcmp(SendBuf.nickname, "end"))
        {
            break;
        }
        printf("\t请输入您聊天的内容:\n");
        scanf("%s", SendBuf.message);
        SendBuf.flag = PCHAT;

        ret = send(sockfd, &SendBuf, sizeof(SendBuf), 0);
        if(-1 == ret)
        {
            perror("send");
            exit(1);
        }

        strcpy(SendBuf.fromname, my_nickname);
        save_message(&SendBuf);
    }
    return;
}

/************************************************群聊***************************************************/

void group_chat(int sockfd)
{
    int ret;
    DATA SendBuf;
    memset(&SendBuf, 0, sizeof(SendBuf));

    printf("\n\n\t请输入您的聊天内容:\n");
    scanf("%s", SendBuf.message);
    SendBuf.flag = GCHAT;

    ret = send(sockfd, &SendBuf, sizeof(SendBuf), 0);
    if(-1 == ret)
    {
        perror("send");
        exit(1);
    }
    strcpy(SendBuf.fromname, my_nickname);
    strcpy(SendBuf.nickname, "all");
    save_message(&SendBuf);
    delay();
}

/**************************************************修改密码**********************************************/

void change_pswd(int sockfd)
{
    char npswd1[20], npswd2[20];
    memset(&npswd1, 0, sizeof(npswd1));
    memset(&npswd2, 0, sizeof(npswd2));

    int ret;
    DATA SendBuf;
	bzero(&SendBuf,sizeof(SendBuf));
    printf("\n\n\t请输入您的旧密码:\n");
    printf("\n\t请输入您的新密码:\n");
    printf("\n\t请核实您的新密码:\n");
    scanf("%s %s %s", SendBuf.pswd, npswd1, npswd2);
    while(strcmp(npswd1, npswd2) || (!strcmp(SendBuf.pswd, npswd1)) || (!strcmp(SendBuf.pswd, npswd2)))
    {
        printf("\n\n\t您输入的两次密码不一致,新密码不能和旧密码一样!\n");
        memset(&npswd1, 0, sizeof(npswd1));
        memset(&npswd2, 0, sizeof(npswd2)); 
        printf("\n\n\t请重新输入您的新密码:\n");
        printf("\n\t请重新核实您的新密码:\n");
        scanf("%s %s", npswd1, npswd2);
    }

    SendBuf.flag = CHANGE;
    strcpy(SendBuf.message, npswd1);
    ret = send(sockfd, &SendBuf, sizeof(SendBuf), 0);
    if(-1 == ret)
    {
        perror("send");
        exit(1);
    }
}

/***************************************************注销**************************************************/

void client_back(int sockfd)
{
    int ret;
    DATA SendBuf;
    memset(&SendBuf, 0, sizeof(SendBuf));
    SendBuf.flag = BACK;
    ret = send(sockfd, &SendBuf, sizeof(SendBuf), 0);
    if(-1 == ret)
    {
        perror("send");
        exit(1);
    }
}   

/****************************************************屏蔽kill**********************************************/

void handel(int aig)
{
    int ret;
    DATA SendBuf;
    if(5 == sign)
    {
        memset(&SendBuf, 0, sizeof(SendBuf));
        SendBuf.flag = BACK;
        ret = send(cfd, &SendBuf, sizeof(SendBuf), 0);
        if(-1 == ret)
        {
            perror("send");
            exit(1);
        }

        pthread_cancel(tid[1]);
    }

    memset(&SendBuf, 0, sizeof(SendBuf));
    SendBuf.flag = QUIT;
    ret = send(cfd, &SendBuf, sizeof(SendBuf), 0);
    if(-1 == ret)
    {
        perror("send");
        exit(1);
    }

    close(cfd);
    show_quit();
    exit(1);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*********************************************show time*******************************************************/

void show_time()
{
    time_t now;
    now = time(NULL);
    struct tm * tm_now;
    tm_now = localtime(&now);

    printf("当前时间:%d年 %d月 %d日 %d时 %d分 %d秒\n", tm_now->tm_year+1990, tm_now->tm_mon+1, tm_now->tm_mday, tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec);
}

/************************************************保存记录***************************************************/

void save_message(DATA *s)
{
    char filename[64];
    FILE *fp;
    time_t now;
    now = time(NULL);
    struct tm * tm_now;
    tm_now = localtime(&now);

    s->time_year = tm_now->tm_year+1990;
    s->time_mon = tm_now->tm_mon+1;
    s->time_mday = tm_now->tm_mday;
    s->time_hour = tm_now->tm_hour;
    s->time_min = tm_now->tm_min;
    s->time_sec = tm_now->tm_sec;


    if(s->flag == PCHAT)
    {
        sprintf(filename, "%s发给%s_pchat", s->fromname, s->nickname);
        fp = fopen(filename, "a+b");
        if(NULL == fp)
        {
            perror("fopen");
            exit(1);
        }
        fwrite(s, 1, sizeof(DATA), fp);
        fclose(fp);
    }
    else if(s->flag == GCHAT)
    {
        sprintf(filename, "%s发给%s_gchat", s->fromname, s->nickname);
        fp = fopen(filename, "a+b");
        if(NULL == fp)
        {
            perror("fopen");
            exit(1);
        }
        fwrite(s, 1, sizeof(DATA), fp);
        fclose(fp);
    }
    return;
}

/******************************************************读取消息******************************************/

void client_read()
{
    int mark = 0;
//  system("clear");
    printf("\n\n\n");
    char choice[10];

    while(1)
    {
        printf("\n\n\t\t\t\t聊天记录查询!\n");
        printf("\n\n\n\t1. 私聊\n\n");
        printf("\t2. 群聊\n\n");
        printf("\t3. 退出\n\n");
        printf("\t请输入您的选项:\n");
        scanf("%s", choice);
        switch(atoi(&choice[0]))
        {
            case 1:
                read_pchat();
                break;
            case 2:
                read_gchat();
                break;
            case 3:
                return;
            default:
                printf("请重新输入:\n");
                break;
        }
    }
}

void read_pchat()
{
    system("clear");
    printf("\n\n\n\t\t\t私聊记录:\n\n");
    FILE *fp;
    int ret;
    DATA tmp;
    char name[20];
    printf("请依次输入您想要查询和谁的聊天记录:\n");
    scanf("%s", name);
    char filename[64];


    sprintf(filename, "%s发给%s_pchat", my_nickname, name);
    printf("发送记录:\n\n");
    fp = fopen(filename, "r");
    if(fp == NULL)
    {
        printf("\t您没有发送记录!\n\n");
    }
    else
    {
        while(1)
        {
            bzero(&tmp, sizeof(tmp));
            ret = fread(&tmp, 1, sizeof(DATA), fp);
            if(0 == ret)
            {
                break;
            }
            printf("\t时间:%d-%d-%d-%d-%d-%d\n", tmp.time_year, tmp.time_mon, tmp.time_mday, tmp.time_hour, tmp.time_min, tmp.time_sec);
            printf("\t%s对%s说:\t%s\n\n", tmp.fromname, tmp.nickname, tmp.message);
        }
        fclose(fp);
    }


    sprintf(filename, "%s发给%s_pchat", name, my_nickname);
    printf("接受记录:\n\n");
    fp = fopen(filename, "r");
    if(fp == NULL)
    {
        printf("\t您没有接受记录!\n\n");
    }
    else
    {
        while(1)
        {
            bzero(&tmp, sizeof(tmp));
            ret = fread(&tmp, 1, sizeof(DATA), fp);
            if(0 == ret)
            {
                break;
            }
            printf("\t时间:%d-%d-%d-%d-%d-%d\n", tmp.time_year, tmp.time_mon, tmp.time_mday, tmp.time_hour, tmp.time_min, tmp.time_sec);
            printf("\t%s对%s说:\t%s\n\n", tmp.fromname, tmp.nickname, tmp.message);
        }
        fclose(fp);
    }
}

void read_gchat()
{
    system("clear");
    printf("\n\n\n\t\t\t群聊记录\n\n");
    DATA tmp;
    int ret;
    FILE *fp;
    char filename[64];
    sprintf(filename, "%s发给all_gchat", my_nickname);
    fp = fopen(filename, "r");
    if(NULL == fp)
    {
        printf("\t您没有发送群消息记录!\n\n");
    }
    else
    {
        while(1)
        {
            bzero(&tmp, sizeof(tmp));
            ret = fread(&tmp, 1, sizeof(DATA), fp);
            if(0 == ret)
            {
                break;
            }
            printf("\t时间:%d-%d-%d-%d-%d-%d\n", tmp.time_year, tmp.time_mon, tmp.time_mday, tmp.time_hour, tmp.time_min, tmp.time_sec);
            printf("\t%s对%s说:\t%s\n\n", tmp.fromname, tmp.nickname, tmp.message);   
        }
        fclose(fp);
    }

    sprintf(filename, "all发给%s_gchat", my_nickname);
    fp = fopen(filename, "r");
    if(NULL == fp)
    {
        printf("\t您没有接受群消息的记录\n\n");
    }
    else
    {   
        while(1)
        {
            bzero(&tmp, sizeof(tmp));
            ret = fread(&tmp, 1, sizeof(tmp), fp);
            if(0 == ret)
            {
                break;
            }
            printf("\t时间:%d-%d-%d-%d-%d-%d\n", tmp.time_year, tmp.time_mon, tmp.time_mday, tmp.time_hour, tmp.time_min, tmp.time_sec);
            printf("\t%s对%s说:\t%s\n\n", tmp.toname, tmp.nickname, tmp.message); 
        }
        fclose(fp);
    }
}

/*********************************************会员****************************************************/

void client_member(int sockfd)
{
    int choice, ret;
    printf("确定支付30rmb,充值会员吗?(是1,否0)\n\n");
    scanf("%d", &choice);
    if(choice == 1)
    {
        DATA SendBuf;
        SendBuf.flag = MEMBER;
        ret = send(sockfd, &SendBuf, sizeof(SendBuf), 0);
        if(-1 == ret)
        {
            perror("send");
            exit(1);
        }
    }
    return;
}

/********************************************禁言***************************************************/

void client_close(int sockfd)
{
    int ret;
    DATA SendBuf;
    printf("请输入您禁言对象的昵称:\n");
    scanf("%s", SendBuf.nickname);
    SendBuf.flag = CLOSE;
    ret = send(sockfd, &SendBuf, sizeof(SendBuf), 0);
    if(-1 == ret)
    {
        perror("send");
        exit(1);
    }
}


/********************************************踢人******************************************/

void client_down(int sockfd)
{
    system("clear");
    int ret;
    DATA SendBuf;
    printf("\n\n\n\n\n\t请输入您想踢人的昵称:\n");
    scanf("%s", SendBuf.nickname);
    SendBuf.flag = DOWN;
    ret = send(sockfd, &SendBuf, sizeof(SendBuf), 0);
    if(-1 == ret)
    {
        perror("send");
        exit(1);
    }
}


/*************************************************delay****************************************/

void delay()
{
    int i, j;
    for(i = 0; i < 1000; i++)
    {
        for(j = 0; j < 800; j++)
        {

        }
    }
}

/*******************************************文件传输*****************************************/

void client_transform(int sockfd)
{
    char tmpbuf[4096];
    system("clear");
    int ret;
    FILE *fp;
    char filename[64];
    TRAN SendBuf;
    bzero(&SendBuf, sizeof(SendBuf));
    printf("\n\n\n\n\n\t请输入您想要传送的文件名:\n");
    scanf("%s", filename);
    printf("\n\t请输入您想要发送文件的对象:\n");
    scanf("%s", SendBuf.toname);
    SendBuf.endflag = YES;
    strcpy(SendBuf.fromname, my_nickname);
    strcpy(SendBuf.filename, filename);

    fp = fopen(filename, "r");
    if(NULL == fp)
    {
        printf("\n\n\t该文件不存在!\n");
        sleep(1);
        return;
    }
	
	DATA Send_file_buf;
	Send_file_buf.flag=GIVE;
	ret = send(sockfd, &Send_file_buf, sizeof(Send_file_buf), 0);
	
	if(-1 == ret)
	{
		perror("send");
		exit(1);
	}
	
    while(1)
    {
        bzero(&tmpbuf, sizeof(tmpbuf));
        ret = fread(&tmpbuf, 1, sizeof(tmpbuf), fp);
        if(0 == ret)
        {
            bzero(&SendBuf.text, sizeof(SendBuf.text));
            SendBuf.endflag = NO;

            ret = send(sockfd, &SendBuf, sizeof(SendBuf), 0);
            if(-1 == ret)
            {
                perror("send");
                exit(1);
            }
            break;
        }

        strcpy(SendBuf.text, tmpbuf);
        ret = send(sockfd, &SendBuf, sizeof(SendBuf), 0);
        if(-1 == ret)
        {
            perror("send");
            exit(1);
        }
		sleep(1);
    }
}

/***********************************************接受文件**************************************/

void client_receive()
{
	int ret;
	TRAN RecvBuf;
	bzero(&RecvBuf, sizeof(RecvBuf));
	recv(cfd,&RecvBuf,sizeof(RecvBuf),0);
	
    FILE *fp;
    char filename[60];
    bzero(&filename, sizeof(filename));
    strcpy(filename, RecvBuf.filename);
	fp = fopen(RecvBuf.filename, "a+b");
	if(NULL == fp)
	{
		perror("fopen");
		exit(1);
	}
	while(RecvBuf.endflag==YES)
	{
		fwrite(&RecvBuf.text, 1, sizeof(RecvBuf.text), fp);
		bzero(&RecvBuf, sizeof(RecvBuf));
		ret = recv(cfd,&RecvBuf,sizeof(RecvBuf),0);
		if(ret < 0)
		{
			perror("recv");
			exit(1);
		}
	}
	fclose(fp);
	printf("\n\n\t您接受到了来自%s用户的新文件!\n", RecvBuf.fromname);
	sleep(1);
	printf("\n\t接受完成!\n");
	sleep(1);
}