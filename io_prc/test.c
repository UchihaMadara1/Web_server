#include<stdio.h>
#include<string.h>
#include<stdlib.h>

int main()
{
//	FILE *fp = fopen("myfile","w");
//	const char *str = "hello io\n";
//	int count = 5;
//	if(!fp)
//	{
//		printf("open file error...\n");
//		exit(1);
//	}
//	while(count--)
//	{
//		fwrite(str,strlen(str),1,fp);
//	}
//	fclose(fp);
//	FILE *fp = fopen("myfile","r");
//	const char *msg = "hello io\n";
//	int count = 5;
//	char buf[1024];
//	if(!fp)
//	{
//		printf("open file error...\n");
//		exit(1);
//	}
//	while(1)
//	{
//		size_t s = fwrite(buf,1,strlen(msg),fp);
//		if(s > 0)
//		{
//			buf[s] = 0;
//			printf("%s",buf);
//		}
//		if(feof(fp))
//		{
//			break;
//		}
//	}
//	fclose(fp);
   	const char*msg = "hello fwrite\n";
	fwrite(msg,strlen(msg),1,stdout);
	
	printf("hello printf\n");

	fprintf(stdout,"hello fprintf\n");
	return 0;
}
