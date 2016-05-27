/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)myget.c	1.1.1.2 88/02/10 17:14:01";

/* includes */
#include "jf.h"

/* defines */
#define  SFSIZE(m,n)   (sizeof(Sfontchar)*(n-m+1))

/* locals */
static FILE *inf;

/* procedures */


static
cget()
{
           return(getc(inf));
}

Font *
gtfont(s)
      char *s;
  {
     Font *f, *ifont();
      if ((inf = fopen(s,"r")) == (FILE *)0)
  {    return((Font *)0);
   }
	f = ifont(cget);
	fclose(inf);
	return(f);
   }

static (*ifn)();

Font *
ifont(fn)
	int (*fn)();
{
	unsigned short n2;
	short n;
	short k,i;
	int ofset;
	Font *convert();
	Sfont *reconvert();
	register Sfont *f;
	register Bitmap *b;
	char *temp2;
	Sfont *temp,*sfptr;
        Sfont sftemp;
	Sfontchar *infopt;
	Font *fpt;
	unsigned short *ps,*pds;
	int ctr;
	/*    let temp point to sfonttemp  */
	temp = &sftemp;
	
	ifn = fn;
	/*     read in the header information    */
	ninch(NSFHEAD,&temp);
	/*     allocate space  for the header and the sfontchars */
	if((f = (Sfont *) alloc(sizeof(Sfont) + SFSIZE(sftemp.m,sftemp.n-1)))== (Sfont *)0)
	  return((Font *)f);
	/*  into the space allocated put header info gotten in sftemp */
	*f = sftemp;
	if(f->n > 127)
	/* allocate spaces for non-ascii    */
	{
       f->nonascii = ((unsigned short *) alloc(k = sizeof(short) * (f->n -127)));
	temp2 = (char *)f->nonascii;
	ninch(k,&temp2);
	pter = (Temp *) alloc(sizeof(Temp));
        pter->nonascii = ((unsigned short *) alloc(k = sizeof(short) * (f->n -127)));
	pds = f->nonascii;
	ps = pter->nonascii;
	for(ctr=0;ctr<f->n-127;ctr++)
		*ps++ = *pds++;
	pter->ptsize = f->ptsize;
	free(f->nonascii);
	}
	n = (sizeof(short) * f->nwords);
	gcalloc(n,&f->bits);
	/*  get Temp structure set up for info to be held until reconvert  */

	temp2 = (char *)f->info;
	ninch(SFSIZE(f->m,f->n),&temp2);
	ninch(n,&f->bits);
	fpt = convert(f,pter);
	return(fpt);
 }   
static
ninch(n, base)
	register n;
	register char **base;
{
	register x, i;

	i = 0;
	do {
		x = (*ifn)();
		(*base)[i++] = x;
		if(x == -1)
			return(1);
	} while (--n > 0);
	return(0);
}

	/*  Convert  takes a pointer to a Sfont structure returning a Font pointer*/

Font *
convert(f,pter)
	Sfont *f;
	Temp  *pter;
{
	Bitmap *tb;
	Bitmap *tempmap;
	short *ps, *pds;
	char ch; 
	Point p;
	Font *fpt;
	int maxcx,sumcx,i,y,line,ctr,swidth,maxdeltay,maxdeltax;
	if((fpt = (Font *) alloc(sizeof(Font) + ((f->n)+1) *sizeof(Fontchar))) == (Font *)0)
		return(fpt);

	fpt->n = f->n+1;
	/* printf("fpt.nn   %d   \n",fpt->n); */
	fpt->height = f->height;
	fpt->ascent = f->ptroffy;
	fpt->unused = (long)pter;
	maxcx=0;
	sumcx=0;
	ch = 'n';
	
	for(i=0;i<=f->n-f->m;i++)
		{
		 sumcx += f->info[i].cornerx;
		 if (f->info[i].cornerx > maxcx)
		 maxcx = f->info[i].cornerx;
		}
	 tb = balloc(Rect(0,0,(sumcx+31)&0xFFE0,f->height ));
	fpt->bits = tb;

/*   allocating  a temporary bitmap to ensure alignment during transfer  */

	if ((tempmap = balloc(Rect(0,0,maxcx ,f->height)))==(Bitmap *)0)
		return((Font *)tempmap);
	p.x=0;
	p.y=0;
	/* printf(" f.mm   %d  \n",f->m);
	printf(" f.nn   %d  \n",f->n); */
	for(i=0;i<=f->n-f->m;i++)
		{
		 rectf(tempmap,Rect(0,0,maxcx ,f->height),F_STORE);

	/* set up pointers to copy bits from sfont storage of bits to tempmap  */

		 ps =  f->info[i].offset + f->bits;
		 swidth = (f->info[i].cornerx +15)/16;
		for(line=0;line<f->info[i].cornery;line++)
		  { /* JRG - * 2 removed */
		   pds = (short *) tempmap->base + line * (sizeof(Word) / 
			sizeof(short)) * tempmap->width;
		   for(ctr=0;ctr<swidth;ctr++)
		     *pds++ = *ps++;
		   }
		p.y =f->info[i].deltay; 
		bitblt(tempmap,Rect(0,0,f->info[i].cornerx,f->info[i].cornery),
			fpt->bits,p,F_STORE);
		fpt->info[i + f->m].x = p.x;
		fpt->info[i + f->m].top = f->info[i].deltay;
		fpt->info[i + f->m].bottom = f->info[i].deltay + f->info[i].cornery;  
		fpt->info[i + f->m].left = (f->info[i].deltax - f->ptroffx);
		fpt->info[i + f->m].width = f->info[i].width;
		p.x += f->info[i].cornerx; 
		}
		fpt->info[i + f->m].x = p.x;

	/*   Below :  put in blank    */

		if (fpt->info[32].width == 0)
			fpt->info[32].width = ((fpt->info)+ch)->width;
	bfree(tempmap);
	return(fpt);
}

