#pragma once

#include "stdafx.h"

namespace Util {
namespace Memory {

template <class T> 
class Iterator
{
public:
    Iterator();
    Iterator(T *ptr, T *begin, T *end);
    ~Iterator();

public: // interface
    T & operator*   () const;
    T * operator&   () const;
    T * operator->  () const;
    T & operator[]  (size_t i) const;

    Iterator<T> & operator++ ();
    Iterator<T> & operator-- ();
    Iterator<T>   operator++ (int);
    Iterator<T>   operator-- (int);
    Iterator<T> & operator+  (size_t n);
    Iterator<T> & operator-  (size_t n);

    bool operator== (const Iterator<T> &obj) const;
    bool operator!= (const Iterator<T> &obj) const;
    bool operator<  (const Iterator<T> &obj) const;
    bool operator<= (const Iterator<T> &obj) const;
    bool operator>  (const Iterator<T> &obj) const;
    bool operator>= (const Iterator<T> &obj) const;

    size_t size() const;

    size_t operator- (const Iterator<T> &obj) const;

private: // member variables
    T*      ptr;
    T*      begin;
    T*      end;
    size_t  length;
};

template <class T>
Iterator<T>::Iterator()
    : ptr      {}
    , begin    {}
    , end      {}
    , length   { 0 }
{
}

template <class T>
Iterator<T>::Iterator(T *p, T *first, T *last)
    : ptr      { p }
    , begin    { first }
    , end      { last }
    , length   { last - first }
{
}

template <class T>
Iterator<T>::~Iterator()
{
}

template <class T>
T & Iterator<T>::operator* () const
{
    if (ptr < begin || ptr >= end)
    {
        throw std::exception("Iterator out of bounds");
    }

    return *ptr;
}

template <class T>
T * Iterator<T>::operator& () const
{
    if (ptr < begin || ptr >= end)
    {
        throw std::exception("Iterator out of bounds");
    }

    return ptr;
}

template <class T>
T * Iterator<T>::operator-> () const
{
    if (ptr < begin || ptr >= end)
    {
        throw std::exception("Iterator out of bounds");
    }

    return ptr;
}

template <class T>
T & Iterator<T>::operator[] (size_t i) const
{
    if (ptr < begin || ptr >= end)
    {
        throw std::exception("Iterator out of bounds");
    }

    return ptr[i];
}

template <class T>
Iterator<T> & Iterator<T>::operator++ ()
{
    ptr++;
    return *this;
}

template <class T>
Iterator<T> & Iterator<T>::operator-- ()
{
    ptr--;
    return *this;
}

template <class T>
Iterator<T> Iterator<T>::operator++ (int)
{
    T * tmp = ptr;

    ptr++;
    return Iterator<T>(tmp, begin, end);
}

template <class T>
Iterator<T> Iterator<T>::operator-- (int)
{
    T * tmp = ptr;

    ptr--;
    return Iterator<T>(tmp, begin, end);
}

template <class T>
Iterator<T> & Iterator<T>::operator+ (size_t n)
{
    ptr += n;
    return *this;
}

template <class T>
Iterator<T> & Iterator<T>::operator- (size_t n)
{
    ptr -= n;
    return *this;
}

template <class T>
bool Iterator<T>::operator== (const Iterator<T> &obj) const
{
    return ptr == obj.ptr;
}

template <class T>
bool Iterator<T>::operator!= (const Iterator<T> &obj) const
{
    return ptr != obj.ptr;
}

template <class T>
bool Iterator<T>::operator< (const Iterator<T> &obj) const
{
    return ptr < obj.ptr;
}

template <class T>
bool Iterator<T>::operator<= (const Iterator<T> &obj) const
{
    return ptr <= obj.ptr;
}

template <class T>
bool Iterator<T>::operator> (const Iterator<T> &obj) const
{
    return ptr > obj.ptr;
}

template <class T>
bool Iterator<T>::operator>= (const Iterator<T> &obj) const
{
    return ptr >= obj.ptr;
}

template <class T>
size_t Iterator<T>::size() const
{
    return length;
}


template <class T>
size_t Iterator<T>::operator- (const Iterator<T> &obj) const
{
    return abs(ptr - obj.ptr);
}

} // namespace Memory
} // namespace Iterator
