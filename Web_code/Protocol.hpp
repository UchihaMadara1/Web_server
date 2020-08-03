#pragma once

#include<iostream>
#include<unordered_map>
#include<string>
#include<sstream>
#include<signal.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/wait.h>
#include<sys/socket.h>
#include<sys/sendfile.h>
#include<sys/types.h>
#include<unistd.h>
#include<fcntl.h>
#include"Sock.hpp"
#include"Util.hpp"

#define WWWROOT "./wwwroot"
#define WELCOME_PAGE "index.html"
#define PAGE_404 "./wwwroot/404.html"

class HttpRequest
{
	public:
		HttpRequest():blank("\n"),path(WWWROOT),cgi(false),file_size(0),fd(-1)
		{}
		~HttpRequest()
		{
			if(fd >= 0)
			{
				close(fd);
			}
		}
	public:
		void SetRequestLine(std::string &line)
		{
			request_line = line;
		}
		void SetRequestHeader(std::string &header)
		{
			request_header = header;
		}
		void SetRequestBody(std::string &body)
		{
			request_body = body;
		}
		void SetUrlToPath()
		{
			path += url;
		}
		std::string GetQueryString()
		{
			return query_string;
		}
		std::string GetRequestBody()
		{
			return request_body;
		}
		int GetFileSize()
		{
			std::cout<<"GetFileSize: "<<file_size<<std::endl;
			return file_size;
		}
		std::string GetPath()
		{
			return path;
		}
		void SetPath(std::string _path)
		{
			path = _path;
			struct stat st;
			stat(path.c_str(),&st);
			file_size = st.st_size;
			cgi = false;
			suffix = ".html";
		}
		//GET /index.html HTTP/1.0\n
		void UrlParse()
		{
			std::size_t pos = url.find('?');
			if(pos == std::string::npos)
			{
				path += url;
			}
			else
			{
				path += url.substr(0,pos);
				query_string = url.substr(pos + 1);
				cgi = true;
			}
		}
		void RequestLineParse()
		{
			std::stringstream ss(request_line);
			ss>>method>>url>>version;
			std::cout<<"Method : "<<method<<std::endl;
			std::cout<<"Url : "<<url<<std::endl;
			std::cout<<"Version : "<<version<<std::endl;
			if(url == "/")
			{
				url += WELCOME_PAGE;
			}

		}
		void RequestHeaderParse()
		{
			std::size_t pos = request_header.find('\n');
			int start = 0;
			while(pos != std::string::npos)
			{
				std::string sub = request_header.substr(start,pos - start);
				//sub->kv
				Util::MakeKV(header_kv,sub);//Content-Length: 30
				start = pos + 1;
				pos = request_header.find('\n',pos + 1);
			}
		}
		bool OpenResources()
		{
			bool ret = true;
			fd = open(path.c_str(),O_RDONLY);
			if(fd < 0)
			{
				LOG(Error,"open resources failed!");
				ret = false;
			}
			return ret;
		}
		int GetFd()
		{
			return fd;
		}
		std::string &GetSuffix()
		{
			return suffix;
		}
		bool IsMethodOk()
		{
			//Get gEt GET GEt
			if(strcasecmp(method.c_str(),"GET") == 0||strcasecmp(method.c_str(),"POST") == 0)
			{
				return true;
			}
			return false;
		}
		bool PathIsLegal()
		{
			//path
			std::cout<<"debug path: "<<path<<std::endl;
			bool ret = true;
			struct stat st;
			if(stat(path.c_str(),&st) == 0)
			{
				//exist
				//idr?
				if(S_ISDIR(st.st_mode))
				{
					if(path[path.size() - 1] != '/')
					{
						path += "/";
					}
					path += WELCOME_PAGE;
				}
				else if((st.st_mode & S_IXUSR) || (st.st_mode & S_IXGRP) || (st.st_mode & S_IXOTH))
				{
					//可执行程序  CGI
					cgi = true;
				}
				else
				{
					//ok
				}
				file_size = st.st_size;
				//std::cout<<"GetFileSize: "<<file_size<<std::endl;
				std::size_t pos = path.rfind(".");
				if(pos == std::string::npos)
				{
					suffix = ".html";
				}
				else
				{
					suffix = path.substr(pos);
				}
			}
			else
			{
				//not exist
				//404
				ret = false;
			}
			return ret;
		}
		bool IsGet()
		{
			return strcasecmp(method.c_str(),"GET") == 0;
		}
		bool IsPost()
		{
			return strcasecmp(method.c_str(),"POST") == 0;
		}
		bool IsCGI()
		{
			return cgi;
		}
		void SetCGI()
		{
			cgi = true;
		}
		int GetContentLength()
		{
			//std::unordered_map<std::string,std::string> header_kv;
			auto it = header_kv.find("Content-Length");
			if(it == header_kv.end())
			{
				LOG(Warning,"POST Request,but no request body!");
				return NORMAL_ERROR;
			}
			return Util::StringToInt(it->second);
		}
		void Show()
		{
		std::cout<<"##############start#####################"<<std::endl;
		std::cout<<"debug : " <<request_line<<std::endl;
		std::cout<<"debug : " <<request_header<<std::endl;
		std::cout<<"debug : "<< blank<<std::endl;
		std::cout<<"debug : " <<request_body<<std::endl;
		std::cout<<"debug : "<< method<<std::endl;
		std::cout<<"debug : "<< url<<std::endl;
		std::cout<<"debug : "<< version<<std::endl;


		std::cout<<"debug : "<< path<<std::endl;
		std::cout<<"debug : "<< query_string<<std::endl;
		std::cout<<"#############end #####################"<<std::endl;
		}
	private:
		std::string request_line;
		std::string request_header;
		std::string blank;
		std::string request_body;
	private:
		std::string method;
		std::string url;
		std::string version;

