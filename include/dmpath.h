
#ifndef __DMPATH_H_INCLUDE__
#define __DMPATH_H_INCLUDE__

#include "dmos.h"

#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

#ifdef WIN32
HMODULE GetSelfModuleHandle()
{
    MEMORY_BASIC_INFORMATION mbi;
    return ((::VirtualQuery(GetSelfModuleHandle, &mbi, sizeof(mbi)) != 0)
        ? (HMODULE)mbi.AllocationBase : NULL);
}
#endif

 const char* DMAPI DMGetDllPath();

#endif // __DMPATH_H_INCLUDE__
