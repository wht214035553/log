#ifndef __LOG_H__
#define __LOG_H__

#ifdef WIN32

#define BUILDING_DLL 1
#if BUILDING_DLL
# define DLLIMPORT __declspec (dllexport)
#else
# define DLLIMPORT __declspec (dllimport)
#endif

#elif linux

#ifndef MAX_PATH
#define MAX_PATH 256
#endif

#define DLLIMPORT

#include <stdarg.h>

#endif


typedef unsigned char uint8_t;

#ifdef __cplusplus
extern "C"
{
#endif

#define MYDEBUG 1
#if MYDEBUG
#define PRINTLOG(level, format, ...) \
PrintLog(level, format, ##__VA_ARGS__)
#else
#define PRINTLOG(level, format, ...)
#endif

#define SECCESS 1
#define INITED	0
#define OPEN_FAILED -1
#define LOW_LEVEL	-2
#define NOINIT	-3

typedef enum level
{
	//跟踪调试，如进入退出函数时使用
	LOG_TRACE = 0,
	//信息日志，入申请内存，或者一些有必要记录的数字等
	LOG_INFO = 1,
	//警告，如文件不存在，打开文件失败等
	LOG_WARNING = 2,
	//错误，如不能联网，某些功能模块不能加载等
	LOG_ERROR = 3,
	//严重错误，程序核心模块加载失败，程序不能完成重要的核心的工作
	LOG_FATAL = 4
}LEVEL;
typedef enum outWay
{
	OUT_FILE = 0,
	OUT_STDOUT = 1
}OUTWAY;

DLLIMPORT int InitLog(LEVEL level, OUTWAY outWay);
DLLIMPORT int PrintLog(LEVEL level, char *format, ...);
DLLIMPORT void FreeLog();

#ifdef __cplusplus
};
#endif

#endif //__LOG_H__
