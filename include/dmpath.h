
#ifndef __DMPATH_H_INCLUDE__
#define __DMPATH_H_INCLUDE__

#include "dmos.h"

#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

#ifdef _WIN32
bool AddToPath(const std::string& newPath);
#endif

#endif // __DMPATH_H_INCLUDE__
