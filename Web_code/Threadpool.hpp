#pragma once

#include<iostream>
#include<queue>
#include<pthread.h>
#include"Threadpool.hpp"

using namespace std;

typedef void *(*Handler_t)(void *args);

class Task
{
	public:
		Task():sock(-1),h(nullptr)
		{}
		~Task()
		{
		}
	public:
		void SetTask(int _sock,Handler_t handler)
		{
			sock = _sock;
			h = handler;
		}
		void Run()
		{
			h((void*)&sock);
		}
	private:
		int sock;
		Handler_t h;
};

class ThreadPool
{
	public:
		ThreadPool(int _sum = 5):sum(_sum)
		{
			pthread_mutex_init(&lock,nullptr);
			pthread_cond_init(&cond,nullptr);
		}
		~ThreadPool()
		{
			pthread_mutex_destroy(&lock);
			pthread_cond_destroy(&cond);
		}
	public:
		void LockQueue()
		{
			pthread_mutex_lock(&lock);
		}
		void UnlockQueue()
		{
			pthread_mutex_unlock(&lock);
		}
		void ThreadWait()
		{
			pthread_cond_wait(&cond,&lock);
		}
		void WakeupThread()
		{
			pthread_cond_signal(&cond);
		}
		void PushTask(Task &t)
		{
			LockQueue();
			q.push(t);
			UnlockQueue();
			WakeupThread();
		}
		bool IsQueueEmpty()
		{
			return q.empty();
		}
		Task PopTask()
		{
			Task t = q.front();
			q.pop();

			return t;
		}
		static void *Routine(void *args)
		{
			ThreadPool *tp = (ThreadPool*)args;

			while(true)
			{
				tp->LockQueue();
				while(tp->IsQueueEmpty())
				{
					tp->ThreadWait();
				}
				//处理任务
				Task t = tp->PopTask();
				tp->UnlockQueue();
				t.Run();
			}
		}
		void InitThreadPool()
		{
			for(auto i = 0;i < sum;++i)
			{
				pthread_t tid;
				pthread_create(&tid,nullptr,Routine,(void*)this);
				pthread_detach(tid);
			}
		}
	private:
		int sum;
		queue<Task> q;//消费场所
		pthread_mutex_t lock;
		pthread_cond_t cond;
};
