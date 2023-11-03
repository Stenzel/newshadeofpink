// (c) 2019 Stefan Stenzel
// stefan at ioptigan.com
// struct or union with few operators for binary hacks on floats

#pragma once

#if (__cplusplus >= 202002L)

#include <bit>

struct floathack
{
    float f;
    floathack(float a)             { f  = a; }
    void operator +=(const int b)  { f = std::bit_cast<float>(std::bit_cast<int>(f) + b);}
    operator float() const         {return f;}
};

#else

union floathack
{
    float f;
    int i;
    floathack(float a)             { f  = a; }
    void operator +=(const int a)  { i += a; }
    operator float() const         {return f;}
};

#endif