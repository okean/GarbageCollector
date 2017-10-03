#include <stdafx.h>
#include <GarbageCollector/GCPtr.hpp>

using namespace Util::Memory;

namespace
{
    class MyClass
    {
    public:
        MyClass()
            : a(0)
            , b(0)
        {
        };

        MyClass(int x, int y)
            : a(x)
            , b(y)
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

        double c[1000000];

    private:
        int a, b;
    };

    std::ostream &operator<<(std::ostream &strm, const MyClass &obj)
    {
        strm << "(" << obj.a << " " << obj.b << ")";

        return strm;
    }
}

TEST(Load, Test)
{
    GCPtr<MyClass> mp;

    ASSERT_NO_THROW(
        for (size_t i = 1; i < 20000; i++)
        {
            try
            {
                mp = new MyClass(i, i);
            }
            catch (std::bad_alloc ex)
            {
                std::cout << "Last object: " << *mp << std::endl;
                std::cout << "Size of gclist before calling collect(): "
                    << GCPtr<MyClass>::gclistSize() << std::endl;

                GCPtr<MyClass>::collect();

                std::cout << "Size of gclist after calling collect(): "
                    << GCPtr<MyClass>::gclistSize() << std::endl;
            }
        }
    );
}