		std::unordered_map<std::string,std::string> header_kv;

		std::string path;
		int fd;
		std::string query_string;

		bool cgi;
		int file_size;
		std::string suffix;
};


class HttpResponse
{
	public:
		HttpResponse():blank("\n")
		{}
		~HttpResponse()
		{}
	public:
		std::string & GetResponseLine()
		{
			return response_line;
		}
		std::string & GetResponseHeader()
		{
			return response_header;
		}
		std::string & GetBlank()
		{
			return blank;
		}
	public:
		void SetResponseLine(std::string &line)
		{
			response_line = line;
		}
		void SetResponseText(std::string &text)
		{
			response_text = text;
		}
		std::string GetResponseText()
		{
			return response_text;
		}
		void AddResponseHeader(std::string &line)
		{
			if(response_header.empty())
			{
				response_header = line;
			}
			else
			{
				response_header += line;
			}
		}
	private:
		std::string response_line;
		std::string response_header;
		std::string blank;
		std::string response_text;
};


class Connect
{
	public:
		Connect(int _sock):sock(_sock)
		{}
		~Connect()
		{
			if(sock >= 0)
				close(sock);
		}
	public:
		bool RecvLine(std::string &line)
		{
			char c = 'B';
			bool result = true;
			while(c != '\n')
			{
				ssize_t s = recv(sock,&c,1,0);
				if(s > 0)
				{
					if(c == '\r')
					{
						recv(sock,&c,1,MSG_PEEK);//窥探，不从队列中移除字符
						if(c == '\n')
						{
							recv(sock,&c,1,0);
						}
						else
						{
							c = '\n';
						}
					}
					line.push_back(c);
				}
				else
				{
					LOG(Warning,"recv request error!");
					result = false;
					break;
				}
			}
			return result;
		}
		bool RecvHttpRequestLine(std::string &request_line)
		{
			return RecvLine(request_line);
		}
		bool RecvHttpRequestHeader(std::string &request_header)
		{
			std::string line = "";
			bool result = true;
			do
			{
				line = "";
				if(RecvLine(line))
				{
					if(line != "\n")
					{
						request_header += line;
					}
				}
				else
				{
					result = false;
					break;
				}
			}while(line != "\n");

			return result;
		}

		//读取http请求的请求行,请求报头,包括空行
		int RecvHttpRequest(HttpRequest *rq)
		{
			std::string request_line;
			std::string request_header;

			if(RecvHttpRequestLine(request_line)&&RecvHttpRequestHeader(request_header))
			{
				rq->SetRequestLine(request_line);	
				rq->SetRequestHeader(request_header);
				return 200;
			}
			return 404;
		}
		//读取http请求的请求正文,正文放进
		void RecvHttpBody(HttpRequest *rq)
		{
			int content_length = rq->GetContentLength();
			if(content_length > 0)
			{
				std::string body;
				char c;
				while(content_length > 0)
				{
					recv(sock,&c,1,0);
					body.push_back(c);
					content_length--;
				}
				rq->SetRequestBody(body);
			}
			rq->SetUrlToPath();
			rq->SetCGI();
		}
		void SendResponse(HttpRequest *rq,HttpResponse *rsp)
		{
			std::string line = rsp->GetResponseLine();
			line += rsp->GetResponseHeader();
			line += rsp->GetBlank();
			send(sock,line.c_str(),line.size(),0);
			
			if(rq->IsCGI())
			{
				std::string text = rsp->GetResponseText();
				send(sock,text.c_str(),text.size(),0);
			}
			else
			{
				sendfile(sock,rq->GetFd(),nullptr,rq->GetFileSize());
			}
			
		}
	private:
		int sock;
};



