#include "comm.h"

#define DOMAIN "COMMON"

static void 
remove_pidfile() {
    char * pidfile = PID_FILE;
    if (-1 == unlink(pidfile))
        DEBUG_LOG(is_dbg, DOMAIN, "failed to remove PID file: %s.\n", pidfile);
}

static int 
create_pidfile(const char *pidfile) {
    char buf[64];
    int fd;

    fd = open(pidfile, O_WRONLY|O_CREAT, 0644);
    if (fd < 0) { 
        daemon_log(LOG_ERR, "unable to create pid file.\n");
        return -1;
    }    

    if (lockf(fd, F_TLOCK, 0) < 0) { 
        daemon_log(LOG_ERR, "unable to lock pid file.\n");
        return -1;
    }    
    if (ftruncate(fd, 0) < 0) {
        daemon_log(LOG_ERR, "failed to ftruncate the PID file: %s.\n", pidfile);
        return -1;
    }    

    sprintf(buf, "%d\n", getpid());
    if (write(fd, buf, strlen(buf)) < strlen(buf)) {
        daemon_log(LOG_ERR, "failed to write PID to PID file: %s.\n", pidfile);
    }
    close(fd);
    return 0;
}

void 
init_daemon() {
    pid_t pid, sid;

#if defined(DEBUG)
    int daemonize = 0;
    is_dbg = 1;
#else
    int daemonize = 1;
    is_dbg = 0;
#endif
    
    if (daemonize) {
        daemon_log(LOG_INFO, "%s server run as daemon. \n", DAEMON_NAME);
    
        pid = fork();  
        if (pid < 0) {
            daemon_log(LOG_ERR, "failed to fork child process.\n");
            exit(EXIT_FAILURE);
        }

        if (pid > 0) {  
            exit(EXIT_SUCCESS);
        }
   
        umask(0);  
        sid = setsid();  
        if (sid < 0) {
            daemon_log(LOG_ERR, "failed to set sid of process: %d.\n", getpid());
            exit(EXIT_FAILURE);
        }  
   
        if ((chdir("/")) < 0) {
            daemon_log(LOG_ERR, "failed to change work directory to '/'.\n");
            exit(EXIT_FAILURE); 
        }  
   
        close(STDIN_FILENO);  
        close(STDOUT_FILENO);  
        close(STDERR_FILENO); 
    }
    create_pidfile(PID_FILE);
    atexit(remove_pidfile);
}

void 
init_syslog() {
#if defined(DEBUG)
    openlog(DAEMON_NAME, LOG_CONS | LOG_NDELAY | LOG_PERROR | LOG_PID,  LOG_LOCAL4);  
    setlogmask(LOG_UPTO(LOG_DEBUG));
#else
    openlog(DAEMON_NAME, LOG_CONS | LOG_PERROR,  LOG_LOCAL4); 
    setlogmask(LOG_UPTO(LOG_INFO));
#endif
}

int 
daemon_log(int __level, const char *const syslogformat, ...) {
    /* reset output file */
    init_syslog();

    va_list args;
    va_start(args, syslogformat);
    
    _daemon_log(__level, syslogformat, args);

    va_end(args);
    return 0;
}


static int 
_daemon_log(int __level, const char *const syslogformat, va_list args) {
    char *format, *level; 
    size_t length; 
    ssize_t result; 

    level = "?"; 
    switch (__level) { 
        case LOG_ALERT: 
            level = "[ALERT]"; 
            break; 
        case LOG_INFO: 
            level = "[INFO]"; 
            break; 
        case LOG_EMERG: 
            level = "[EMERG]"; 
            break; 
        case LOG_CRIT: 
            level = "[CRIT]"; 
            break; 
        case LOG_ERR: 
            level = "[ERR]"; 
            break; 
        case LOG_WARNING: 
            level = "[WARN]"; 
            break; 
        case LOG_NOTICE: 
            level = "[NOTICE]"; 
            break; 
        case LOG_DEBUG: 
            level = "[DEBUG]"; 
            break; 
    } 

    length = strlen(syslogformat) + strlen(level) + 2; 
    format = malloc(length + 1); 
    result = snprintf(format, length + 1, "%s %s", level, syslogformat); 
    
    if ((result < 0) || (result >= length + 1)) {
        syslog(LOG_ERR, "!!! syslog output error!!!");
        va_end(args);
        return -1;
    }
    
    vsyslog(__level, format, args);
    free(format);
    return 0; 
} 