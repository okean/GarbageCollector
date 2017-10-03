#include <stdafx.h>
#include <GarbageCollector/Iterator.hpp>
#include <vector>

using namespace Util::Memory;

const size_t size = 5;

namespace
{
    struct Point
    {
        int x, y;

        bool operator==(const Point &obj) const
        {
            return x == obj.x && y == obj.y;
        }
    };

    Point points[] = { 
        Point{ 11, 12 }, 
        Point{ 22, 23 }, 
        Point{ 33, 34 }, 
        Point{ 44, 45 }, 
        Point{ 55, 56 }
    };
}

TEST(Iterator, Initialization)
{
    Iterator<int> empty;

    EXPECT_EQ(empty.size(), 0);

    Iterator<Point> it(points, points, points + size);

    EXPECT_EQ(it.size(), size);
}

TEST(Iterator, Access)
{
    Iterator<Point> it(points, points, points + size);

    EXPECT_EQ(*it,          points[0]);
    EXPECT_EQ(&it,         &points[0]);
    EXPECT_EQ(it[2],        points[2]);
    EXPECT_EQ(it->x,        points[0].x);

    it--;

    EXPECT_THROW(*it,       std::exception);
    EXPECT_THROW(&it,       std::exception);
    EXPECT_THROW(it[2],     std::exception);
    EXPECT_THROW(it->x,     std::exception);

    try
    {
        Point pt = *it;
    }
    catch (const std::exception &ex)
    {
        EXPECT_EQ(std::string(ex.what()), "Iterator out of bounds");
    }
}

TEST(Iterator, IncrementDecrement)
{
    Iterator<Point> it(points, points, points + size);

    EXPECT_EQ(*(++it),      points[1]);
    EXPECT_EQ(*(--it),      points[0]);

    EXPECT_EQ(*(it++),      points[0]);
    EXPECT_EQ(*it,          points[1]);
    EXPECT_EQ(*(it--),      points[1]);
    EXPECT_EQ(*it,          points[0]);

    EXPECT_EQ(*(it + 2),    points[2]);
    EXPECT_EQ(*(it - 2),    points[0]);
}

TEST(Iterator, Boolean)
{
    Iterator<Point> it1(points, points, points + size);
    Iterator<Point> it2(points, points, points + size);

    EXPECT_TRUE     (it1 == it2);
    EXPECT_FALSE    (it1 != it2);

    it2++;

    EXPECT_FALSE    (it1 == it2);
    EXPECT_TRUE     (it1 != it2);

    EXPECT_TRUE     (it1 <  it2);
    EXPECT_TRUE     (it1 <= it2);
    EXPECT_TRUE     (it2 >  it1);
    EXPECT_TRUE     (it2 >= it1);

    EXPECT_FALSE    (it2 <  it1);
    EXPECT_FALSE    (it2 <= it1);
    EXPECT_FALSE    (it1 >  it2);
    EXPECT_FALSE    (it1 >= it2);
}

TEST(Iterator, Distance)
{
    Iterator<Point> begin   (points, points, points + size);
    Iterator<Point> end     (points + size, points, points + size);

    EXPECT_EQ       (end - begin, size);
    EXPECT_EQ       (begin - end, size);
}
