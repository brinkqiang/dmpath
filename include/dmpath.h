
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

static std::string DMGetDllPath() {
#ifdef WIN32
    static char path[MAX_PATH];
    static bool first_time = true;

    if (first_time) {
        first_time = false;
        GetModuleFileNameA(GetSelfModuleHandle(), path, sizeof(path));
        char* p = strrchr(path, '\\');
        *(p) = '\0';
    }

    return path;
#elif __APPLE__
    static char path[MAX_PATH];
    static bool first_time = true;

    if (first_time) {
        uint32_t size = sizeof(path);
        int nRet = _NSGetExecutablePath(path, &size);

        if (nRet != 0) {
            return "./";
        }

        char* p = strrchr(path, '/');
        *(p) = '\0';
    }

    return path;
#else
    static char path[MAX_PATH];
    static bool first_time = true;

    if (first_time) {
        first_time = false;
        int nRet = readlink("/proc/self/exe", path, MAX_PATH);

        if (nRet < 0 || nRet >= MAX_PATH) {
            return "./";
        }

        char* p = strrchr(path, '/');
        *(p) = '\0';
    }

    return path;
#endif
}


#endif // __DMPATH_H_INCLUDE__
