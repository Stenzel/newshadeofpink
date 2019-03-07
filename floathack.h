// (c) 2019 Stefan Stenzel
// stefan at ioptigan.com
// union with few operators for binary hacks on floats

#pragma once

union floathack
{
    float f;
    int i;
    floathack(float a)             { f  = a; }
    void operator +=(const int a)  { i += a; }
    operator float() const         {return f;}
};