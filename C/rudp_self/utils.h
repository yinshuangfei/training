#ifndef _HXJ_UTILS_
#define _HXJ_UTILS_

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <semaphore.h>

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define EXIT(error) do { perror("[Error] " error); exit(EXIT_FAILURE); } while(0)

#define __DEBUG__   1

#if (0 < __DEBUG__)
#define LOG(fmt, ...) printf("[Server: %s:%d] "fmt, __FILENAME__, __LINE__, ##__VA_ARGS__)
#else
#define LOG(fmt, ...) {}
#endif

#define FIX_HEAD  sizeof(int32_t)

#endif /* _HXJ_UTILS_ */
