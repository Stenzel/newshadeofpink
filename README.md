A New Shade of Pink
==============

A new recipe for generation of high quality pink noise. 
Although this is a multirate algorithm, it requires very few operations,
neither multiplication, division nor transcendental functions are used. 

The repository contains a detailed description, here a demonstration with audio that allows selective control of the individual multirate components.
Just hit Play! and enjoy:
   
    
<script>
var draw = 0;
var img;


var noisy   = 0;
var newnoisy= 0;
var newmask = 0x1FFF;
var mask    = 0x1FFF;
var offset  = 0;



function makemask()
{
  	var i,Text="Mask:",m=0;
	for (i=0; i<13; i++)
        if (document.sources.elements[i].checked)
            m |= (1<<i);
    newmask = m;
    fplot();
}
document.write('<form name="sources">\n');
for(var i=0; i<13; i++)
{
	var k = 12-i;
	if(k != 0) 

document.write('<input type=checkbox checked onClick="makemask()">&nbsp' + k + '&nbsp\n');
 	else	document.write('&nbsp<input type=checkbox checked onClick="makemask()">&nbspFIR&nbsp\n');
}
document.write('&nbsp<input type=checkbox onClick="noisectl()">&nbspPlay!\n');
document.write('</form>\n');

var nreg = 0xD0D0FACE;
var lfsr = 0xD0D0FACE;
var dec  = 0x0CCC;
var inc  = 0x0CCC;
var accu = 0;
var cnt = 0;

var pseq = [
            0x00,0x80,0x40,0x80,0x20,0x80,0x40,0x80,0x10,0x80,0x40,0x80,0x20,0x80,0x40,0x80,
            0x08,0x80,0x40,0x80,0x20,0x80,0x40,0x80,0x10,0x80,0x40,0x80,0x20,0x80,0x40,0x80,
            0x04,0x80,0x40,0x80,0x20,0x80,0x40,0x80,0x10,0x80,0x40,0x80,0x20,0x80,0x40,0x80,
            0x08,0x80,0x40,0x80,0x20,0x80,0x40,0x80,0x10,0x80,0x40,0x80,0x20,0x80,0x40,0x80,
            0x02,0x80,0x40,0x80,0x20,0x80,0x40,0x80,0x10,0x80,0x40,0x80,0x20,0x80,0x40,0x80,
            0x08,0x80,0x40,0x80,0x20,0x80,0x40,0x80,0x10,0x80,0x40,0x80,0x20,0x80,0x40,0x80,
            0x04,0x80,0x40,0x80,0x20,0x80,0x40,0x80,0x10,0x80,0x40,0x80,0x20,0x80,0x40,0x80,
            0x08,0x80,0x40,0x80,0x20,0x80,0x40,0x80,0x10,0x80,0x40,0x80,0x20,0x80,0x40,0x80,
            0x01,0x80,0x40,0x80,0x20,0x80,0x40,0x80,0x10,0x80,0x40,0x80,0x20,0x80,0x40,0x80,
            0x08,0x80,0x40,0x80,0x20,0x80,0x40,0x80,0x10,0x80,0x40,0x80,0x20,0x80,0x40,0x80,
            0x04,0x80,0x40,0x80,0x20,0x80,0x40,0x80,0x10,0x80,0x40,0x80,0x20,0x80,0x40,0x80,
            0x08,0x80,0x40,0x80,0x20,0x80,0x40,0x80,0x10,0x80,0x40,0x80,0x20,0x80,0x40,0x80,
            0x02,0x80,0x40,0x80,0x20,0x80,0x40,0x80,0x10,0x80,0x40,0x80,0x20,0x80,0x40,0x80,
            0x08,0x80,0x40,0x80,0x20,0x80,0x40,0x80,0x10,0x80,0x40,0x80,0x20,0x80,0x40,0x80,
            0x04,0x80,0x40,0x80,0x20,0x80,0x40,0x80,0x10,0x80,0x40,0x80,0x20,0x80,0x40,0x80,
            0x08,0x80,0x40,0x80,0x20,0x80,0x40,0x80,0x10,0x80,0x40,0x80,0x20,0x80,0x40,0x80
            ];

