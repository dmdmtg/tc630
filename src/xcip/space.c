/*			MEMORY SPACE ROUTINES		 */


#include "cip.h"

extern short		noSpace;

#ifdef DMDTERM

#ifdef DMD5620
#  include "setup.h"
#endif

#ifdef DMD630

typedef struct aheader {
	struct aheader *	next;
	long *			proc;
} aheader;

#undef BUSY
#define BUSY 3l
#define testbusy(p) ((long)p & BUSY)
#define clearbusy(p) (aheader *)((long)p & ~BUSY)

#endif


#ifdef DMD5620

#include <sa.h>

#undef spl7
#undef splx
#include <pandora.h>

typedef union aheader {
	struct {
		union aheader *	Uptr;
		char *		Uproc;
	} u;
	union aheader *	next;
	int		dummy[1];
	int		calloc;	/* calloc clears an array of integers */
} aheader;

#undef BUSY
#define BUSY 1
#define testbusy(p) ((long)(p)&BUSY)
#define clearbusy(p) (aheader *)((long)(p)&~BUSY)

#endif

#ifdef DMD630
#undef min
#define min(A,B) ((A) < (B) ? (A) : (B))
#endif

extern Point		PointCurr;
extern Texture16	textCursor;
extern int		backspaceOneWord();


	long
spaceLeft()
{

#ifdef DMD5620
	register long		sum;
	register aheader *	p;
	register aheader *	q;
	static aheader *	startp;
	static aheader *	endp = (aheader *)0;

	if (!endp) {
		/* Make calculations just once. */
		startp = (aheader *) ((int *)Sys[163]);
		endp = (aheader *) (( (maxaddr[VALMAXADDR] + 
		    (int)Sys[163]) / 2) & 0xfffffffc );
	}

	sum = (long) endp - (long) startp;
	p = startp;
	q = clearbusy(p->next);

	while( p < q ) {
		if( testbusy(p->next) ) {
			sum -= (long) q - (long) p;
		}
		p = q;
		q = clearbusy(p->next);
	}
	return sum;
#endif

#ifdef DMD630
	register long		sum;
	register aheader *	p;
	register aheader *	q;
	/* Calculate total available alloc space which includes the
	   space above the alloclevel up to the either gclevel or
	   alloclimit, which ever is lower.  One could do a "gccompact()" 
	   to get more alloc space, but not necessary here.
        */
	sum = min( (long)gclevel , (long)alloclimit ) - (long) memstartp;

	/* Run through memory blocks in alloc space subtracting from
	   "sum" those that are currently busy.
	*/
	for( p = (aheader*)memstartp;  p <= (aheader*)alloclevel;  p = q ) {
		q = clearbusy(p->next);
		if( testbusy(p->next) )  sum -= (long) q - (long) p;
	}
	return sum;
#endif
}

#endif  /* DMDTERM */


/* This routine allocates memory.  */

  char *
getSpace (numbytes)
  int numbytes;
{
  char *b;		/* Pointer to allocated memory */

  if (numbytes < 8) numbytes = 8;
  if ((b = alloc (numbytes)) == (char *) NULL) {
    outOfSpace ();
    return (b);
  }
  return (b);
}


/* This routine prints an error message telling the user xcip is out of space.*/

outOfSpace()
{
  newMessage("Out of Storage - PUT and QUIT");
  sleep(240);
  noSpace = 1;
}
