/*************************************************************************
    > File Name: test.c
    > Author: wht
    > Mail: vi_pzp@163.com 
    > Created Time: 2016年06月03日 星期五 10时38分25秒
 ************************************************************************/

#include<stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include "log.h"

void *testThreadOne(void *p)
{
	PRINTLOG(LOG_TRACE, ">>%s %s %d %s\n",__TIME__,  __FILE__, __LINE__, __FUNCTION__);
	int i = 20000;
	while (i--)
	{
		PRINTLOG(LOG_INFO, " %s %s %d %s %s\n",__TIME__,  __FILE__, __LINE__, __FUNCTION__, "there is thread one");
	}
	PRINTLOG(LOG_TRACE, "<<%s %s %d %s\n",__TIME__,  __FILE__, __LINE__, __FUNCTION__);
}

void *testThreadTwo(void *p)
{
	PRINTLOG(LOG_TRACE, ">>%s %s %d %s\n",__TIME__,  __FILE__, __LINE__, __FUNCTION__);
	int i = 20000;
	while (i--)
	{
		PRINTLOG(LOG_INFO, " %s %s %d %s %s\n",__TIME__,  __FILE__, __LINE__, __FUNCTION__, "there is thread two");
	}
	PRINTLOG(LOG_TRACE, "<<%s %s %d %s\n",__TIME__,  __FILE__, __LINE__, __FUNCTION__);
}



int main(int argc, char **argv)
{
	int ret = 0;
	LEVEL level = LOG_TRACE;
	OUTWAY outway = OUT_FILE;

	//测试未初始化就使用PRINTFLOG的情况
	ret = PRINTLOG(LOG_TRACE, "未初始化\n");
	printf("未初始化 返回值=%d\n", ret);

	//正常的使用方法
	InitLog(level, outway);
	ret = PRINTLOG(LOG_TRACE, ">>%s %s %d %s\n",__TIME__,  __FILE__, __LINE__, __FUNCTION__);
	printf("输出级别和初始化级别相同 返回值=%d\n", ret);
	PRINTLOG(LOG_TRACE, "<<%s %s %d %s\n",__TIME__,  __FILE__, __LINE__, __FUNCTION__);
	FreeLog();

	sleep(1);

	//输出级别不同于初始化级别的测试
	level = LOG_INFO;
	InitLog(level, outway);
	ret = PRINTLOG(LOG_TRACE, " %s %s %d %s\n",__TIME__,  __FILE__, __LINE__, __FUNCTION__);
	printf("输出级别低于初始化级别 返回值=%d\n", ret);
	PRINTLOG(LOG_WARNING, " %s %s %d %s\n",__TIME__,  __FILE__, __LINE__, __FUNCTION__);
	FreeLog();

	sleep(1);

	//测试多线程
	level = LOG_TRACE;
	InitLog(level, outway);
	
	pthread_t t1, t2;
	pthread_create(&t1, NULL, testThreadOne, NULL);
	pthread_create(&t2, NULL, testThreadTwo, NULL);

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
	
	FreeLog();
	return 0;
}
