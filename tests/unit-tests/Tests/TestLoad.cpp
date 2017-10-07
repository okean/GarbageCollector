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

        double c[100000];

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
        for (size_t i = 1; i < 2000; i++)
        {

            mp = new MyClass(i, i);

            if (!(i % 100))
            {
                std::cout << "gclist contains " << GCPtr<MyClass>::gclistSize() << " entries.\n";
            }
        }
    );
}