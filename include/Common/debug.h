#ifndef DEBUG_H
#define DEBUG_H

#ifdef SCIQLOP_CRASH_ON_ERROR
    #define SCIQLOP_ERROR(x) assert(false)
#else
    #define SCIQLOP_ERROR(x)
#endif
#endif
