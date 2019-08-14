#ifndef _THREAD_POOL_H
#define _THREAD_POOL_H

#include <stdbool.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define MAX_THREAD 20
#define MAX_TASK 2000
struct task//任务节点
{
	void*(*do_task)(void*);//函数指针，指向要执行的函数
	void*arg;//一个指针，任务执行函数的参数
	struct task*next;
};

struct head//任务队列的头结点
{
	struct task*first;
	struct task*last;
	int num;
};

struct thread_pool
{
	struct head*head;//指向任务队列的头结点
	pthread_t tid[MAX_THREAD];//保存线程ID的数组
	
	pthread_mutex_t mutex;//互斥锁，用来保护任务队列
	pthread_cond_t cond;//条件变量
	
	bool shutdown;//线程销毁标志
	
	
	int now_threads;//当前线程数
	int max_tasks;//最大任务数
	
};
bool init_pool(struct thread_pool*pool,int thread_num);
bool add_task(struct thread_pool*pool,void*(*task)(void*),void*arg);
bool destroy_pool(struct thread_pool*pool);


#endif