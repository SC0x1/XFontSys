#ifndef singleton_h__
#define singleton_h__

#pragma once

template<typename T>
class Singleton
{
public:
    static T& Spec()
    {
        static T specimen;
        return specimen;
    }
protected:
    Singleton() {}
private:
    Singleton(const T&);
    T& operator = (const T&);
};

#endif // singleton_h__