//var pfir = [1.19952,0.1639427,0,0.0199870,0,0.0086181,0,0.0051455];
//var pfir =   [1.19016147,0.16090963,0.00209673,0.02437414,-0.00142806,0.00633615,0.00072911,0.00323382];
var pfir8 =  [1.190303,0.162685,0.002433,0.025560,-0.001321,0.007357,0.002008,0.004408];
var pfir9 =  [1.190866,0.162774,0.002376,0.025599,-0.001364,0.007410,0.001939,0.004590,-0.001298];
var pfir10 = [1.191811,0.163642,0.003218,0.026382,-0.000618,0.008099,0.002588,0.005224,-0.000781,0.001425];
var pfir12 = [1.190566,0.162580,0.002208,0.025475,-0.001522,0.007322,0.001774,0.004529,-0.001561,0.000776,-0.000486,0.002017];

var pfir = pfir8;

var pfirtab = new Array();
var pflta   = new Array();
var pfltb   = new Array();
var pfirscale = 2048.0/65536.0;	//4096.0/65536.0;

var pscale   = 1.0/65536.0;

// precalculate all possible 8-tap fir outputs
for(var i=0; i<256; i++)
{
	var p = 0;
	for(var m=0; m<8; m++)
	{
		if(i & (1<<m))	p += pfir8[m];
        else            p -= pfir8[m];
    }
	pfirtab[i] = p * pfirscale ;
}

// precalculate all possible 12-tap fir outputs
for(var i=0; i<0x40; i++)
{
	var a = 0,b = 0;
	for(var m=0; m<6; m++)
	{
		if(i & (1<<m))
		{	
			a += pfir12[m];
			b += pfir12[m+6];
		}
        else
        {	
			a -= pfir12[m];
			b -= pfir12[m+6];
		}
        
    }
	pflta[i] = a * pfirscale ;
	pfltb[i] = b * pfirscale ;
}