/*    reconvert  takes a pointer to a Font structure returning a pointer to its
                                corresponding Sfont structure  */  



Sfont *
	reconvert(fpt)
	Font *fpt;
{
	int ctr,line,cornx,corny,i,left,right,swidth,maxcornx,ofset;
	Rectangle R,rs,rsupport();
	Sfont *sf;
	Bitmap *pter,*tempmap;
	Point p;
	Temp *tmp;
	char ch;
	short *ps, *pds,minleft;
  
 
	ctr=i=0;
	ch = 'm';
	tmp = (Temp *)fpt->unused;
	while ((fpt->info[i].x == fpt->info[i+1].x) && (fpt->info[i].width == 0))
		i++;
	
	if((sf = (Sfont *) alloc(sizeof(Sfont) + (fpt->n-i+1) * sizeof(Sfontchar))) == 
		(Sfont *) 0) return(sf);
	/*     Set  up  header information  of the Sfont   */

	sf->m = i;
	sf->n = fpt->n-1;	
        sf->height = fpt->height;
	sf->ptroffy = fpt->ascent;
	if(tmp == 0)
	{
		sf->ptsize = muldiv(((fpt->info)+ch)->width,72,100);
		sf->nonascii = 0;
	}
	else
	{	
		sf->ptsize = tmp->ptsize;
		sf->nonascii = tmp->nonascii;
	}
	maxcornx = 0;
	left = fpt->info[sf->m].x;
	right= fpt->info[sf->m+1].x;
	line = left - right;
	pter = balloc(Rect(0,0,line,fpt->height));

	/*   Below using rsupport to determine how much space needed to store bits*/

	sf->nwords = 1;
	for(i=sf->m;i<=sf->n;i++)
		{
		/*   find   minleft  to determine ptroffx  */
		 if(SIGNED(fpt->info[i+1].left) < minleft)
			minleft = SIGNED(fpt->info[i+1].left);
		 right=fpt->info[i+1].x;
		 if(line < (right-left))
		  {
		    line=right-left;
		    bfree(pter);
		    pter = balloc(Rect(0,0,line,fpt->height));
		   }	
		 if (left != right)
		 {  R.origin.x = left;
		    R.origin.y = 0;
		    R.corner.x = right;
		    R.corner.y = fpt->height;
		  }
		 else
		  {  R.origin.x = R.origin.y = R.corner.x = R.corner.y = 0;
			continue;    
		   }
		 rs = rsupport(fpt->bits,R,pter);
		 cornx = rs.corner.x - rs.origin.x;
		 corny = rs.corner.y - rs.origin.y;
		 if(cornx > maxcornx) maxcornx = cornx;                            
		 sf->nwords += (corny * ((cornx+15)/16));
		 left=right;
		}
	sf->ptroffx = 0 - minleft;
	tempmap = balloc(Rect(0,0,maxcornx,fpt->height));
	gcalloc((sizeof(short) * sf-> nwords),&sf->bits);
	left = fpt->info[sf->m].x;
	ofset = 1;
	for(i=sf->m;i<=sf->n;i++)
		{
		 rectf(tempmap,Rect(0,0,maxcornx ,fpt->height),F_STORE);
		 right=fpt->info[i+1].x;
		 if (left != right)
		 {  R.origin.x = left;
		    R.origin.y = 0;
		    R.corner.x = right;
		    R.corner.y = fpt->height;
		  }
		 else
		  {  R.origin.x = R.origin.y = R.corner.x = R.corner.y = 0;
		  sf->info[i-sf->m].width = fpt->info[i].width;
			continue; 
		   }
		 rs = rsupport(fpt->bits,R,pter);
		 cornx = rs.corner.x - rs.origin.x;
		 corny = rs.corner.y - rs.origin.y;

		/*   transfer  bits from Bitmap to tempmap by using bitblt  */

		 bitblt(fpt->bits,rs,tempmap,Pt(0,0),F_STORE);
		 left=right;
                sf->info[i-sf->m].offset = ofset;

		/*   transfer  bits  from tempmap  to storage area for sfont bits */

		pds =  sf->info[i-sf->m].offset + sf->bits;
		swidth = (cornx + 15)/16;
		for (line = 0; line < corny; line++)
		  { /* JRG - *2 removed */
		ps = (short *) tempmap->base + line * (sizeof(Word) /
			sizeof(short)) *tempmap->width;
		   for(ctr=0;ctr<swidth;ctr++)
			*pds++ = *ps++;
		   }
		  sf->info[i-sf->m].width = fpt->info[i].width;
			ofset += (corny * ((cornx+15)/16));			
		  sf->info[i-sf->m].cornerx = cornx;
		  sf->info[i-sf->m].cornery = corny;
		  sf->info[i-sf->m].deltax = (rs.origin.x - R.origin.x);
		  sf->info[i-sf->m].deltax +=  sf->ptroffx + SIGNED(fpt->info[i].left);
		  sf->info[i-sf->m].deltay = rs.origin.y - R.origin.y;
		}
	bfree(tempmap);
	bfree(pter);
	return(sf);
}



