#include "chat.h"

extern char sql[200];
extern sqlite3 *ppdb;
extern STATUS sta;

int count;  
char nname[20];                 //登录时保存昵称,修改密码保存昵称
char psnum[20];                 //忘记密码保存密码
int aim_a;                      //记录目标fd
int member_flag;                //会员标志

/****************************************************注册***********************************************/

void server_sign(int aim, DATA RecvBuf)
{
    int ret;
    memset(&sql,0, sizeof(sql));
    sprintf(sql, "insert into userdata (id, pswd, nickname, sex, phone, member) values (%d, '%s', '%s', '%c', '%s', 0);", RecvBuf.id, RecvBuf.pswd, RecvBuf.nickname, RecvBuf.sex, RecvBuf.phone);

    ret = sqlite3_exec(ppdb, sql, NULL, NULL, NULL);
    if(ret != SQLITE_OK)
    {
        DATA SendBuf;
        memset(&SendBuf, 0, sizeof(SendBuf));
        SendBuf.flag = FAILURE;
        ret = send(aim, &SendBuf, sizeof(SendBuf), 0);
        if(-1 == ret)
        {
            perror("send");
            exit(1);
        }
    }
    else
    {
        DATA SendBuf;
        memset(&SendBuf, 0, sizeof(SendBuf));
        SendBuf.flag = SUCCESS;
        ret = send(aim, &SendBuf, sizeof(SendBuf), 0);
        if(-1 == ret)
        {
            perror("send");
            exit(1);
        }
    }
}

/****************************************************登录**********************************************/

void server_logo(int aim, DATA RecvBuf)
{
    member_flag = 0;
    DATA SendBuf;
    memset(&SendBuf, 0, sizeof(SendBuf));
    count = 0;
    int ret;
    memset(&sql, 0, sizeof(sql));
    sprintf(sql, "select * from userdata;");
    ret = sqlite3_exec(ppdb, sql, compare, (void *)&RecvBuf, NULL);
    if(ret != SQLITE_OK)
    {
        perror("sqlite3_exec");
        exit(1);
    }

    if(count == 1)
    {
        STATUS tmp = sta;
        STATUS p = sta->next;
        while(p)
        {
            if(p->id == RecvBuf.id)
            {
                SendBuf.flag = FAILURE;                                 //用户已在线
                ret = send(aim, &SendBuf, sizeof(SendBuf), 0);
                if(-1 == ret)
                {
                    perror("send");
                    exit(1);
                }

                return;
            }
            tmp = p;
            p = p->next;
        }

        SendBuf.flag = SUCCESS;                                         //登录成功
        ret = send(aim, &SendBuf, sizeof(SendBuf), 0);
        if(-1 == ret)
        {
            perror("send");
            exit(1);
        }

        STATUS n = (STATUS)malloc(sizeof(Status));
        if(NULL == n)
        {
            perror("malloc");
            exit(1);
        }
        n->id = RecvBuf.id;
        n->fd = aim;
        n->member_flag = member_flag;
        strcpy(n->nickname, nname);
        tmp->next = n;
        n->next = NULL;

        return;
    }
    else if(count == 2)
    {
        SendBuf.flag = ERROR;                                           //密码错误
        ret = send(aim, &SendBuf, sizeof(SendBuf), 0);
        if(-1 == ret)
        {
            perror("send");
            exit(1);
        }

        return;
    }
    else
    {
        SendBuf.flag = FAULT;                                           //用户未注册
        ret = send(aim, &SendBuf, sizeof(SendBuf), 0);
        if(-1 == ret)
        {
            perror("send");
            exit(1);
        }

        return;
    }
}

int compare(void *para, int columnCount, char **columnValue, char **columnName)
{
    int ret, j;
    if(atoi(columnValue[0]) == ((*(DATA *)para).id))
    {
        if(!strcmp(columnValue[1], ((*(DATA *)para).pswd)))
        {
            memset(&nname, 0, sizeof(nname));
            strcpy(nname, columnValue[2]);
            member_flag = ((*(((int **)columnValue)[5])) - 48);
            count = 1;                              //密码帐号正确,但需要印证是否在线
            return 0;
        }
        else
        {
            count = 2;                              //密码错误
            return 0;
        }
    }
    return 0;
}

/**************************************************忘记密码*************************************************/