/*
// pink noise generator
pink = function(e)
{
    var leftOut = e.outputBuffer.getChannelData(0);
    var rightOut = e.outputBuffer.getChannelData(1);
    
    var bit;
	
    for (var i = 0; i < leftOut.length; i += 16)
    {
        bit = lfsr >> 31;
        dec &= ~pseq[cnt];
        lfsr <<= 1;
        dec |= inc & pseq[cnt];
        inc ^= bit & pseq[cnt];
        accu -= dec;
        lfsr ^= bit & 0x46000001;
        accu += inc;
		leftOut[i] = accu * pscale + pflta[lfsr & 0x3F] + pfltb[lfsr >> 6 & 0x3F];
        
		cnt = (cnt + 1) & 0x0FF;
        
 		bit   = lfsr >> 31;        dec &= ~0x0800;
        lfsr <<= 1;                dec |= inc & 0x0800;
        inc ^= bit  & 0x0800;      accu -= dec;
        lfsr ^= bit & 0x46000001;  accu += inc;
		leftOut[i+1] = accu * pscale + pflta[lfsr & 0x3F] + pfltb[lfsr >> 6 & 0x3F];
        
		bit   = lfsr >> 31;        dec &= ~0x0400;
        lfsr <<= 1;                dec |= inc & 0x0400;
        inc ^= bit  & 0x0400;      accu -= dec;
        lfsr ^= bit & 0x46000001;  accu += inc;
		leftOut[i+2] = accu * pscale + pflta[lfsr & 0x3F] + pfltb[lfsr >> 6 & 0x3F];
        
		bit   = lfsr >> 31;        dec &= ~0x0800;
        lfsr <<= 1;                dec |= inc & 0x0800;
        inc ^= bit  & 0x0800;      accu -= dec;
        lfsr ^= bit & 0x46000001;  accu += inc;
		leftOut[i+3] = accu * pscale + pflta[lfsr & 0x3F] + pfltb[lfsr >> 6 & 0x3F];
        
		bit   = lfsr >> 31;        dec &= ~0x0200;
        lfsr <<= 1;                dec |= inc & 0x0200;
        inc ^= bit  & 0x0200;      accu -= dec;
        lfsr ^= bit & 0x46000001;  accu += inc;
		leftOut[i+4] = accu * pscale + pflta[lfsr & 0x3F] + pfltb[lfsr >> 6 & 0x3F];
        
		bit   = lfsr >> 31;        dec &= ~0x0800;
        lfsr <<= 1;                dec |= inc & 0x0800;
        inc ^= bit  & 0x0800;      accu -= dec;
        lfsr ^= bit & 0x46000001;  accu += inc;
		leftOut[i+5] = accu * pscale + pflta[lfsr & 0x3F] + pfltb[lfsr >> 6 & 0x3F];
        
		bit   = lfsr >> 31;        dec &= ~0x0400;
        lfsr <<= 1;                dec |= inc & 0x0400;
        inc ^= bit  & 0x0400;      accu -= dec;
        lfsr ^= bit & 0x46000001;  accu += inc;
		leftOut[i+6] = accu * pscale + pflta[lfsr & 0x3F] + pfltb[lfsr >> 6 & 0x3F];
        
		bit   = lfsr >> 31;        dec &= ~0x0800;
        lfsr <<= 1;                dec |= inc & 0x0800;
        inc ^= bit  & 0x0800;      accu -= dec;
        lfsr ^= bit & 0x46000001;  accu += inc;
		leftOut[i+7] = accu * pscale + pflta[lfsr & 0x3F] + pfltb[lfsr >> 6 & 0x3F];
        
		bit   = lfsr >> 31;        dec &= ~0x0100;
        lfsr <<= 1;                dec |= inc & 0x0100;
        inc ^= bit  & 0x0100;      accu -= dec;
        lfsr ^= bit & 0x46000001;  accu += inc;
		leftOut[i+8] = accu * pscale + pflta[lfsr & 0x3F] + pfltb[lfsr >> 6 & 0x3F];
        
		bit   = lfsr >> 31;        dec &= ~0x0800;
        lfsr <<= 1;                dec |= inc & 0x0800;
        inc ^= bit  & 0x0800;      accu -= dec;
        lfsr ^= bit & 0x46000001;  accu += inc;
		leftOut[i+9] = accu * pscale + pflta[lfsr & 0x3F] + pfltb[lfsr >> 6 & 0x3F];
        
		bit   = lfsr >> 31;        dec &= ~0x0400;
        lfsr <<= 1;                dec |= inc & 0x0400;
        inc ^= bit  & 0x0400;      accu -= dec;
        lfsr ^= bit & 0x46000001;  accu += inc;
		leftOut[i+10] = accu * pscale + pflta[lfsr & 0x3F] + pfltb[lfsr >> 6 & 0x3F];
        
		bit   = lfsr >> 31;        dec &= ~0x0800;
        lfsr <<= 1;                dec |= inc & 0x0800;
        inc ^= bit  & 0x0800;      accu -= dec;
        lfsr ^= bit & 0x46000001;  accu += inc;
		leftOut[i+11] = accu * pscale + pflta[lfsr & 0x3F] + pfltb[lfsr >> 6 & 0x3F];
        
		bit   = lfsr >> 31;        dec &= ~0x0200;
        lfsr <<= 1;                dec |= inc & 0x0200;
        inc ^= bit  & 0x0200;      accu -= dec;
        lfsr ^= bit & 0x46000001;  accu += inc;
		leftOut[i+12] = accu * pscale + pflta[lfsr & 0x3F] + pfltb[lfsr >> 6 & 0x3F];
        
		bit   = lfsr >> 31;        dec &= ~0x0800;
        lfsr <<= 1;                dec |= inc & 0x0800;
        inc ^= bit  & 0x0800;      accu -= dec;
        lfsr ^= bit & 0x46000001;  accu += inc;
		leftOut[i+13] = accu * pscale + pflta[lfsr & 0x3F] + pfltb[lfsr >> 6 & 0x3F];
        
		bit   = lfsr >> 31;        dec &= ~0x0400;
        lfsr <<= 1;                dec |= inc & 0x0400;
        inc ^= bit  & 0x0400;      accu -= dec;
        lfsr ^= bit & 0x46000001;  accu += inc;
		leftOut[i+14] = accu * pscale + pflta[lfsr & 0x3F] + pfltb[lfsr >> 6 & 0x3F];
        
		bit   = lfsr >> 31;        dec &= ~0x0800;
        lfsr <<= 1;                dec |= inc & 0x0800;
        inc ^= bit  & 0x0800;      accu -= dec;
        lfsr ^= bit & 0x46000001;  accu += inc;
		leftOut[i+15] = accu * pscale + pflta[lfsr & 0x3F] + pfltb[lfsr >> 6 & 0x3F];
    }
    // dump copy
    for (var i = 0; i < rightOut.length; i += 4)
    {
        rightOut[i]   = leftOut[i];
        rightOut[i+1] = leftOut[i+1];
        rightOut[i+2] = leftOut[i+2];
        rightOut[i+3] = leftOut[i+3];
    }
};
*/
// pink noise generator with masking of individual sources
pinkmasked = function(e)
{
    var leftOut = e.outputBuffer.getChannelData(0);
//    var rightOut = e.outputBuffer.getChannelData(1);
    
    var bit;
    
	if(mask != newmask)		// idle slot once every 4k samples - use for building mask and offset
	{
		mask = newmask;
		offset = 0;
		for(var b=0x0800; b>0; b>>=1)
		{
			if(!(mask & b))
			{
				if(inc & b) offset -= 0x0800;
				else        offset += 0x0800;
			}
		}
	}
    
    if(noisy == 0)
    {
        for (var i = 0; i < leftOut.length; i += 2)
        {
//            rightOut[i]   = 0;
            leftOut[i]    = 0;
//            rightOut[i+1] = 0;
            leftOut[i+1]  = 0;
        }
        if(draw)
        	cx.putImageData(img,0,0);
        draw = 0;
        return;
    }
//	pflta[lfsr & 0x3F] + pfltb[lfsr >> 6 & 0x3F];
	
    if(mask & 0x1000)		// highest rate included?
    {
        for (var i = 0; i < leftOut.length; i += 16)
        {
            cnt = (cnt + 1) & 0x0FF;
           
            bit = lfsr >> 31;
            dec &= ~pseq[cnt];
            lfsr <<= 1;
            dec |= inc & pseq[cnt];
            inc ^= bit & (pseq[cnt] & mask);
            accu -= dec;
            lfsr ^= bit & 0x46000001;
            accu += inc;
            leftOut[i] = (accu + offset) * pscale + pflta[lfsr & 0x3F] + pfltb[lfsr >> 6 & 0x3F];
            
            bit   = lfsr >> 31;        dec &= ~0x0800;
            lfsr <<= 1;                dec |= inc & 0x0800;
            inc ^= bit  & (0x0800 & mask);  accu -= dec;
            lfsr ^= bit & 0x46000001;  accu += inc;
            leftOut[i+1] = (accu + offset) * pscale + pflta[lfsr & 0x3F] + pfltb[lfsr >> 6 & 0x3F];
            
            bit   = lfsr >> 31;        dec &= ~0x0400;
            lfsr <<= 1;                dec |= inc & 0x0400;
            inc ^= bit  & (0x0400 & mask);      accu -= dec;
            lfsr ^= bit & 0x46000001;  accu += inc;
            leftOut[i+2] = (accu + offset) * pscale + pflta[lfsr & 0x3F] + pfltb[lfsr >> 6 & 0x3F];
            
            bit   = lfsr >> 31;        dec &= ~0x0800;
            lfsr <<= 1;                dec |= inc & 0x0800;
            inc ^= bit  & (0x0800 & mask);      accu -= dec;
            lfsr ^= bit & 0x46000001;  accu += inc;
            leftOut[i+3] = (accu + offset) * pscale + pflta[lfsr & 0x3F] + pfltb[lfsr >> 6 & 0x3F];
            
            bit   = lfsr >> 31;        dec &= ~0x0200;
            lfsr <<= 1;                dec |= inc & 0x0200;
            inc ^= bit  & (0x0200 & mask);      accu -= dec;
            lfsr ^= bit & 0x46000001;  accu += inc;
            leftOut[i+4] = (accu + offset) * pscale + pflta[lfsr & 0x3F] + pfltb[lfsr >> 6 & 0x3F];
            
            bit   = lfsr >> 31;        dec &= ~0x0800;
            lfsr <<= 1;                dec |= inc & 0x0800;
            inc ^= bit  & (0x0800 & mask);      accu -= dec;
            lfsr ^= bit & 0x46000001;  accu += inc;
            leftOut[i+5] = (accu + offset) * pscale + pflta[lfsr & 0x3F] + pfltb[lfsr >> 6 & 0x3F];
            
            bit   = lfsr >> 31;        dec &= ~0x0400;
            lfsr <<= 1;                dec |= inc & 0x0400;
            inc ^= bit  & (0x0400 & mask);      accu -= dec;
            lfsr ^= bit & 0x46000001;  accu += inc;
            leftOut[i+6] = (accu + offset) * pscale + pflta[lfsr & 0x3F] + pfltb[lfsr >> 6 & 0x3F];
            
            bit   = lfsr >> 31;        dec &= ~0x0800;
            lfsr <<= 1;                dec |= inc & 0x0800;
            inc ^= bit  & (0x0800 & mask);      accu -= dec;
            lfsr ^= bit & 0x46000001;  accu += inc;
            leftOut[i+7] = (accu + offset) * pscale + pflta[lfsr & 0x3F] + pfltb[lfsr >> 6 & 0x3F];
            
            bit   = lfsr >> 31;        dec &= ~0x0100;
            lfsr <<= 1;                dec |= inc & 0x0100;
            inc ^= bit  & (0x0100 & mask);      accu -= dec;
            lfsr ^= bit & 0x46000001;  accu += inc;
            leftOut[i+8] = (accu + offset) * pscale + pflta[lfsr & 0x3F] + pfltb[lfsr >> 6 & 0x3F];
            
            bit   = lfsr >> 31;        dec &= ~0x0800;
            lfsr <<= 1;                dec |= inc & 0x0800;
            inc ^= bit  & (0x0800 & mask);      accu -= dec;
            lfsr ^= bit & 0x46000001;  accu += inc;
            leftOut[i+9] = (accu + offset) * pscale + pflta[lfsr & 0x3F] + pfltb[lfsr >> 6 & 0x3F];
            
            bit   = lfsr >> 31;        dec &= ~0x0400;
            lfsr <<= 1;                dec |= inc & 0x0400;
            inc ^= bit  & (0x0400 & mask);      accu -= dec;
            lfsr ^= bit & 0x46000001;  accu += inc;
            leftOut[i+10] = (accu + offset) * pscale + pflta[lfsr & 0x3F] + pfltb[lfsr >> 6 & 0x3F];
            
            bit   = lfsr >> 31;        dec &= ~0x0800;
            lfsr <<= 1;                dec |= inc & 0x0800;
            inc ^= bit  & (0x0800 & mask);      accu -= dec;
            lfsr ^= bit & 0x46000001;  accu += inc;
            leftOut[i+11] = (accu + offset) * pscale + pflta[lfsr & 0x3F] + pfltb[lfsr >> 6 & 0x3F];
            
            bit   = lfsr >> 31;        dec &= ~0x0200;
            lfsr <<= 1;                dec |= inc & 0x0200;
            inc ^= bit  & (0x0200 & mask);      accu -= dec;
            lfsr ^= bit & 0x46000001;  accu += inc;
            leftOut[i+12] = (accu + offset) * pscale + pflta[lfsr & 0x3F] + pfltb[lfsr >> 6 & 0x3F];
            
            bit   = lfsr >> 31;        dec &= ~0x0800;
            lfsr <<= 1;                dec |= inc & 0x0800;
            inc ^= bit  & (0x0800 & mask);      accu -= dec;
            lfsr ^= bit & 0x46000001;  accu += inc;
            leftOut[i+13] = (accu + offset) * pscale + pflta[lfsr & 0x3F] + pfltb[lfsr >> 6 & 0x3F];
            
            bit   = lfsr >> 31;        dec &= ~0x0400;
            lfsr <<= 1;                dec |= inc & 0x0400;
            inc ^= bit  & (0x0400 & mask);      accu -= dec;
            lfsr ^= bit & 0x46000001;  accu += inc;
            leftOut[i+14] = (accu + offset) * pscale + pflta[lfsr & 0x3F] + pfltb[lfsr >> 6 & 0x3F];
            
            bit   = lfsr >> 31;        dec &= ~0x0800;
            lfsr <<= 1;                dec |= inc & 0x0800;
            inc ^= bit  & (0x0800 & mask);      accu -= dec;
            lfsr ^= bit & 0x46000001;  accu += inc;
            leftOut[i+15] = (accu + offset) * pscale + pflta[lfsr & 0x3F] + pfltb[lfsr >> 6 & 0x3F];
        }
    }
    else
    {
        for (var i = 0; i < leftOut.length; i += 16)
        {
            cnt = (cnt + 1) & 0x0FF;
            
            if(cnt == 0 && mask != newmask)		// idle slot once every 4k samples - use for building mask and offset
            {
                mask = newmask;
                offset = 0;
                for(var b=0x0800; b>0; b>>=1)
                {
                    if(inc & b) offset += 0x0800;
                    else        offset -= 0x0800;
                }
            }
            bit = lfsr >> 31;
            dec &= ~pseq[cnt];
            lfsr <<= 1;
            dec |= inc & pseq[cnt];
            inc ^= bit & (pseq[cnt] & mask);
            accu -= dec;
            lfsr ^= bit & 0x46000001;
            accu += inc;
            leftOut[i] = (accu + offset) * pscale;
            
            bit   = lfsr >> 31;        dec &= ~0x0800;
            lfsr <<= 1;                dec |= inc & 0x0800;
            inc ^= bit  & (0x0800 & mask);  accu -= dec;
            lfsr ^= bit & 0x46000001;  accu += inc;
            leftOut[i+1] = (accu + offset) * pscale;
            
            bit   = lfsr >> 31;        dec &= ~0x0400;
            lfsr <<= 1;                dec |= inc & 0x0400;
            inc ^= bit  & (0x0400 & mask);      accu -= dec;
            lfsr ^= bit & 0x46000001;  accu += inc;
            leftOut[i+2] = (accu + offset) * pscale;
            
            bit   = lfsr >> 31;        dec &= ~0x0800;
            lfsr <<= 1;                dec |= inc & 0x0800;
            inc ^= bit  & (0x0800 & mask);      accu -= dec;
            lfsr ^= bit & 0x46000001;  accu += inc;
            leftOut[i+3] = (accu + offset) * pscale;
            
            bit   = lfsr >> 31;        dec &= ~0x0200;
            lfsr <<= 1;                dec |= inc & 0x0200;
            inc ^= bit  & (0x0200 & mask);      accu -= dec;
            lfsr ^= bit & 0x46000001;  accu += inc;
            leftOut[i+4] = (accu + offset) * pscale;
            
            bit   = lfsr >> 31;        dec &= ~0x0800;
            lfsr <<= 1;                dec |= inc & 0x0800;
            inc ^= bit  & (0x0800 & mask);      accu -= dec;
            lfsr ^= bit & 0x46000001;  accu += inc;
            leftOut[i+5] = (accu + offset) * pscale;
            
            bit   = lfsr >> 31;        dec &= ~0x0400;
            lfsr <<= 1;                dec |= inc & 0x0400;
            inc ^= bit  & (0x0400 & mask);      accu -= dec;
            lfsr ^= bit & 0x46000001;  accu += inc;
            leftOut[i+6] = (accu + offset) * pscale;
            
            bit   = lfsr >> 31;        dec &= ~0x0800;
            lfsr <<= 1;                dec |= inc & 0x0800;
            inc ^= bit  & (0x0800 & mask);      accu -= dec;
            lfsr ^= bit & 0x46000001;  accu += inc;
            leftOut[i+7] = (accu + offset) * pscale;
            
            bit   = lfsr >> 31;        dec &= ~0x0100;
            lfsr <<= 1;                dec |= inc & 0x0100;
            inc ^= bit  & (0x0100 & mask);      accu -= dec;
            lfsr ^= bit & 0x46000001;  accu += inc;
            leftOut[i+8] = (accu + offset) * pscale;
            
            bit   = lfsr >> 31;        dec &= ~0x0800;
            lfsr <<= 1;                dec |= inc & 0x0800;
            inc ^= bit  & (0x0800 & mask);      accu -= dec;
            lfsr ^= bit & 0x46000001;  accu += inc;
            leftOut[i+9] = (accu + offset) * pscale;
            
            bit   = lfsr >> 31;        dec &= ~0x0400;
            lfsr <<= 1;                dec |= inc & 0x0400;
            inc ^= bit  & (0x0400 & mask);      accu -= dec;
            lfsr ^= bit & 0x46000001;  accu += inc;
            leftOut[i+10] = (accu + offset) * pscale;
            
            bit   = lfsr >> 31;        dec &= ~0x0800;
            lfsr <<= 1;                dec |= inc & 0x0800;
            inc ^= bit  & (0x0800 & mask);      accu -= dec;
            lfsr ^= bit & 0x46000001;  accu += inc;
            leftOut[i+11] = (accu + offset) * pscale;
            
            bit   = lfsr >> 31;        dec &= ~0x0200;
            lfsr <<= 1;                dec |= inc & 0x0200;
            inc ^= bit  & (0x0200 & mask);      accu -= dec;
            lfsr ^= bit & 0x46000001;  accu += inc;
            leftOut[i+12] = (accu + offset) * pscale;
            
            bit   = lfsr >> 31;        dec &= ~0x0800;
            lfsr <<= 1;                dec |= inc & 0x0800;
            inc ^= bit  & (0x0800 & mask);      accu -= dec;
            lfsr ^= bit & 0x46000001;  accu += inc;
            leftOut[i+13] = (accu + offset) * pscale;
            
            bit   = lfsr >> 31;        dec &= ~0x0400;
            lfsr <<= 1;                dec |= inc & 0x0400;
            inc ^= bit  & (0x0400 & mask);      accu -= dec;
            lfsr ^= bit & 0x46000001;  accu += inc;
            leftOut[i+14] = (accu + offset) * pscale;
            
            bit   = lfsr >> 31;        dec &= ~0x0800;
            lfsr <<= 1;                dec |= inc & 0x0800;
            inc ^= bit  & (0x0800 & mask);      accu -= dec;
            lfsr ^= bit & 0x46000001;  accu += inc;
            leftOut[i+15] = (accu + offset) * pscale;
        }
    }
    // dump copy
/*
    for (var i = 0; i < rightOut.length; i += 4)
    {
        rightOut[i]   = leftOut[i];
        rightOut[i+1] = leftOut[i+1];
        rightOut[i+2] = leftOut[i+2];
        rightOut[i+3] = leftOut[i+3];
    }
*/    
    if(draw)
    {
    
    	cx.putImageData(img,0,0);
  		cx.save();
  		cx.globalAlpha = 0.6;
    	var nb = analyzer.frequencyBinCount;
//    	nb = nb * 0.5;
//    	var fd = new Uint8Array(analyzer.frequencyBinCount);
     	analyzer.getByteFrequencyData(fd);
        cx.strokeStyle = '#C000C0';
    	cx.lineWidth = 1; 
    	cx.beginPath();
       
		var pxf = cv.width/Math.log(22050);
		var lgf = 22050/nb;
		var dx = Math.log(lgf) * pxf;
		var yf = (cv.height-10)/256;
		var ymax = cv.height - 10;
        cx.moveTo(dx,ymax-yf*(fd[1])); 
        for(var i=2; i<nb; i++)
    	{
    		var dx = Math.log(i * lgf) * pxf;
    		var dy = ymax-yf*(fd[i]);
    		cx.lineTo(dx,dy);
    	}
        cx.stroke();  
        cx.restore();  
    }
};

