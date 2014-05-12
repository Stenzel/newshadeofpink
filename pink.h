// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// New Shade of Pink
// (c) 2014 Stefan Stenzel 
// stefan at waldorfmusic.de
//  
// Terms of use:
// Use for any purpose. If used in a commercial product, you should give me one.  
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifndef PINK_H
#define PINK_H

// some temporary bias required for hack on floats
#define PINK_BIAS   3.0f

class pink
{
public: 
    void generate16(float *out)                 // generate 16 samples of pink noise
    {                                           // requires an adaptor if less are wanted
        register int inc    =   pinc;           // load old fashioned register variables-
        register int dec    =   pdec;           // as ugly as this might seem, it improves 
        register int accu   =   paccu;          // the generated code for most compilers
        register int lfsr   =   plfsr;
        register int bit;
                                                
#define PINK(bitmask)                /* macro for processing:           */\
        bit = lfsr >> 31;            /* spill random to all bits        */\
        dec &= ~bitmask;             /* blank old decrement bit         */\
        lfsr <<= 1;                  /* shift lfsr                      */\
        dec |= inc & bitmask;        /* copy increment to decrement bit */\
        inc ^= bit & bitmask;        /* new random bit                  */\
        *((int *)(out)) = accu;      /* save biased value as float      */\
        accu += inc - dec;           /* integrate                       */\
        lfsr ^= bit & 0x46000001;    /* update lfsr                     */\
        *out += pfira[lfsr & 0x3F];  /* add 1st half precalculated FIR  */\
        *out++ += pfirb[lfsr>>6 & 0x3F] /* add 2nd half, also correts bias */

        int mask = pnmask[pncnt++];    // pncnt is 8 bit for relaxed modulo

        PINK(mask);		PINK(0x040000); PINK(0x020000); PINK(0x040000);
        PINK(0x010000); PINK(0x040000); PINK(0x020000); PINK(0x040000);
        PINK(0x008000); PINK(0x040000); PINK(0x020000); PINK(0x040000);
        PINK(0x010000); PINK(0x040000); PINK(0x020000); PINK(0x040000);
        
        pinc    =   inc;                        // write back variables
        pdec    =   dec;
        paccu   =   accu;
        plfsr   =   lfsr;
    };
    
    pink()                                      // constructor  
    {
        plfsr  = 0x5EED41F5 + instance_cnt++;   // seed for lfsr, decorrelate multiple instances
        *((float *)(&paccu))  = PINK_BIAS;      // init float hack
        pncnt = 0;                              // counter from zero    
        pinc   = 0x04CCCC;                      // balance initial states to avoid DC 
        pdec   = 0x04CCCC;        
    };  
    
private:    
    static const unsigned int pnmask[256];      // lookup for bitrversed masks
    static const float pfira[64];               // 1st precalculated FIR lookup table
    static const float pfirb[64];               // 2nd precalculated FIR lookup table, also for bias correction
    static int instance_cnt;                    // used for decorrelation in case of multiple instances
    int plfsr;                                  // linear feedback shift register
    int pinc;                                   // increment for all noise sources (bits)
    int pdec;                                   // decrement for all noise sources
    int paccu;                                  // accu, also interpreted as float
    unsigned char pncnt;                        // overflowing counter as index to pnmask[]
};

#endif          // PINK_H