static FILE *outfile;
static int (*ouch)();
static
cput(c)
char c;
	{
	 return(putc(c,outfile));
	}

  /*   using  putfont  to move info from  sfont structure  to file  */

putfont(sf,s)
	char *s;
	Sfont *sf;
{
	int n;
	if ((outfile = fopen(s,"w")) == (FILE *)0)
		return(-1);
	 n = soutfont(sf,cput);
	 fclose(outfile);
	 free(sf->nonascii);
	 free(sf);
	 return(n);
}



soutfont(sf,fn)
	Sfont *sf;
	int (*fn)();
{
	int n;
	char *temp;
	ouch = fn;
	temp = (char *)sf;
	if(nouch(NSFHEAD,&temp))
	    return(-1);
	if (sf->n>127)
	{
	n = (sizeof(short) * (sf->n - 127));
	temp = (char *)sf->nonascii;
	if(nouch(n,&temp))
	   return(-1);
	}
	temp = (char *)sf->info;
	if(nouch(sizeof(Sfontchar)*(sf->n-sf->m +1),&temp))
	   return(-1);
	n = (sizeof(short) * sf->nwords);
	if(nouch(n,&sf->bits))
	   return(-1); 
}
static
nouch(n,base)
	register n;
	register char **base;
{
	register i = 0;
	do {
		if((*ouch)((*base)[i++]) == -1)
			return(-1);
	} while (--n > 0);
	return(0);
}

  /*   routine  containing  printf  for Sfont  in case  bug occurs   */

sfontinfo(f)
	Sfont *f;
{
	int i;
	short n2;
      /*  print header   information  */       	
	printf("%d\n",f->height); 
	printf("%d\n",f->nwords);
	printf("%d     %d\n",f->ptroffx,f->ptroffy);
	printf("%d\n",f->ptsize);     
	printf("ascii cornerx cornery deltax deltay width offset\n");
	for(i=0;i<=f->n-f->m;i++)
		{
		printf("  %d     %d      %d",i+f->m,f->info[i].cornerx,f->info[i].cornery);
		printf("     %d     %d  ",f->info[i].deltax,f->info[i].deltay);
		printf("      %d       %d\n",f->info[i].width,f->info[i].offset);
		}  
	  	for(i=0;i<f->n-127;i++)
		{  n2 = *(f->nonascii+i);
		   printf("%x	%c%c\n",n2,(n2>>8)&255,n2&255);
		}
}  