// initial test
white = function(e) {
    //  var leftIn = e.inputBuffer.getChannelData(0);
    //  var rightIn = e.inputBuffer.getChannelData(1);
    var leftOut = e.outputBuffer.getChannelData(0);
    var rightOut = e.outputBuffer.getChannelData(1);
    
    for (var i = 0; i < leftOut.length; i++)
    {
        var n = nreg>>31;
        nreg = (nreg<<1) ^ (0x46000001 & n);
        var f = 0.3;
        if(n) f = -f;
        leftOut[i] = f;
        rightOut[i] = f;
    }
};


window.AudioContext = window.AudioContext||window.webkitAudioContext;
// Note that any computer generated sequence is periodic, as a computer can have only a limited number of states. 
var context = new AudioContext();
var processor = context.createScriptProcessor(2048,1,1);
processor.onaudioprocess = pinkmasked;

// correct firefox' different analyzer
var foxhack = ( navigator.userAgent.match(/(Firefox)/g) ? -6 : 0 );

analyzer = context.createAnalyser(2048);
analyzer.smoothingTimeConstant = 0.9;
analyzer.maxDecibels = foxhack-22;
analyzer.minDecibels = foxhack-106;
var fd = new Uint8Array(analyzer.frequencyBinCount);

// check if we run on iOS and need a dummy oscillator
var iOS = ( navigator.userAgent.match(/(iPad|iPhone|iPod)/g) ? true : false );
//var iOS  = true;

