// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// New Shade of Pink
// (c) 2014 Stefan Stenzel 
// stefan at ioptigan.com
//  
// Simple Test for pink noise generators
//
// outputs four files when run:
// pink.wav         pink noise as 32 bit float
// pink-low.wav     same as above with more low frequency content
// pink-short.wav   pink noise as 16-bit int
// pink52.wav       pink noise from 52 individual octave spaced generators
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#include "pink.h"
#include "pink-int.h"
#include "pink-low.h"
#include "pink52.h"

#include <stdio.h>

#define NSMP    0x40000

float PinkNoise[NSMP];
float LowNoise[NSMP];
short ShortNoise[NSMP];
float Pink52Noise[NSMP];

void float2wav(float *smp, int n, int rate, const char *wavname)    // quick & dirty wav writer 
{
    int head[] = {'FFIR',4*n+36,'EVAW',' tmf',16,0x10003,rate,4*rate,0x200004,'atad',4*n};
    FILE *f = fopen(wavname,"wb"); 
    fwrite(&head,44,1,f); 
    fwrite(smp,n,4,f); 
    fclose(f);
}

void short2wav(short *smp, int n, int rate, const char *wavname)    // same for 16 bit values
{
    int head[] = {'FFIR',2*n+36,'EVAW',' tmf',16,0x10001,rate,2*rate,0x100002,'atad',2*n};
    FILE *f = fopen(wavname,"wb"); 
    fwrite(&head,44,1,f); 
    fwrite(smp,n,2,f); 
    fclose(f);
}

int main(void)
{
    pink    pinknoise;
    pinklow plownoise;
    pinkint shortnoise;
    pink52  p52octaves;
    
    for(int i=0; i<NSMP; i+=16)
    {
        pinknoise.generate16(PinkNoise+i);
        plownoise.generate16(LowNoise+i);
        shortnoise.generate16(ShortNoise+i);
        p52octaves.generate16(Pink52Noise+i);
    }
    
    float2wav(PinkNoise,NSMP,44100,"pink.wav");
    float2wav(LowNoise,NSMP,44100,"pink-low.wav");
    float2wav(Pink52Noise,NSMP,44100,"pink52.wav");
    short2wav(ShortNoise,NSMP,44100,"pink-short.wav");
    
    return 0;
}