#ifndef DEBUG_H
#define DEBUG_H
#include <string>
#include <iostream>
#include <typeinfo>
#include <assert.h>

template <class T, bool crash>
void SciQLopError(const std::string& message, int line, const std::string& file)
{
    std::cerr << "Error in " << file << ", class:" << typeid(T).name() << ", line: " << line <<
              std::endl << "Message: " << message << std::endl;
    if constexpr(crash)
            throw;
}

#ifdef SCIQLOP_CRASH_ON_ERROR
    #define SCIQLOP_ERROR(CLASS, MESSAGE) SciQLopError<CLASS, true>(MESSAGE, __LINE__, __FILE__)
#else
    #define SCIQLOP_ERROR(CLASS, MESSAGE) SciQLopError<CLASS, false>(MESSAGE, __LINE__, __FILE__)
#endif


#endif
