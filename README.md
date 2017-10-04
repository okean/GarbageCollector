## A Simple Garbage Collector
[![Build status](https://ci.appveyor.com/api/projects/status/i443rq6ocwtsel6p/branch/master?svg=true)](https://ci.appveyor.com/project/okean/garbagecollector/branch/master)

The project was inspired by ideas described in The Art of C++ book by Herbert Schildt

### Examples
```C++
GCPtr<int> p = new int(5);
std::cout << *p;

const size_t size = 10;
GCPtr<int, size> ap = new int[size];

for (size_t i = 0; i < size; i++)
{
  ap[i] = i;
}

for (size_t i = 0; i < size; i++)
{
  std::cout << *(ap + i);
}

GCPtr<int>::collect();
```
Find more examples at <GarbageCollector/tests/unit-tests/Tests> directory

### License
GarbageCollector project is covered by the GNU General Public License version 3
