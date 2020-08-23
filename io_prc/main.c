#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>

int main()
{
//	int fd = open("myfile",O_RDONLY);
//	int count = 5;
//	char buf[1024];
//	const char *msg = "hello kenerl\n";
//	if(fd < 0)
//	{
//		printf("open file error...\n");
//		exit(1);
//	}
//	while(count--)
//	{
//		size_t s = read(fd,buf,strlen(msg));
//		if(s > 0)
//		{
//			buf[s] = 0;
//			printf("this is read result...\n");
//			printf("%s",buf);
//		}
//		else
//		{
//			break;
//		}
//	}
//	close(fd);
	int fd = open("myfile",O_CREAT|O_RDWR,0644);
	if(fd < 0)
	{
		printf("open file error...\n");
		exit(1);
	}
	dup2(fd,1);
	for(;;)
	{
		char buf[1024] = {0};
		ssize_t s = read(0,buf,sizeof(buf) - 1);
		if(s > 0)
		{
			printf("%s",buf);
			fflush(stdout);
		//	write(1,buf,strlen(buf));
		//	write(2,buf,strlen(buf));
		}
		else
		{
			break;
		}
	}
	return 0;
}
