#ifdef WIN32
#include <Windows.h>
#elif linux
#include <pthread.h>
#endif
#include <time.h>
#include <stdio.h>
#include "log.h"


typedef struct log
{
	int initialized;
	LEVEL logLevel;
	OUTWAY outWay;
	FILE *fp;
#ifdef WIN32
	RTL_CRITICAL_SECTION cs;
#elif linux
	pthread_mutex_t mutex;
#endif
}LOG;

static LOG s_log = {0};

int InitLog(LEVEL level, OUTWAY outWay)
{
	char logFileName[MAX_PATH] = {0};
	time_t timer;
	struct tm *tblock;
	int ret = NOINIT;

	do
	{
		if (s_log.initialized)
		{
			ret = INITED;
			break;
		}

		timer = time(NULL);
		tblock = localtime(&timer);
		sprintf(logFileName, "./%04d-%02d-%02d-%02d-%02d-%02d.txt",
		tblock->tm_year+1900, tblock->tm_mon+1, tblock->tm_mday,
		tblock->tm_hour, tblock->tm_min, tblock->tm_sec);
		
		s_log.logLevel = level;
		s_log.outWay = outWay;
		
		s_log.fp = fopen(logFileName, "w+");
		if (NULL == s_log.fp)
		{
			ret = OPEN_FAILED;
			break;
		}
#ifdef WIN32	
		InitializeCriticalSection(&s_log.cs);
#elif linux
		pthread_mutex_init(&s_log.mutex, NULL);
#endif
		s_log.initialized = 1;
		ret = SECCESS;
	} while(0);
		
	return ret;
}


int PrintLog(LEVEL level, char *format, ...)
{
	va_list args;
	int i = 0, ret = 0;
	char logString[2048] = {0};

	do
	{
		if (0 == s_log.initialized)
		{
			ret = NOINIT;
			break;
		}

		if (level < s_log.logLevel)
		{
			ret = LOW_LEVEL;
			break;
		}

		va_start(args, format);
#ifdef WIN32	
		i = vsprintf(logString, format, args);
		EnterCriticalSection(&s_log.cs);
#elif linux
		i = vsnprintf(logString, 2048, format, args);
		pthread_mutex_lock(&s_log.mutex);
#endif
		if (s_log.outWay == OUT_FILE)
		{
#ifdef WIN32
			fprintf(s_log.fp, "%u : %d : ", GetCurrentThreadId(), level);
#elif linux
			fprintf(s_log.fp, "%u : %d : ", pthread_self(), level);	
#endif
			ret = fwrite(logString, 1, i, s_log.fp);
			fflush(s_log.fp);
		}
		else if (s_log.outWay == OUT_STDOUT)
		{
			printf("%d %s", level, logString);
		}
#ifdef WIN32
		LeaveCriticalSection(&s_log.cs);
#elif linux
		pthread_mutex_unlock(&s_log.mutex);
#endif
		ret = SECCESS; 

	} while(0);

	return ret;
}
void FreeLog()
{
	if (0 == s_log.initialized)
	{
		return ;
	}

	s_log.initialized = 0;
	fclose(s_log.fp);
#ifdef WIN32	
	DeleteCriticalSection(&s_log.cs);
#elif linux
	pthread_mutex_destroy(&s_log.mutex);
#endif
}
