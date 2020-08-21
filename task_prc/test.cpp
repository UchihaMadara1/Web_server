#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/wait.h>
#include<errno.h>

using namespace std;

int main()
{
	pid_t pid = fork();
	if(pid < 0)
	{
		printf("%s fork error...\n",__FUNCTION__);
		exit(1);
	}
	else if(pid == 0)
	{
		//子进程
		cout<<"child is run,pid is "<<getpid()<<endl;
		sleep(5);
		exit(3);
	}
	else
	{
		//父进程
		int status = 0;
		pid_t ret = 0;
		do
		{
			ret = waitpid(-1,&status,WNOHANG);
			if(ret == 0)
				cout<<"this child is running..."<<endl;
			sleep(1);
		}while(ret == 0);
		if(WIFEXITED(status)&&ret == pid)
		{//正常退出
			cout<<"child exit code :"<<WEXITSTATUS(status)<<endl;
		}
		else if(ret > 0)
		{
			//异常退出
			cerr<<"wait child failed,return";
			return 1;
		}
	}

	return 0;
}
