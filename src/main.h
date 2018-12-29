#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdio.h>

// #define _DEBUG

/*
 * debug level for different message
 */
typedef enum {
    E_ERROR,
    E_INFO,
    E_DEBUG,
    E_ALL,
}DEBUG_LEVEL_E;

/*
 * the global debug level flag.
 */
extern DEBUG_LEVEL_E debug_level;


/*
 * print the message for defferent log level
 */
#ifdef _DEBUG
#define log_msg(level, fmt, ...) do{ \
                                    if(level <= debug_level) \
                                    {\
                                        if(level == E_ERROR) \
                                            printf("\033[1m\033[31;40m"); \
                                        printf("[%s] "fmt, #level, ##__VA_ARGS__); \
                                        if(level == E_DEBUG) \
                                            printf("%10s[%s: %s: %d] ", "--> ", __FILE__,__func__, __LINE__); \
                                        printf("\033[0m\r\n"); \
                                    } \
                                }while(0)
#else
#define log_msg(level, fmt, ...)
#endif



#endif

