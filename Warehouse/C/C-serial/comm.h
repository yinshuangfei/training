#ifndef _SERIAL_COMM_
#define _SERIAL_COMM_

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <syslog.h>
#include <stdarg.h>

#define PID_FILE "/var/run/serial_daemon.pid"
#define DAEMON_NAME "serial_daemon"  

/* control syslog */
#define DEBUG_SWITCH 1
#if ( DEBUG_SWITCH > 0 )
    #define DEBUG
#endif

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define EXIT(error) do { perror("[Error] " error); exit(EXIT_FAILURE); } while(0)

#define DEBUG_LOG(dbg, domain, fmt, ...) \
if (dbg > 0) \
    printf("[%s] (%s:%d, fn:%s) "fmt, domain, __FILENAME__, __LINE__, __FUNCTION__, ##__VA_ARGS__);\
else ;

int is_dbg;

static void 
remove_pidfile();

static int 
create_pidfile(const char *pidfile);

void 
init_daemon();

void 
init_syslog();

int 
daemon_log(int __level, const char *const syslogformat, ...);

static int 
_daemon_log(int __level, const char *const syslogformat, va_list args);

#endif /* _SERIAL_COMM_ */