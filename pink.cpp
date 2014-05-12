// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// New Shade of Pink
// (c) 2014 Stefan Stenzel
// stefan at waldorfmusic.de
//  
// Terms of use:
// Use for any purpose. If used in a commercial product, you should give me one.  
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// see pink.h for actual code, this contains only static variables and tables

#include "pink.h"

int pink::instance_cnt = 0;

// Let preprocessor and compiler calculate two lookup tables for 12-tap FIR filter with these coefficients:
// 1.190566,0.162580,0.002208,0.025475,-0.001522,0.007322,0.001774,0.004529,-0.001561,0.000776,-0.000486,0.002017

#define F(cf,m,shift)   (0.0625f*cf*(2*((m)>>shift&1)-1))

#define FA(n)   F(1.190566,n,0)+F(0.162580,n,1)+F(0.002208,n,2)+ F(0.025475,n,3)+F(-0.001522,n,4)+F(0.007322,n,5)-PINK_BIAS         
#define FB(n)   F(0.001774,n,0)+F(0.004529,n,1)+F(-0.001561,n,2)+F(0.000776,n,3)+F(-0.000486,n,4)+F(0.002017,n,5)

#define FA8(n)  FA(n),FA(n+1),FA(n+2),FA(n+3),FA(n+4),FA(n+5),FA(n+6),FA(n+7)   
#define FB8(n)  FB(n),FB(n+1),FB(n+2),FB(n+3),FB(n+4),FB(n+5),FB(n+6),FB(n+7)   

const float pink::pfira[64] = {FA8(0),FA8(8),FA8(16),FA8(24),FA8(32),FA8(40),FA8(48),FA8(56)}; // 1st FIR lookup table 
const float pink::pfirb[64] = {FB8(0),FB8(8),FB8(16),FB8(24),FB8(32),FB8(40),FB8(48),FB8(56)}; // 2nd FIR lookup table  
            
// bitreversed lookup table
#define PM16(n) n<<7,0x4000,0x2000,0x4000,0x1000,0x4000,0x2000,0x4000,0x0800,0x4000,0x2000,0x4000,0x1000,0x4000,0x2000,0x4000

const unsigned int pink::pnmask[256] = 
{
    PM16(0),PM16(0x08),PM16(0x04),PM16(0x08),PM16(2),PM16(0x08),PM16(0x04),PM16(0x08),
    PM16(1),PM16(0x08),PM16(0x04),PM16(0x08),PM16(2),PM16(0x08),PM16(0x04),PM16(0x08)
};