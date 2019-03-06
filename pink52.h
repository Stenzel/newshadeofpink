// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// New Shade of Pink
// (c) 2014 Stefan Stenzel 
// stefan at ioptigan.com
//  
// Terms of use:
// Use for any purpose. If used in a commercial product, you should give me one.  
//
// This version calculates 52 octave wide pink noise. 
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifndef PINK52_H
#define PINK52_H

class pink52
{
public: 
    void generate16(float *out)                     // generate 16 samples of pink noise
    {                                               // requires an adaptor if less are wanted
        register long long inc  =   pinc;           // load old fashioned register variables-
        register long long dec  =   pdec;           // as ugly as this might seem, it seems to
        register long long accu =   paccu;          // improve code for most compilers.
        register long long lfsr =   plfsr;
        register long long bit;
            
                                                
#define PINK52(bitmask)              /* macro for processing:           */\
        bit = lfsr >> 63;            /* spill random to all bits        */\
        dec &= ~bitmask;             /* blank old decrement bit         */\
        lfsr <<= 1;                  /* shift lfsr                      */\
        dec |= inc & bitmask;        /* copy increment to decrement bit */\
        inc ^= bit & bitmask;        /* new random bit                  */\
        accu += inc - dec;           /* integrate                       */\
        lfsr ^= bit &  0xA800000000000001ll; /* update lfsr             */\
        *out++ = (accu +              /* save output                     */\
             pfira[lfsr & 0x3F] +    /* add 1st half precalculated FIR  */\
             pfirb[lfsr >> 6 & 0x3F]) * 0x1p-57f; /* add 2nd half and discard low bits */

        long long pindex = pncnt++;
        int shiftcnt = 0;
 
        while(0 == (pindex & 0x0FF) && shiftcnt < 40) 
        {
            pindex >>=  8;
            shiftcnt += 8;
        }

        long long mask = pnmask[pindex & 0x0FF];
        mask <<= 40 - shiftcnt; 

        PINK52(mask);      PINK52((1L<<51)); PINK52((1L<<50)); PINK52((1L<<51));
        PINK52((1L<<49));  PINK52((1L<<51)); PINK52((1L<<50)); PINK52((1L<<51));
        PINK52((1L<<48));  PINK52((1L<<51)); PINK52((1L<<50)); PINK52((1L<<51));
        PINK52((1L<<49));  PINK52((1L<<51)); PINK52((1L<<50)); PINK52((1L<<51));
        
        pinc    =   inc;                        // write back variables
        pdec    =   dec;
        paccu   =   accu;
        plfsr   =   lfsr;
    };
    
    pink52()                                   // constructor  
    {
        plfsr  = 3 + instance_cnt++;   // anything but zero, decorrelate multiple instances
        paccu  = 0;
        pncnt  = 0;                             // counter from zero    
        pinc   = 0x000CCCCCCCCCCCCCL;           // balance initial states to avoid DC 
        pdec   = 0x000CCCCCCCCCCCCCL;
    };  
    
private:    
    static const unsigned char pnmask[256];     // lookup for bitrversed masks
    static const long long pfira[64];                 // 1st precalculated FIR lookup table
    static const long long pfirb[64];                 // 2nd precalculated FIR lookup table
    static int instance_cnt;                    // used for decorrelation in case of multiple instances
    long long plfsr;                            // linear feedback shift register
    long long pinc;                             // increment for all noise sources (bits)
    long long pdec;                             // decrement for all noise sources
    long long paccu;                            // accu
    long long pncnt;                            // index to pnmask[]
};

#endif          // PINK52_H