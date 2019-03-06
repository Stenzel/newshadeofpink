// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// New Shade of Pink
// (c) 2014 Stefan Stenzel 
// stefan at ioptigan.com
//  
// This variant generates 16-bit integer output
// Terms of use:
// Use for any purpose. If used in a commercial product, you should give me one.  
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifndef PINKINT_H
#define PINKINT_H

class pinkint
{
public: 
    void generate16(short *out)                 // generate 16 samples of pink noise
    {
        register int inc    =   pinc;           // load old fashioned register variables-
        register int dec    =   pdec;           // as ugly as this might seem, it improves 
        register int accu   =   paccu;          // the generated code for most compilers
        register int lfsr   =   plfsr;
        register int bit;
                                                
#define PINT(bitmask)                /* macro for processing:           */\
        bit = lfsr >> 31;            /* spill random to all bits        */\
        dec &= ~bitmask;             /* blank old decrement bit         */\
        lfsr <<= 1;                  /* shift lfsr                      */\
        dec |= inc & bitmask;        /* copy increment to decrement bit */\
        inc ^= bit & bitmask;        /* new random bit                  */\
        accu += inc - dec;           /* integrate                       */\
        lfsr ^= bit & 0x46000001;    /* update lfsr                     */\
        *out++ = accu +              /* save output                     */\
             pfira[lfsr & 0x3F] +    /* add 1st half precalculated FIR  */\
             pfirb[lfsr >> 6 & 0x3F] /* add 2nd half, also correts bias */

        int mask = pnmask[pncnt++];    // pncnt is 8 bit for relaxed modulo
 
        PINT(mask);   PINT(0x0800); PINT(0x0400); PINT(0x0800);
        PINT(0x0200); PINT(0x0800); PINT(0x0400); PINT(0x0800);
        PINT(0x0100); PINT(0x0800); PINT(0x0400); PINT(0x0800);
        PINT(0x0200); PINT(0x0800); PINT(0x0400); PINT(0x0800);
        
        pinc    =   inc;                        // write back variables
        pdec    =   dec;
        paccu   =   accu;
        plfsr   =   lfsr;
    };
    
    pinkint()                                   // constructor  
    {
        plfsr  = 0x5EEED1F5 + instance_cnt++;   // anything but zero, decorrelate multiple instances
        paccu  = 0;
        pncnt  = 0;                             // counter from zero    
        pinc   = 0x0CCC;                        // balance initial states to avoid DC 
        pdec   = 0x0CCC;
    };  
    
private:    
    static const unsigned char pnmask[256];     // lookup for bitrversed masks
    static const int pfira[64];                 // 1st precalculated FIR lookup table
    static const int pfirb[64];                 // 2nd precalculated FIR lookup table
    static int instance_cnt;                    // used for decorrelation in case of multiple instances
    int plfsr;                                  // linear feedback shift register
    int pinc;                                   // increment for all noise sources (bits)
    int pdec;                                   // decrement for all noise sources
    int paccu;                                  // accu
    unsigned char pncnt;                        // overflowing counter as index to pnmask[]
};

#endif          // PINKINT_H