void server_forget(int aim, DATA RecvBuf)
{
    count = 0;
    int ret;
    memset(&sql, 0, sizeof(sql));
    sprintf(sql, "select * from userdata;");
    ret = sqlite3_exec(ppdb, sql, getpswd, (void *)&RecvBuf, NULL);
    if(ret != SQLITE_OK)
    {
        perror("sqlite3_exec");
        exit(1);
    }

    DATA SendBuf;
    memset(&SendBuf, 0, sizeof(SendBuf));
    if(1 == count)
    {
        SendBuf.flag = FAILURE;                             //用户已上线
    }
    else if(2 == count)
    {
        SendBuf.flag = SUCCESS;                             //查找成功
        strcpy(SendBuf.pswd, psnum);
    }
    else
    {
        SendBuf.flag = ERROR;                               //用户未注册
    }

    ret = send(aim, &SendBuf, sizeof(SendBuf), 0);
    if(-1 == ret)
    {
        perror("send");
        exit(1);
    }
    return;
}

int getpswd(void *para, int columnCount, char **columnValue, char **columnName)
{
    printf("in\n");
    int ret, j;
    if((atoi(columnValue[0]) == (*(DATA *)para).id) && (!strcmp(columnValue[2], ((*(DATA *)para).nickname))) && (columnValue[3][0] == (*(DATA *)para).sex) && (!strcmp(columnValue[4], ((*(DATA *)para).phone))))
    {
        STATUS p;
        p = sta->next;
        while(p)
        {
            if(p->id == (*(DATA *)para).id)
            {
                count = 1;
                return 0;
            }
            p = p->next;
        }

        memset(&psnum, 0, sizeof(psnum));
        strcpy(psnum, columnValue[1]);
        count = 2;
        return 0;
    }
    return 0;
}

/***********************************************显示在线人昵称************************************************/

void server_show(int aim)
{
    int ret, num = 0;

    STATUS p = sta->next;
    while(p)
    {
        num++;
        p = p->next;
    }

    DATA SendBuf;
    memset(&SendBuf, 0, sizeof(SendBuf));
    SendBuf.flag = SHOWNUM;
    SendBuf.num = num;
    ret = send(aim, &SendBuf, sizeof(SendBuf), 0);
    if(-1 == ret)
    {
        perror("send");
        exit(1);
    }

    p = sta->next;
    while(p)
    {
        if(p->next == NULL)
        {
            memset(&SendBuf, 0, sizeof(SendBuf));
            SendBuf.flag = SHOW;
            SendBuf.endflag = YES;
            strcpy(SendBuf.nickname, p->nickname);
            ret = send(aim, &SendBuf, sizeof(SendBuf), 0);
            if(-1 == ret)
            {
                perror("send");
                exit(1);    
            }

        }
        else
        {
            memset(&SendBuf, 0, sizeof(SendBuf));
            SendBuf.flag = SHOW;
            SendBuf.endflag = NO;
            strcpy(SendBuf.nickname, p->nickname);
            ret = send(aim, &SendBuf, sizeof(SendBuf), 0);
            if(-1 == ret)
            {
                perror("send");
                exit(1);    
            }
        }

        p = p->next;
    }

    DATA RecvBuf;
    p = sta->next;
    while(p)
    {
        if(aim == p->fd)
        {
            strcpy(RecvBuf.fromname, p->nickname);
            break;
        }
        p = p->next;
    }

    RecvBuf.flag = GETNICK;
    ret = send(aim, &RecvBuf, sizeof(RecvBuf), 0);
    if(-1 == ret)
    {
        perror("send");
        exit(1);
    }
    return;
}

/***************************************************私聊*****************************************************/

