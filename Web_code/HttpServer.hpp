#pragma once

#include<iostream>
#include<pthread.h>
#include"Sock.hpp"
#include"Log.hpp"
#include"Protocol.hpp"
#include"Threadpool.hpp"

#define DefaultPort 8081

using namespace std;

class HttpServer
{
	public:
		HttpServer(int _port = DefaultPort):listen_sock(-1),port(_port),tp(nullptr)
		{}
		~HttpServer()
		{
			if(listen_sock >= 0)
			{
				close(listen_sock);
			}
		}
	public:
		void InitServer()
		{
			listen_sock = Sock::Socket();
			Sock::Bind(listen_sock,port);
			Sock::Listen(listen_sock);
			signal(SIGPIPE,SIG_IGN);

			tp = new ThreadPool();
			tp->InitThreadPool();
		}
		void Start()
		{
			LOG(Normal,"server start success...");
			for(;;)
			{
				int sock = Sock::Accept(listen_sock);
				if(sock >= 0)
				{
					LOG(Normal,"get a new linking...");
					Task t;
					t.SetTask(sock,Entry::HandlerRequest);
					tp->PushTask(t);
				//	pthread_t tid;
				//	int *p = new int(sock);
				//	pthread_create(&tid,nullptr,Entry::HandlerRequest,(void *)p);
				//	pthread_detach(tid);
				}
			}
		}
	private:
		int listen_sock;
		int port;

		ThreadPool *tp;
};
