#ifndef __LOG_H_
#define __LOG_H_

#include "ul_log.h"

#define _LOG_FATAL(fmt, arg...) \
    do {\
        ul_writelog (UL_LOG_FATAL, "[%s:%d] " fmt, __FILE__, __LINE__, ##arg); \
    }while(0)

#define _LOG_WARNING(fmt, arg...) \
    do {\
        ul_writelog (UL_LOG_WARNING, "[%s:%d] " fmt, __FILE__, __LINE__, \
        ##arg); \
    }while(0)

#define _LOG_NOTICE(fmt, arg...) \
    do {\
        ul_writelog (UL_LOG_NOTICE, "[%s:%d] " fmt, __FILE__, __LINE__, \
        ##arg); \
    }while(0)

#define _LOG_TRACE(fmt, arg...) \
    do {\
        ul_writelog (UL_LOG_TRACE, "[%s:%d] " fmt, __FILE__, __LINE__, ##arg); \
    }while(0)

#define _LOG_DEBUG(fmt, arg...) \
    do {\
        ul_writelog (UL_LOG_DEBUG, "[%s:%d] " fmt, __FILE__, __LINE__, ##arg); \
    }while(0)

#endif