void server_pchat(int aim, DATA RecvBuf)
{
    int ret;
    aim_a = 0;
    char fromname[20] = {0};
    STATUS p = sta->next;
    while(p)
    {
        if(p->fd == aim && p->nospeak == 1)
        {
            DATA SendBuf;
            memset(&SendBuf, 0, sizeof(SendBuf));
            SendBuf.flag = PCHAT;
            SendBuf.endflag = FAILURE;
            ret = send(aim, &SendBuf, sizeof(SendBuf), 0);
            if(-1 == ret)
            {
                perror("send");
                exit(1);
            }
            return;
        }
        p = p->next;
    }
    p = sta->next;
    while(p)
    {
        if(!strcmp(p->nickname, RecvBuf.nickname))
        {
            aim_a = p->fd;
            break;
        }
        p = p->next;
    }
    if(0 == aim_a)
    {
        RecvBuf.flag = PCHAT;
        RecvBuf.endflag = NO;
        ret = send(aim, &RecvBuf, sizeof(RecvBuf), 0);
        if(-1 == ret)
        {
            perror("send");
            exit(1);
        }
        return;
    }
    else if(aim == aim_a)
    {
        RecvBuf.flag = PCHAT;
        RecvBuf.endflag = FAULT;
        ret = send(aim, &RecvBuf, sizeof(RecvBuf), 0);
        if(-1 == ret)
        {
            perror("send");
            exit(1);
        }
        return;
    }
    else
    {
        p = sta->next;
        while(p)
        {
            if(aim == p->fd)
            {
                strcpy(RecvBuf.fromname, p->nickname);
                RecvBuf.flag = PCHAT;
                RecvBuf.endflag = YES;
                printf("name:%s, endflag:%d,  message:%s\n", RecvBuf.nickname, RecvBuf.endflag, RecvBuf.message);
                ret = send(aim_a, &RecvBuf, sizeof(RecvBuf), 0);
                if(-1 == ret)
                {
                    perror("send");
                    exit(1);
                }
                return;
            }
            p = p->next;
        }
        return;
    }
}

/****************************************************群聊****************************************************/

void server_gchat(int aim, DATA RecvBuf)
{
    int ret;
    char fromname[20] = {0};
    aim_a = 0;
    STATUS p = sta->next;
    while(p)
    {
        if(p->fd == aim && p->nospeak == 1)
        {
            DATA SendBuf;
            memset(&SendBuf, 0, sizeof(SendBuf));
            SendBuf.flag = GCHAT;
            SendBuf.endflag = FAILURE;
            ret = send(aim, &SendBuf, sizeof(SendBuf), 0);
            if(-1 == ret)
            {
                perror("send");
                exit(1);
            }
            return;
        }
        p = p->next;
    }
    p = sta->next;
    while(p)
    {
        if(p->fd == aim)
        {
            strcpy(fromname, p->nickname);
        }
        p = p->next;
    }

    p = sta->next;
    while(p)
    {
        if(aim != p->fd)
        {
            aim_a = p->fd;
            RecvBuf.flag = GCHAT;
            strcpy(RecvBuf.fromname, fromname);
            ret = send(aim_a, &RecvBuf, sizeof(RecvBuf), 0);
            if(-1 == ret)
            {
                perror("send");
                exit(1);
            }
        }
        p = p->next;
    }
    if(aim_a == 0)
    {
        RecvBuf.flag = ERROR;
        ret = send(aim, &RecvBuf, sizeof(RecvBuf), 0);
        if(-1 == ret)
        {
            perror("send");
            exit(1);
        }
    }

    return;
}

/*************************************************注销**********************************************/

void server_back(int aim)
{
    STATUS p = sta->next;
    STATUS tmp = sta;

    while(p)
    {
        if(p->fd == aim)
        {
            if(p->next == NULL)
            {
                tmp->next = NULL;
                free(p);
                return;
            }
            else
            {
                tmp->next = p->next;
                free(p);
                p->next = NULL;
                return;
            }
        }
        tmp = p;
        p = p->next;
    }
}

/**************************************************修改密码**********************************************/

void server_change(int aim, DATA RecvBuf)
{
    int ret;
    count = 0;
    memset(&nname, 0, sizeof(nname));
    STATUS p = sta->next;
    while(p)
    {
        if(p->fd == aim)
        {
            memset(&nname, 0, sizeof(nname));
            strcpy(nname, p->nickname);
			//break;
        }
        p= p->next;
    }

    memset(&sql, 0, sizeof(sql));
    sprintf(sql, "select * from userdata;");
    ret = sqlite3_exec(ppdb, sql, find, (void *)&RecvBuf, NULL);
    if(ret != SQLITE_OK)
    {
        perror("sqlite3_exec");
        exit(1);
    }

    DATA SendBuf;
    memset(&SendBuf, 0, sizeof(SendBuf));
    SendBuf.flag = CHANGE;

    if(0 == count)
    {
        SendBuf.endflag = FAILURE;                          //旧密码错误
    }
    else if(1 == count)
    {
        memset(&sql, 0, sizeof(sql));
        sprintf(sql, "update userdata set pswd = '%s' where nickname = '%s';", RecvBuf.message, nname);
        ret = sqlite3_exec(ppdb, sql, NULL, NULL, NULL);
        if(ret != SQLITE_OK)
        {
            perror("sqlite3_exec");
            exit(1);
        }

        SendBuf.endflag = SUCCESS;                          //修改密码成功
    }
    ret = send(aim, &SendBuf, sizeof(SendBuf), 0);
    if(-1 == ret)
    {
        perror("send");
        exit(1);
    }

    return;
}

