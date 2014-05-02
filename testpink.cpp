// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// New Shade of Pink
// (c) 2014 Stefan Stenzel 
// stefan at waldorfmusic.de
//  
// Simple Test for pink noise generators
//
// outputs three files when run:
// pink.raw			pink noise as 32 bit float
// pink-low.raw		same as above with more low frequency content
// pink-int.raw     pink noise as 16-bi int
//
// load as raw data into audacity for verification
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#include "pink.h"
#include "pink-int.h"
#include "pink-low.h"

#include <stdio.h>

#define NSMP	0x40000

float PinkNoise[NSMP];
float  LowNoise[NSMP];
short ShortNoise[NSMP];

int main(void)
{
	pink    p;
	pinklow pl;
	pinkint pi;
	
	for(int i=0; i<NSMP; i+=16)
	{
		p.generate16(PinkNoise+i);
		pl.generate16(LowNoise+i);
		pi.generate16(ShortNoise+i);
	}
	
	FILE *f = fopen("pink.raw","wb");
	fwrite(PinkNoise,NSMP * sizeof(float),1,f);
	fclose(f);
	
	f = fopen("pink-low.raw","wb");
	fwrite(LowNoise,NSMP * sizeof(float),1,f);
	fclose(f);
	
	f = fopen("pink-int.raw","wb");
	fwrite(ShortNoise,NSMP * sizeof(short),1,f);
	fclose(f);
	
	return 0;
}