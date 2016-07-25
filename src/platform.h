#ifndef NAIVEDB_PLATFORM_H
#define NAIVEDB_PLATFORM_H


#ifdef _WIN32

#include <windows.h>
typedef HANDLE FD;
#define InvalidFD INVALID_HANDLE_VALUE

#else

typedef int FD;
#define InvalidFD (-1)

#endif


#endif //NAIVEDB_PLATFORM_H
