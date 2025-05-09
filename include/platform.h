#ifndef _PLATFORM_H
#define _PLATFORM_H

#if defined(_WIN32) || defined(_WIN64)
    #define PAUSE "pause"
#elif defined(unix) || defined(__unix__) || defined(__unix)
    #define PAUSE "read -p 'Press Enter to continue...' var"
#endif 

#endif