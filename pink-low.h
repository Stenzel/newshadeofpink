// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// New Shade of Pink
// (c) 2014 Stefan Stenzel 
// stefan at waldorfmusic.de
//  
// This variant sums 20 octave spaced noise sources
// Terms of use:
// Use for any purpose. If used in a commercial product, you should give me one.  
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


#ifndef PINKLOW_H
#define PINKLOW_H

// some temporary bias required for hack on floats
#define PINKLOW_BIAS	1.5f					  	

class pinklow
{
public:	
	void generate16(float *out)					// generate 16 samples of pink noise
	{											// requires an adaptor if less are wanted
		register int inc 	= 	pinc;			// load old fashioned register variables-
		register int dec 	= 	pdec;			// as ugly as this might seem, it improves 
		register int accu 	= 	paccu;			// the generated code for most compilers
		register int lfsr	= 	plfsr;
		register int bit;
												
#define PLOW(bitmask)        		 /* macro for processing:	        */\
		bit = lfsr >> 31;            /* spill random to all bits 	    */\
		dec &= ~bitmask;             /* blank old decrement bit	 	    */\
		lfsr <<= 1;                  /* shift lfsr					   	*/\
		dec |= inc & bitmask;        /* copy increment to decrement bit */\
		inc ^= bit & bitmask;		 /* new random bit                  */\
		*((int *)(out)) = accu;      /* save biased value as float		*/\
		accu += inc - dec;           /* integrate						*/\
		lfsr ^= bit & 0x46000001;    /* update lfsr						*/\
		*out++ += pfira[lfsr & 0x3F] /* add 1st half precalculated FIR	*/\
	       + pfirb[lfsr >> 6 & 0x3F] /* add 2nd half, also correts bias	*/

		int mask = pnmask[pncnt++];    			// pncnt is 8 bit for relaxed modulo
		if(!mask) mask = pnmask[plcnt++]>>8;	// rarely update lowest
 
 		PLOW(mask);     PLOW(0x040000); PLOW(0x020000); PLOW(0x040000);
 		PLOW(0x010000); PLOW(0x040000); PLOW(0x020000); PLOW(0x040000);
 		PLOW(0x008000); PLOW(0x040000); PLOW(0x020000); PLOW(0x040000);
 		PLOW(0x010000); PLOW(0x040000); PLOW(0x020000); PLOW(0x040000);
		
		pinc 	= 	inc;						// write back variables
	    pdec 	= 	dec;
	    paccu 	= 	accu;
	    plfsr	= 	lfsr;
	};
	
	pinklow()										// constructor	
	{
		plfsr  = 0xB0B0FACE + instance_cnt++;	// anything but zero, decorrelate multiple instances
		*((float *)(&paccu))  = PINKLOW_BIAS;	// init float hack
		pncnt = plcnt = 0;						// counter from zero	
		pinc   = 0x04CCCC;						// balance initial states to avoid DC 
		pdec   = 0x04CCCC;
	};	
	
private:	
	static const int pnmask[256];				// lookup for bitrversed masks
	static const float pfira[64];				// 1st precalculated FIR lookup table
	static const float pfirb[64];				// 2nd precalculated FIR lookup table, also for bias correction
	static int instance_cnt;					// used for decorrelation in case of multiple instances
	int plfsr;									// linear feedback shift register
	int pinc;									// increment for all noise sources (bits)
	int pdec;									// decrement for all noise sources
	int paccu;									// accu, also interpreted as float
	unsigned char pncnt;						// overflowing counter as index to pnmask[]
	unsigned char plcnt;
};

#endif			// PINK_H