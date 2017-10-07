#pragma once

#include "stdafx.h"
#include "CriticalSection.h"
#include "Iterator.hpp"
#include <process.h>

namespace Util {
namespace Memory {

template <class T, size_t size = 0>
class GCPtr
{
public:
    GCPtr(T *t = nullptr);
    GCPtr(const GCPtr &obj);
    ~GCPtr();

public: // public typedefs
    typedef Iterator<T> GCIterator;

public: // interface
    T &operator*() const
    {
        return *(gcPtrImpl->memPtr);
    }

    operator T * () const
    {
        return gcPtrImpl->memPtr;
    }

    T *operator->() const
    {
        return gcPtrImpl->memPtr;
    }

    T &operator[](size_t i) const
    {
        return gcPtrImpl->memPtr[i];
    }

    T *operator=(T *t);
    
    GCPtr<T, size> & operator=(GCPtr &obj);

    GCIterator begin() const;
    GCIterator end() const;

public: // class interface
    static size_t gclistSize()
    {
        CRITICAL_SECTION_LOCK(cs);

        return gclist.size();
    }
    static bool collect();
    static bool isRunning();
    static void showlist();
    static void cleanup();

private: // internal classes
    template<class T> struct GCPtrImpl
    {
        GCPtrImpl(T *t, size_t size = 0)
            : memPtr    { t }
            , refCount  { 1 }
            , arraySize { size }
        {
        }

        GCPtrImpl(const GCPtrImpl &obj)
            : memPtr    { obj.memPtr }
            , refCount  { obj.refCount }
            , arraySize { obj.arraySize }
        {
        }

        ~GCPtrImpl()
        {
            if (memPtr)
            {
                if (isArray())
                {
                    #ifdef _DEBUG
                    std::cout << "Deleting array of size " << arraySize << std::endl;
                    #endif

                    delete[] memPtr;
                }
                else
                {
                    #ifdef _DEBUG
                    std::cout << "Deleting: " << *(T *) memPtr << std::endl;
                    #endif

                    delete memPtr;
                }
            }
        }

        bool isArray() const
        {
            return arraySize != 0;
        }

        T * memPtr          = nullptr;
        size_t refCount     = 0;
        size_t arraySize    = 0;
    };

private: // internal class typedefs
    typedef GCPtrImpl<T>                GCPtrImpl_;
    typedef std::vector<GCPtrImpl_*>    GCPtrList_;

private: // internal class helpers
    static typename GCPtrList_::iterator findPtrImpl(const T *ptr);
    static GCPtrImpl_* createPtrOrIncreaseRefcount(T *ptr, size_t size);
    static unsigned __stdcall gc(void *param);

private: // member variables
    static GCPtrList_       gclist;
    static HANDLE           hThrd;
    static CriticalSection  cs;
    static size_t           instCount;

