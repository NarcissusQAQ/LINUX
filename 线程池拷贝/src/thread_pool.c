#include "thread_pool.h"

void handler(void*arg)
{
	pthread_mutex_unlock((pthread_mutex_t*)arg);//解锁
}

void*func(void*arg)//任务执行函数
{
	struct thread_pool*pool = (struct thread_pool*)arg;
	struct task*p = NULL;
	while(1)
	{
		//访问任务队列之前，加锁，为了防止取消后死锁，注册一个线程处理函数
		pthread_cleanup_push(handler,&pool->mutex);
		pthread_mutex_lock(&pool->mutex);
		
		//若当前没有任务，且线程池未被关闭，则进入条件变量等待队列休眠
		if(pool->head->num == 0 && !pool->shutdown)
		{
			pthread_cond_wait(&pool->cond,&pool->mutex);
		}
		
		//若当前没有任务，且线程池被关闭，则立即释放互斥锁并退出
		if(pool->head->num == 0 && pool->shutdown)
		{
			pthread_mutex_unlock(&pool->mutex);
			pthread_exit(NULL);
		}
		
		//若当前有任务，则执行任务队列上的任务
		p = pool->head->first;
		pool->head->first = p->next;
		p->next = NULL;
		
		pool->head->num--;
		//释放互斥锁，并出栈处理函数（不执行）
		pthread_mutex_unlock(&pool->mutex);
		pthread_cleanup_pop(0);
		
		//执行任务，并且在此期间禁止相应取消请求
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE,NULL);
		(p->do_task)(p->arg);//执行任务
	
		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
		
		free(p);
		
		
	}
	
}


/*
	init_pool:初始化一个线程池
	@pool ：指向要初始化的线程池
	@thread_num :线程池中的活跃线程数
*/

bool init_pool(struct thread_pool*pool,int thread_num)
{
	pool->head = malloc(sizeof(struct head));
	pool->head->first = pool->head->last = NULL;
	pool->head->num = 0;
	
	pthread_mutex_init(&pool->mutex,NULL);
	pthread_cond_init(&pool->cond,NULL);
	
	pool->shutdown = false;//不销毁
	
	pool->now_threads  = thread_num;
	pool->max_tasks = MAX_TASK;
	
	int i;
	for(i = 0;i < pool->now_threads;i++)
	{
		if(pthread_create(&pool->tid[i],NULL,func,(void*)pool) != 0)
		{
			printf("i = %d\n",i);
			return false;
		}
	}
	
	return true;
}

/*
	add_task ：添加任务,往线程池中的任务队列上添加任务
	@pool:线程池，往哪个线程池中投放任务
	@task:你的任务执行函数
	arg:你的任务执行函数的参数
*/
bool add_task(struct thread_pool*pool,void*(*task)(void*),void*arg)
{
	//创建新的任务节点
	struct task*new_task = malloc(sizeof(*new_task));
	new_task->do_task = task;
	new_task->arg = arg;
	new_task->next = NULL;
	
	//访问任务队列之前，加锁
	pthread_mutex_lock(&pool->mutex);
	
	if(pool->head->num >= MAX_TASK)//如果等待任务数量达到上限
	{
		printf("full\n");
		pthread_mutex_unlock(&pool->mutex);
		return false;
	}
	
	
	//从无到有
	if(pool->head->first == NULL)//空任务队列
	{
		pool->head->first = new_task;
		pool->head->last = new_task;
	}
	else
	{
		pool->head->last->next = new_task;
		pool->head->last = new_task;
	}
	
	pool->head->num++;
	
	//解锁
	pthread_mutex_unlock(&pool->mutex);
	
	
	pthread_cond_signal(&pool->cond);//唤醒条件变量
	return true;
	
}

/*
	destroy_pool:销毁线程池
	@pool:要销毁的线程池
*/

bool destroy_pool(struct thread_pool*pool)
{
	pool->shutdown = true;
	pthread_cond_broadcast(&pool->cond);
	
	int i;
	for(i = 0;i< pool->now_threads;i++)
	{
		pthread_join(pool->tid[i],NULL);
		
	}
	
	pthread_mutex_destroy(&pool->mutex);
	pthread_cond_destroy(&pool->cond);
	
	
	free(pool->head);
	
	//假设线程池是动态分配的空间
	//free(pool);
	
	return true;
}