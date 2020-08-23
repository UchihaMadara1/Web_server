#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>

int main()
{
	const char *str1 = "hello printf\n";
	const char *str2 = "hello fwrite\n";
	const char *str3 = "hello write\n";
	
//	fork();
//	printf("\n");
	printf("%s",str1);
	fwrite(str2,strlen(str2),1,stdout);
	write(1,str3,strlen(str3));

	fflush(stdout);
	fork();

	return 0;
}