    GCPtrImpl_             *gcPtrImpl;
    unsigned                tid;
};

template <class T, size_t size>
typename GCPtr<T, size>::GCPtrList_ GCPtr<T, size>::gclist;

template <class T, size_t size>
HANDLE GCPtr<T, size>::hThrd = 0;

template <class T, size_t size>
CriticalSection GCPtr<T, size>::cs;

template <class T, size_t size>
size_t GCPtr<T, size>::instCount = 0;

template <class T, size_t size>
GCPtr<T, size>::GCPtr(T *t)
    : gcPtrImpl{ createPtrOrIncreaseRefcount(t, size) }
{
    CRITICAL_SECTION_LOCK(cs);

    if (!hThrd)
    {
        hThrd = (HANDLE) _beginthreadex(NULL, 0, gc, (void *) 0, 0, &tid);

        ::SetThreadPriority(hThrd, THREAD_PRIORITY_BELOW_NORMAL);

        ::atexit(cleanup);
    }

    instCount++;

    #ifdef _DEBUG
    std::cout << "Constructing GCPtr. ";
    
    if (gcPtrImpl->isArray())
    {
        std::cout << " Size is " << gcPtrImpl->arraySize;
    }

    std::cout << std::endl;
    #endif
}

template <class T, size_t size>
GCPtr<T, size>::GCPtr(const GCPtr &obj)
    : gcPtrImpl{ createPtrOrIncreaseRefcount(obj.gcPtrImpl->memPtr, obj.gcPtrImpl->arraySize) }
{
    #ifdef _DEBUG
    std::cout << "Constructing copy. ";

    if (gcPtrImpl->isArray())
    {
        std::cout << " Size is " << gcPtrImpl->arraySize;
    }

    std::cout << std::endl;
    #endif
}

template <class T, size_t size>
GCPtr<T, size>::~GCPtr()
{
    CRITICAL_SECTION_LOCK(cs);

    if (gcPtrImpl->refCount)
    {
        gcPtrImpl->refCount--;
    }

    instCount--;

    #ifdef _DEBUG
    std::cout << "GCPtr going out of scope." << std::endl;
    #endif
}

template <class T, size_t size>
T * GCPtr<T, size>::operator=(T *t)
{
    CRITICAL_SECTION_LOCK(cs);

    if (gcPtrImpl->refCount)
    {
        gcPtrImpl->refCount--;
    }

    gcPtrImpl = createPtrOrIncreaseRefcount(t, size);

    return t;
}

template <class T, size_t size>
GCPtr<T, size> & GCPtr<T, size>::operator=(GCPtr &obj)
{
    CRITICAL_SECTION_LOCK(cs);

    if (gcPtrImpl->refCount)
    {
        gcPtrImpl->refCount--;
    }

    gcPtrImpl = createPtrOrIncreaseRefcount(obj.gcPtrImpl->memPtr, obj.gcPtrImpl->arraySize);

    return obj;
}

template <class T, size_t size>
Iterator<T> GCPtr<T, size>::begin() const
{
    size_t size = 1;

    if (gcPtrImpl->isArray())
    {
        size = gcPtrImpl->arraySize;
    }

    return GCIterator(gcPtrImpl->memPtr, 
        gcPtrImpl->memPtr, 
        gcPtrImpl->memPtr + size);
}

template <class T, size_t size>
Iterator<T> GCPtr<T, size>::end() const
{
    size_t size = 1;

    if (gcPtrImpl->isArray())
    {
        size = gcPtrImpl->arraySize;
    }

    return GCIterator(gcPtrImpl->memPtr + size,
        gcPtrImpl->memPtr,
        gcPtrImpl->memPtr + size);
}

template <class T, size_t size>
bool GCPtr<T, size>::collect()
{
    CRITICAL_SECTION_LOCK(cs);

    bool memfreed = false;

    #ifdef _DEBUG
    std::cout << "Before garbage collection for ";
    showlist();
    #endif

    GCPtrList_::iterator it;

    do
    {
        for (it = gclist.begin(); it != gclist.end(); ++it)
        {
            if ((*it)->refCount > 0)
            {
                continue;
            }

            delete *it;

            it = gclist.erase(it);

            memfreed = true;

            break;
        }
    } 
    while (it != gclist.end());

    #ifdef _DEBUG
    std::cout << "After garbage collection for ";
    showlist();
    #endif

    return memfreed;
}

template <class T, size_t size>
bool GCPtr<T, size>::isRunning()
{
    return instCount > 0;
}

template <class T, size_t size>
void GCPtr<T, size>::showlist()
{
    CRITICAL_SECTION_LOCK(cs);

    std::cout << "gclist<" << typeid(T).name() << ", " << size << ">:\n";

    std::cout << "memPtr\t\trefCount\tvalue\n";

    if (gclist.begin() == gclist.end())
    {
        std::cout << " -- Empty --\n\n";
    }

    for (auto it : gclist)
    {
        std::cout << "[" << (void *)it->memPtr << "]\t\t" << it->refCount << "\t";

        if (it->memPtr)
        {
            std::cout << *(it->memPtr);
        }
        else
        {
            std::cout << "---";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

template <class T, size_t size>
void GCPtr<T, size>::cleanup()
{
    if (gclist.empty())
    {
        return;
    }

    for (auto it : gclist)
    {
        it->refCount = 0;
    }

    #ifdef _DEBUG
    std::cout << "Before collecting for cleanup() for " << typeid(T).name() << std::endl;
    #endif

    collect();

    #ifdef _DEBUG
    std::cout << "After collecting for cleanup() for " << typeid(T).name() << std::endl;
    #endif

    gclist.clear();
}

template <class T, size_t size>
typename GCPtr<T, size>::GCPtrList_::iterator GCPtr<T, size>::findPtrImpl(const T *ptr)
{
    auto onPtrMatch = [=](const GCPtrImpl_ *ptrImpl)
    {
        return ptrImpl->memPtr == ptr;
    };

    return std::find_if(gclist.begin(), gclist.end(), onPtrMatch);
}

template <class T, size_t size>
typename GCPtr<T, size>::GCPtrImpl_ * GCPtr<T, size>::createPtrOrIncreaseRefcount(T *ptr, size_t size)
{
    CRITICAL_SECTION_LOCK(cs);

    GCPtrImpl_ *gcPtr_;

    auto it = findPtrImpl(ptr);

    if (it != gclist.end())
    {
        (*it)->refCount++;

        gcPtr_ = *it;
    }
    else
    {
        gcPtr_ = new GCPtrImpl_(ptr, size);

        gclist.push_back(gcPtr_);
    }

    return gcPtr_;
}

template <class T, size_t size>
unsigned __stdcall GCPtr<T, size>::gc(void *param)
{
    #ifdef DEBUG
    std::cout << "Garbage collection started" << std::endl;
    #endif

    while (isRunning())
    {
        collect();
        ::Sleep(5);
    }

    collect();

    ::CloseHandle(hThrd);
    hThrd = 0;

    #ifdef DEBUG
    std::cout << "Garbage collection terminated for" <<
        << typeid(T).name() <<  std::endl;
    #endif

    return 0;
}

}  // namespace Memory
}  // namespace Util
