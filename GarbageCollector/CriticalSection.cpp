#include "stdafx.h"
#include "CriticalSection.h"

using namespace Util::Memory;

CriticalSection::CriticalSection()
    : _cs()
{
    ::InitializeCriticalSection(&_cs);
}

CriticalSection::~CriticalSection()
{
    ::DeleteCriticalSection(&_cs);
}

CriticalSection::Lock::Lock(CriticalSection &cs)
    : _cs(cs)
{
    _cs.enter();
}

CriticalSection::Lock::~Lock()
{
    _cs.leave();
}

// internal helpers

void CriticalSection::enter()
{
    ::EnterCriticalSection(&_cs);
}

void CriticalSection::leave()
{
    ::LeaveCriticalSection(&_cs);
}