if(iOS)
{
	var osc = context.createOscillator();
	osc.type = 0; 				// sine
	osc.connect(processor);		// dummy
}

processor.connect(analyzer);
analyzer.connect(context.destination);

function noisectl()
{
    noisy = document.sources.elements[13].checked;
    if(noisy) 
    {
    	draw = 1;
	}
	
    if(iOS)
    {
    	if(noisy) osc.noteOn(0);
    	else      osc.noteOff(0);
    }
}

document.write('<canvas id="c3" width="850" height="400"></canvas>\n');

var cv = document.getElementById('c3');
var cx = cv.getContext('2d');
cx.font         = 'Normal 11px Sans-Serif';

hz = function(f)
{
    var z = 0;
    var im = 0;
    var re = pfir[0];
    for(var i=1; i<8; i++)
    {
        z -= f;
        im += Math.sin(z) * pfir[i];
        re += Math.cos(z) * pfir[i];
    }
    return im * im + re * re * 0.67;
}

mag = function(x,n)
{
    if(x < 0) return  0;
    if(n==1) 
    {
//    	return hz(x);
  		x = hz(x);
  		x = 8.68588963806504 * Math.log(x);
        return x;
    } 
    x = Math.sin(n*x)/(n*Math.sin(x));
    x = 8.68588963806504 * Math.log(Math.pow(n,0.5) * x * x);
    return x;
}

