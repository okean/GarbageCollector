#pragma once

#include <Windows.h>

namespace Util {
namespace Memory {

#define CRITICAL_SECTION_LOCK(x) CriticalSection::Lock lock(x);

class CriticalSection
{
public:
    CriticalSection();
    ~CriticalSection();

public:
    class Lock
    {
    public:
        Lock(CriticalSection &cs);
        ~Lock();

    private: // member variables
        CriticalSection &_cs;
    };

private: // internal helpers
    void enter();
    void leave();

private: // member variables
    CRITICAL_SECTION _cs;
};

} // namespace Memory
} // namespace Util
