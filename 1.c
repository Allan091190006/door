#include<stdio.h>
#include<sys/time.h>
int main(int argc, const char *argv[])
{
	struct timeval tv;
	long r=0;
	gettimeofday(&tv,NULL);
	r=tv.tv_sec*1000+tv.tv_usec;
	printf("%ld\n",r);
	return 0;
}
