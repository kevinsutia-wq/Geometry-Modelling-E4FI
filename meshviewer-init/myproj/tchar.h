// Compatibility shim for Linux - tchar.h is Windows-specific
#ifndef TCHAR_H
#define TCHAR_H

#include <string.h>

typedef char TCHAR;

#endif