int find(void *para, int columnCount, char **columnValue, char **columnName)
{
    int ret;
    if((!strcmp(columnValue[1], (*(DATA *)para).pswd)) && ((!strcmp(columnValue[2], nname))))
    {
        count = 1;
        return 0;
    }

    return 0;
}

/*******************************************会员************************************/

void server_member(int aim, DATA RecvBuf)
{

    int ret;
    count = 0;

    memset(&nname, 0, sizeof(nname));
    STATUS p = sta->next;
    while(p)
    {
        if(p->fd == aim)
        {
            if(p->member_flag == 0)
            {
                p->member_flag = 1;
                memset(&nname, 0, sizeof(nname));
                strcpy(nname, p->nickname);
                count = 1;
                break;
            }
            else
            {
                count = 2;
                break;
            }
        }
        p= p->next;
    }

    printf("name:%s\n", nname);

    DATA SendBuf;
    bzero(&SendBuf, sizeof(SendBuf));
    if(1 == count)
    {
        memset(&sql, 0, sizeof(sql));
        sprintf(sql, "update userdata set member = 1 where nickname = '%s';", nname);
        ret = sqlite3_exec(ppdb, sql, NULL, NULL, NULL);
        if(ret != SQLITE_OK)
        {
            perror("sqlite3_exec");
            exit(1);
        }

        SendBuf.flag = MEMBER;
        SendBuf.endflag = SUCCESS;
        ret = send(aim, &SendBuf, sizeof(SendBuf), 0);
        if(-1 == ret)
        {
            perror("send");
            exit(1);
        }
    }
    else if(2 == count)
    {
        SendBuf.flag = MEMBER;
        SendBuf.endflag = YES;
        ret = send(aim, &SendBuf, sizeof(SendBuf), 0);
        if(-1 == ret)
        {
            perror("send");
            exit(1);
        }
    }
    else if(0 == count)
    {
        SendBuf.flag = MEMBER;
        SendBuf.endflag = FAILURE;
        ret = send(aim, &SendBuf, sizeof(SendBuf), 0);
        if(-1 == ret)
        {
            perror("send");
            exit(1);
        }
    }
    return;
}


/******************************************禁言*****************************************/

void server_close(int aim, DATA RecvBuf)
{
    aim_a = 0;
    int ret;
    count = 0;
    STATUS p = sta->next;
    while(p)
    {
        if(p->fd == aim && p->member_flag == 0)
        {
            DATA SendBuf;
            memset(&SendBuf, 0, sizeof(SendBuf));
            SendBuf.flag = CLOSE;
            SendBuf.endflag = ERROR;
            ret = send(aim, &SendBuf, sizeof(SendBuf), 0);
            if(-1 == ret)
            {
                perror("send");
                exit(1);
            }
            return;
        }
        p = p->next;
    }

    p = sta->next;
    while(p)
    {
        if(!strcmp(p->nickname, RecvBuf.nickname))
        {
            if(p->fd == aim)
            {
                count = 3;
                break;
            }
            else
            {
                if(p->member_flag != 1)
                {
                    p->nospeak = 1;
                    aim_a = p->fd;
                    count = 1;
                    break;
                }
                else
                {
                    count = 2;
                    break;
                }
            }       
        }
        p= p->next;
    }

    DATA SendBuf;
    memset(&SendBuf, 0, sizeof(SendBuf));
    if(0 == count)
    {
        SendBuf.flag = CLOSE;
        SendBuf.endflag= FAILURE;
    }
    else if(1 == count)
    {
        SendBuf.flag = CLOSE;
        SendBuf.endflag  = SUCCESS;

        DATA ToBuf;
        ToBuf.flag = CLOSE;
        ToBuf.endflag = YES;
        ret = send(aim_a, &ToBuf, sizeof(ToBuf), 0);
        if(-1 == ret)
        {
            perror("send");
            exit(1);
        }
    }
    else if(2 == count)
    {
        SendBuf.flag = CLOSE;
        SendBuf.endflag = FAULT;
    }
    else if(3 == count)
    {
        SendBuf.flag = CLOSE;
        SendBuf.endflag = NO;
    }
    ret = send(aim, &SendBuf, sizeof(SendBuf), 0);
    if(ret == -1)
    {
        perror("send");
        exit(1);
    }
}