class Entry
{
	public:
		static void MakeResponse(HttpRequest *rq,HttpResponse *rsp,int code)
		{

			std::string line = Util::GetStatusLine(code);
			if(code == 404)
			{
				rq->SetPath(PAGE_404);
			}
			rsp->SetResponseLine(line);
			//Location

			line = "Content-Type: ";
			line += Util::SuffixToType(rq->GetSuffix());
			line += "\r\n";
			rsp->AddResponseHeader(line);
			line = "Content-Length: ";

			if(rq->IsCGI())
			{
				//TODO
				std::string text = rsp->GetResponseText();
				line += Util::IntToString(text.size());	
				line += "\r\n";
				rsp->AddResponseHeader(line);
			}
			else
			{
				line += Util::IntToString(rq->GetFileSize());
				line += "\r\n";
				rsp->AddResponseHeader(line);
				rq->OpenResources();
			}
		}
		static void ProcessNormal(Connect *conn,HttpRequest *rq,HttpResponse *rsp)
		{
			//没有cgi,没有query_string,不是post
		//	MakeResponse(rq,rsp);
		//	conn->SendResponse(rq,rsp);
		}
		static int ProcessCGI(Connect *conn,HttpRequest *rq,HttpResponse *rsp)
		{
			std::string content_length;
			//站在子进程角度
			int read_pipe[2];
			int write_pipe[2];
			pipe(read_pipe);
			pipe(write_pipe);

			std::string args;
			if(rq->IsGet())
			{
				args = rq->GetQueryString();
			}
			else
			{
				args = rq->GetRequestBody();
			}
			pid_t id = fork();
			if(id < 0)
			{
				LOG(Error,"fork error!");
				return 404;
			}
			else if(id == 0)
			{
				//child
				close(read_pipe[1]);
				close(write_pipe[0]);

				dup2(read_pipe[0],0);
				dup2(write_pipe[1],1);

				content_length = "Content-Length=";
				content_length += Util::IntToString(args.size());

				putenv((char*)content_length.c_str());

				//read_pipe[0]
				std::string path = rq->GetPath();
				//read() or write
				//增加约定，利用重定向技术，来完成文件描述符的约定
				//通过0文件描述符读取，往1文件描述符打印
				//0->read_pipe[0];
				//1->write_pipe[1]
				execl(path.c_str(),path.c_str(),nullptr);

				//rq->path 这个是我们要让子进程执行的程序
				//rq->query_string(GET) or rq->body(POST)

				exit(1);

			}
			else
			{
				//father
				close(read_pipe[0]);
				close(write_pipe[1]);

				//TODO
				for(auto i = 0;i < args.size();++i)
				{
					write(read_pipe[1],&args[i],1);
				}
				char c;
				std::string body;
				while(read(write_pipe[0],&c,1) > 0)
				{
					body.push_back(c);
				}

				rsp->SetResponseText(body);
				//将body设置进response->body
				pid_t id = waitpid(id,nullptr,0);
				if(id < 0)
				{
					LOG(Warning,"waitpid child failed!");
					return 404;
				}
				return 200;
				//makeresponse
				//send
			}
		}
		static void* HandlerRequest(void *arg)
		{
			int sock = *(int *)arg;

			int code = 200;	

			Connect *conn = new Connect(sock);
			HttpRequest *rq = new HttpRequest();
			HttpResponse *rsp = new HttpResponse();

			code = conn->RecvHttpRequest(rq);
			if(code == 200)
			{
				rq->RequestLineParse();
				rq->RequestHeaderParse();
			}
			else
			{
				LOG(Warning,"recv http Request error!");
				goto end;
			}

			//Method is OK?GET  POST
			if(!rq->IsMethodOk())
			{
				code = 404;
				LOG(Error,"request method is not ok!");
				goto end;
				//TODO
			}
			//分析url:path,paramter
			//rq->RequestHeaderParse();
			//域名/文件资源?x=xx&&yyy=YY
			
			if(rq->IsPost())
			{
				//post
				conn->RecvHttpBody(rq);
				//request请求全部读完
			}
			if(rq->IsGet())
			{
				//分析url:path,paramter
				rq->UrlParse();
			}
			

			//分析请求资源是否合法
			//rq->path
			if(!rq->PathIsLegal())
			{
				code = 404;
				LOG(Warning,"Path is not Legal!");
				goto end;
				//TODO

			}
			if(rq->IsCGI())
			{
				LOG(Normal,"exec by cgi!");
				code = ProcessCGI(conn,rq,rsp);
			}
			else
			{
				LOG(Normal,"exec by non cgi");
			}

end:
			MakeResponse(rq,rsp,code);
			conn->SendResponse(rq,rsp);
			//rq->Show();

			//recv request
			//parse request
			//make response
			//send response
			delete conn;
			delete rq;
			delete rsp;
			
			return nullptr;
		}
};