hf = function(f)
{
    if(f < 0) return  0;
    var x = 0;
    
    for(var i=0; i<12; i++)
    {
        if(newmask & (1<<i))
        {
            var n = 0x01000>>i;
            var b = Math.sin(n*f)/(n*Math.sin(f));
            b *= b;
            x += n * b * b;
        }
    }
    if(newmask & 0x1000)
    {
        var h = hz(f);
        x += h * h;
    }
    
    if(x <= 0) return 0;
    x = 8.68588963806504 * Math.log(x) * 0.5;
    return x;
}
function fplot()
{
	draw = 0;
    cx.clearRect(0,0,cv.width,cv.height);
    // paint log grid, x axis first
    // should start real low at about 5 Hz
    // so 1 2 3 4 5 6 7 8 9 10 20 30 40 50 60 70 80 90 100 200
    // or 1       5         10          50             100
    //
    var inc = 1;
    var ii  = 0;
    cx.strokeStyle = '#000000';
    cx.lineWidth = 1;
    cx.textAlign = 'center';
    var x = 0;
    var y = 0;
    //        cx.textBaseline = 'middle';
    for(var i = 1; i<=20000; i+=inc)
    {
        x = Math.log(i)*cv.width/Math.log(22050);
        
        if((ii == 9) || (ii == 1) || (ii == 4) || (i == 1))
        {
            if(i < 1000)
                cx.fillText(i,x+2,cv.height);
            else
            {
            	if(i > 15000) x-=4;
                cx.fillText(i/1000 + "K",x,cv.height);
            }
            cx.strokeStyle = '#000000';
        }
        else cx.strokeStyle = '#C0C0C0';
        x = Math.round(x)+0.5;
        cx.beginPath();
        cx.moveTo(x,0);
        cx.lineTo(x,cv.height-10);
        cx.stroke();
        ii+=1;
        if(ii >= 10) 
        {
            inc *= 10;
            ii = 1;
        }
    }
    // now the y grid
    cx.strokeStyle = '#000000';
    for(var i=-48; i<=36; i+=6)
    {
        var pos = 1-(48.0+i)/84.0;
        y = (cv.height-10)*pos; 
        y = Math.round(y)+0.5;
        var db = i - 36;
        if(db > -80)
        {
        if(db < -36)
        	cx.fillText(db + " dB",cv.width*0.035,y+10);
        else
        	cx.fillText(db + " dB",cv.width*0.955,y+10);
        }
        //        	else	cx.fillText(i,cv.width/2-6,y+9);
        cx.beginPath();
        cx.moveTo(0,y);
        //       		cx.lineTo(x,y);
        cx.lineTo(cv.width,y);
        cx.stroke();
        
    }
    cx.save();
    cx.rect(0,0,cv.width,cv.height-10);
    cx.stroke();
    cx.clip();
    cx.lineJoin = 'round';
    
    hplot = function()
    {
        cx.strokeStyle = '#000000';
        cx.beginPath();
        y = (42	 - hf(3.1415926/44100)) * (1/84) * (cv.height-10);
        cx.moveTo(0,y);
        cx.lineWidth = 1;
        for(var i = 1; i<cv.width; i++)
        {
        	x = i;
        	// now frequency(x) = 1 for 0, 22050 for width, 
        	frq = Math.pow(22050,i/cv.width);
        	y =  (42 - hf(3.1415926*frq/44100)) * (1/84) * (cv.height-10);
        	cx.lineTo(x,y);
        } 
		cx.stroke();
		cx.lineTo(cv.width,cv.height-10);
		cx.lineTo(0,cv.height); 
		cx.globalAlpha = 0.1;		
		cx.fillStyle = '#000030';
		cx.fill();
		cx.globalAlpha = 1;
    }
    
	var n2col = ["#C000FF","#8000FF","#0000FF","#0070FF","#00C0FF","#00E0C0","#00F000",
			 "#C0E000","#FF8000","#FF0000","#C00000","#800000","#000000"];
    plot = function(i)
    {
		cx.strokeStyle = n2col[i];
		var n =  1<<i;
        cx.beginPath();
        y = (42 - mag(3.1415926/44100,n)) * (1/84) * (cv.height-10);
        cx.moveTo(0,y);
        cx.lineWidth = 1;
        for(var i = 1; i<cv.width; i++)
        {
        	x = i;
        	// now frequency(x) = 1 for 0, 22050 for width, 
        	frq = Math.pow(22050,i/cv.width);
        	y =  (42 - mag(3.1415926*frq/44100,n)) * (1/84) * (cv.height-10);
        	
        	cx.lineTo(x,y);
        } 
		cx.stroke();
    }
    hplot();
    for(var i=0; i<=12; i++)	if(newmask & (0x01000>>i)) plot(i); 
    cx.restore();
    img = cx.getImageData(0,0,cv.width,cv.height);
    draw = 1;
}
fplot();

</script>