/**************************************************踢人***************************************/

void server_down(int aim, DATA RecvBuf)
{
    count = 0;
    aim_a = 0;
    DATA SendBuf;
    bzero(&SendBuf, sizeof(SendBuf));
    int ret;
    STATUS p = sta->next;
    while(p)
    {
        if(p->fd == aim)
        {
            if(p->member_flag != 1)
            {
                count = 1;
            }
            break;
        }
        p = p->next;
    }

    p = sta->next;
    while(p)
    {
        if(!strcmp(p->nickname, RecvBuf.nickname))
        {
            if(p->fd == aim)
            {
                count = 3;
                break;
            }
            else
            {
                if(p->member_flag == 1)
                {
                    count = 2;
                    break;
                }
                else
                {
                    aim_a = p->fd;
                    count = 4;
                    break;
                }

            }
        }
        p = p->next;
    }

    if(1 == count)
    {
        SendBuf.flag = DOWN;
        SendBuf.endflag = FAILURE;
    }
    else if(2 == count)
    {
        SendBuf.flag = DOWN;
        SendBuf.endflag = ERROR;
    }
    else if(3 == count)
    {
        SendBuf.flag = DOWN;
        SendBuf.endflag = NO;
    }
    else if(4 == count)
    {
        SendBuf.flag = DOWN;
        SendBuf.endflag = SUCCESS;

        DATA ToBuf;
        bzero(&ToBuf, sizeof(ToBuf));
        ToBuf.flag = DOWN;
        ToBuf.endflag = YES;
        ret = send(aim_a, &ToBuf, sizeof(ToBuf), 0);
        if(-1 == ret)
        {
            perror("send");
            exit(1);
        }
    }

    ret = send(aim, &SendBuf, sizeof(SendBuf), 0);
    if(-1 == ret)
    {
        perror("send");
        exit(1);
    }
    return;
}

/*************************************************文件传输****************************************/

void server_give(int aim)
{
	TRAN RecvBuf;
	recv(aim,&RecvBuf,sizeof(RecvBuf),0);
    int ret;
    aim_a = 0;
    STATUS p = sta->next;
    while(p)
    {
        if(!strcmp(RecvBuf.toname, p->nickname))
        {
            aim_a = p->fd;
            break;
        }
        p = p->next;
    }

    DATA SendBuf;
    bzero(&SendBuf, sizeof(SendBuf));
    if(0 == aim_a)
    {
        SendBuf.flag = GIVE;
        SendBuf.endflag = FAILURE;
        ret = send(aim, &SendBuf, sizeof(SendBuf), 0);
        if(-1 == ret)
        {
            perror("send");
            exit(1);
        }

    }
    else if(aim == aim_a)
    {
        SendBuf.flag = GIVE;
        SendBuf.endflag = ERROR;
        ret = send(aim, &SendBuf, sizeof(SendBuf), 0);
        if(-1 == ret)
        {
            perror("send");
            exit(1);
        }
    }
    else
    {
		//发送give yes信号给文件接收者
		bzero(&SendBuf, sizeof(SendBuf));
		SendBuf.flag = GIVE;
        SendBuf.endflag = YES;
        ret = send(aim_a, &SendBuf, sizeof(SendBuf), 0);
        if(-1 == ret)
        {
            perror("send");
            exit(1);
        }
		
		//文件传输直到endflag==NO
		while(RecvBuf.endflag==YES)
		{
			ret = send(aim_a, &RecvBuf, sizeof(RecvBuf), 0);
			if(-1 == ret)
			{
				perror("send");
				exit(1);
			}
			sleep(1);
			
			bzero(&RecvBuf, sizeof(RecvBuf));
			ret = recv(aim,&RecvBuf,sizeof(RecvBuf),0);
			if(ret < 0)
			{
				perror("recv");
				exit(1);
			}
		}
		//发送NO给文件接收者
		ret = send(aim_a, &RecvBuf, sizeof(RecvBuf), 0);
		if(-1 == ret)
		{
			perror("send");
			exit(1);
		}
			
		//发送give ok信号给文件发送者
		bzero(&SendBuf, sizeof(SendBuf));
		SendBuf.flag = GIVE;
        SendBuf.endflag = OK;
		ret = send(aim, &SendBuf, sizeof(SendBuf), 0);
        if(-1 == ret)
        {
            perror("send");
            exit(1);
        }
    }
}