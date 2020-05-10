#ifndef __DUCK_HPP__
#define __DUCK_HPP__

#include <behavior.hpp>

class Duck
{
public:
    Duck() = default;
    void swim(void);
    virtual void display(void) = 0;

}

class TangDuck:Duck
{
    TangDuck() = default;
}

#endif
