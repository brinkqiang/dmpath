
#ifndef __DMPATH_H_INCLUDE__
#define __DMPATH_H_INCLUDE__

#include "dmos.h"

#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

bool AddToPath(const std::string& newPath);
bool RemoveFromPath(const std::string& pathToRemove);

#endif // __DMPATH_H_INCLUDE__
