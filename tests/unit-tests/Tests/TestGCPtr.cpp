#include <stdafx.h>
#include <GarbageCollector/GCPtr.hpp>

using namespace Util::Memory;

namespace
{
    int passGCPtr(const GCPtr<int> p)
    {
        #ifdef _DEBUG
        std::cout << "Inside passGCPtr(): " << *p << std::endl;
        #endif

        return  *p;
    }

    int passPtr(const int *p)
    {
        #ifdef _DEBUG
        std::cout << "Inside passPtr(): " << *p << std::endl;
        #endif

        return  *p;
    }

    class MyClass
    {
    public:
        MyClass() 
            : a(0)
            , b(0)
            , c(.0)
        {
        };
        
        MyClass(int x, int y) 
            : a(x)
            , b(y)
            , c(.0)
        {
        };
        
        ~MyClass()
        {
            #ifdef _DEBUG
            std::cout << "Destructing MyClass(" << a << ", " << b << ")" << std::endl;
            #endif
        }

        int sum() { return a + b; }

        friend std::ostream &operator<<(std::ostream &strm, const MyClass &obj);

        double c;

    private:
        int a, b;
    };

    std::ostream &operator<<(std::ostream &strm, const MyClass &obj)
    {
        strm << "(" << obj.a << " " << obj.b << ")";

        return strm;
    }
}

TEST(GCPtr, Sanity)
{
    const int val = 88;

    GCPtr<int> p;
    ASSERT_NO_THROW(
        p = new int
    );

    *p = val;
    ASSERT_EQ(*p, val);

    int k = *p;
    ASSERT_EQ(k, val);
}

TEST(GCPtr, PassGCPtr)
{
    const int val = 5;

    GCPtr<int> p = new int(val);

    ASSERT_EQ(passGCPtr(p), val);
}

TEST(GCPtr, PassPtr)
{
    const int val = 6;

    GCPtr<int> p = new int(val);

    ASSERT_EQ(passPtr(p), val);
}

TEST(GCPtr, GoingOutOfScope)
{
    GCPtr<int> p = new int(10);
    GCPtr<int> q = new int(11);


    const int val = 12;
    int * k = nullptr;

    #ifdef _DEBUG
    std::cout << "Before entering block" << std::endl;
    #endif

    {
        GCPtr<int> r = new int(val);
        ASSERT_EQ(*r, val);

        k = r;
    }

    ASSERT_NE(*k, val);

    #ifdef _DEBUG
    std::cout << "After exiting block" << std::endl;
    #endif
}

TEST(GCPtr, DiscardedMemory)
{
    const size_t total = 4;
    size_t i = 0;

    GCPtr<int> p;

    while (i++ < total)
    {
        p = new int(i);
    }

    // Check against total + 1 due to an empty ptr when declaring GCPtr<int> p;
    ASSERT_EQ(GCPtr<int>::gclistSize(), total + 1);

    ASSERT_TRUE(GCPtr<int>::collect());

    ASSERT_EQ(*p, i - 1);
}

TEST(GCPtr, ArraySanity)
{
    const size_t size = 10;

    GCPtr<int, size> ap;
    ASSERT_NO_THROW(
        ap = new int[size];
    );

    for (size_t i = 0; i < size; i++)
    {
        ap[i] = i;
    }

    for (size_t i = 0; i < size; i++)
    {
        ASSERT_EQ(ap[i], i);
    }
}

TEST(GCPtr, ClassSanity)
{
    GCPtr<MyClass> ob;
    ASSERT_NO_THROW(
        ob = new MyClass(10, 20);
    );

    #ifdef _DEBUG
    std::cout << "ob points to " << *ob << std::endl;
    #endif

    std::ostringstream strm;
    strm << *ob;

    ASSERT_EQ(strm.str(), "(10 20)");

    ob = new MyClass(11, 22);

    ASSERT_EQ(ob->sum(), 33);

    ob->c = 3.14;
    ASSERT_EQ(ob->c, 3.14);
}

TEST(GCPtr, PointerToGCPtr)
{
    const int val = 100;

    GCPtr<int> *pp = new GCPtr<int>();
    *pp = new int(val);

    ASSERT_EQ(**pp, val);

    delete pp;
}