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
/*作者:吴洪涛
 *功能:初始化日志模块
 *参数:两个联合体，定义在log.h中
 *参数说明:level是日志的预设级别，低于level级别的日志不会被输出，outWay是输出方式目前支持文件输出和控制台输出
 *返回值:SECCESS表示成功，其他值表示失败，具体原因参照.h中的返回值宏
 */
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

/*作者:吴洪涛
 *功能:打印日志
 *参数:将输出的日志的级别和其他的格式化的信息输入(参照printf使用)
 *返回值:返回SECCESS表示成功，其他值表示失败，失败原因参考.h中返回值的宏
 *注意:格式化输入需要注意格式的对应，如%s对应字符串，%d对应int型数值，由于只在程序内部使用，
 *出于效率考虑，本函数并没有对参数进行复杂的检查，请使用者自己注意
 *其他:为了增强代码可读性，.h中提供了一个宏PRINTLOG来代替本函数，请尽量使用宏代替本函数
 */
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

/*作者:吴洪涛
 *功能:释放日志模块的资源
 */
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
