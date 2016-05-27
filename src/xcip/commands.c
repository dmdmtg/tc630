#ifndef X11
#ifndef BRAM
#include "setup.h"
#endif
#endif

#include "cip.h"

#ifdef DMD5620
#include <pandora.h>
#endif

extern char *Pgm_name;
extern char *Jpic_pgm;

struct macro *macroList;
int nextMacroName;
Rectangle macroBB();

char	filename[MAXNAMESIZE+10];

extern int currentBrush, copyFlag, gridState, buttonState;
extern int videoState;
extern int editDepth;
extern char c;
extern char fontString[];
extern Rectangle brushes[];
extern Rectangle BBpic;
extern struct thing *readPic();
extern void StringSize();
extern void undo_clear();
extern Point jString ();
extern Point translate();


char tempString[MAXTEXT];
char fixedString[MAXTEXT];

/* Fixes string by placing backslashes in front of characters
 * that give problems in pic and troff (i.e., double quote
 * and backslash).  Exceptions are "\(" and "\*" which are passed thru
 * untouched to allow users to get things like bullits ("\(bu") and
 * trademark ("\*(Tm").
 */
	char *
addBackslashes( s )
	register char *		s;
{
	register int	i,j;

	for( j=i=0; s[i]!='\0'; i++ ) {
		if( s[i] == '"' ) {
			fixedString[j++] = '\\';
		} else if( s[i] == '\\' ) {
			if( (s[i+1] == '\\') && (
				(s[i+2] == '(') || (s[i+2] == '*') ) ) {
				/* Special cases of \\( turn to \\\\( 
				 * and \\* turn to \\\\*
				 */
				fixedString[j++] = '\\';
				fixedString[j++] = '\\';
				fixedString[j++] = '\\';
				i++;
			} else if ( (s[i+1] != '(') && (s[i+1] != '*') ) {
				/* Normal case - add 3 extra backslashes */
				fixedString[j++] = '\\';
				fixedString[j++] = '\\';
				fixedString[j++] = '\\';
			}
		}
		fixedString[j++] = s[i];
	}

	fixedString[j++] = '\0';
	return( fixedString );
}


  int
numLines(s)
  register char *s;
{
  register int i;
  register int num = 1;

  if (s[0]=='\0') {
    return (0);
  } else {
    for(i=0; s[i]!='\0'; i++)
      if ( s[i] == '\r' )
	num++;
    return (num);
  }
}


putText(fp,h)		/* recursive */
	FILE *fp;
	register struct thing *h;
{
	register struct thing *t;
        register int q,v,start;
	int 	hi;
        char *	s;
	short	spacing;
	Point	w;

	if ((t = h) != TNULL) {
	    do {
	    	if (t->type == TEXT) {
	    	    s = t->otherValues.text.s;
	    	    if ( numLines(s) > 1 ) {
	    		s = addBackslashes(s);
	    		spacing = t->otherValues.text.spacing +
				  fontheight(t->otherValues.text.f->f);
			w = sub(t->bb.corner, t->bb.origin);
	    		fprintf(fp, "define t%d |\n", 
	    			t->otherValues.text.outName);
	    		fprintf(fp, 
	    		   "[ box invis ht %d wid %d with .sw at 0,0\n",
			   w.y, w.x );

	    		hi = w.y - (fontheight(t->otherValues.text.f->f)>>1) -
				   (fontheight(t->otherValues.text.f->f)>>3);
	    		q = 0; v = 0; start = 0;
	    		while(s[q] != '\0') {
            	    		for(q=start; (s[q]!='\r')&&(s[q]!='\0');
	    					q++)
                	    		tempString[v++] = s[q];
            	    		tempString[v] = '\0';

	    			font2string(t->otherValues.text.f->num);
	    			if( strlen(fontString) == 2 )
	    			  fprintf(fp, "\"\\f(%s", fontString );
	    			else
	    			  fprintf(fp, "\"\\f%s", fontString );

	    		        switch (t->otherValues.text.just) {
	    		        case CENTER:
	                     fprintf(fp,"\\s%d\\&%s\\fP\\s0\" at %d,%d\n",
	    				t->otherValues.text.f->ps,
	    	                        tempString, (w.x>>1), hi );
	    			      break;
	    		        case LEFTJUST:
                             fprintf(fp,"\\s%d\\&%s\\fP\\s0\" at %d,%d %s\n",
	    				t->otherValues.text.f->ps,
	    	                        tempString, 0, hi, "ljust" );
	    			      break;
	    			case RIGHTJUST:
                             fprintf(fp,"\\s%d\\&%s\\fP\\s0\" at %d,%d %s\n",
	    				t->otherValues.text.f->ps,
	    	                        tempString, w.x, hi, "rjust" );
	    			      break;
	    			}

                    	    	start = q+1;
                    	    	hi = hi - spacing; 
	    	    		v = 0;
                       	    }
                        fprintf(fp, "] |\n\n");
	    	    }
       	    	} else if (t->type == MACRO) {
	    		putText(fp,t->otherValues.list->parts);
	    	}
       	    	t = t->next;
     	    } while(t != h);
   	}
}


doPut()
{
    register struct thing *	t;
    FILE *			fp;
    struct macro *		m;
    int 			saveB;
    int 			r;
    char 			cp2backup[300];

    saveB = buttonState;

    initMessage();

    if( firstThing == TNULL ) {
        moreMessage("Nothing to write out!");
        return;
    }

    changeButtons(DObuttons);
    moreMessage("Put file: ");
    r = getMessage(filename);

    if( (r==1) || (r==2) || (strlen(filename) == 0) ) {
        newMessage("*** put file cancelled ***");
    	changeButtons(saveB);
        return;
    }

    cursswitch(&hourglass);
    changeButtons(BLANKbuttons);

    if( access(filename,4) == 0 ) {
        /* If file exists already, copy it to a backup file in 
           user's home directory.   */
        sprintf(cp2backup, "/bin/cp %s $HOME/%s.backup", 
            filename, Pgm_name);

#ifdef DMDTERM
        /* Check for more than 60 characters - a limit imposed by
           the jx command. */
        if( strlen(cp2backup) >= 60 ) {
            newMessage("Warning: file path name is too long!");
            moreMessage("\nIt may cause xcip to abort on machines");
            moreMessage("\nsuch as VAXes.  Continue anyway?");
            if( !RUsure() ) {
                newMessage("*** put file cancelled ***");
    		cursSwitch();
                return;
            }
        }
#endif

        if( (fp=popen(cp2backup,"r")) == (FILE *) NULL ) {
            fileError("Failed to backup the", filename );
        } else {
            getc(fp);  /* forces DMD to wait for cp completion. */
            newMessage("Previous file copied to ");
            moreMessage( Pgm_name );
            moreMessage( ".backup in\nyour home directory." );
            pclose(fp);
        }
    }

    if ((fp = fopen(filename,"w")) == (FILE *) NULL) {
        fileError("open",filename);
    	cursSwitch();
        return;
    }

    fprintf(fp,".nf\n.PS\nscale=100\n");
    findBBpic( firstThing );

    putText( fp, firstThing );

    /* Write out macro definitions */
    for( m=macroList; m!=MNULL; m=m->next ) {
        if( m->useCount>0 ) {
            fprintf(fp,"define %s |\n",m->name);
            fprintf(fp,"[ box invis ht %d wid %d with .sw at 0,0\n",
            m->bb.corner.y, m->bb.corner.x);

            if( (t=m->parts) != TNULL ) {
                do {
                    writePIC( t, fp, Rpt(Pt(0,0), m->bb.corner) );
                    t = t->next;
                } while( t != m->parts );
            }
            fprintf(fp,"] |\n\n");
        }
    }

    /* Write out bounding box. */
    fprintf(fp,"box invis ht %d wid %d with .sw at 0,0\n",
    BBpic.corner.y - BBpic.origin.y,
    BBpic.corner.x - BBpic.origin.x);

    t = firstThing;
    do {
        writePIC(t,fp,BBpic);
        t = t->next;
    } while( t != firstThing );

    fprintf(fp,".PE\n.fi\n");
    fclose(fp);
    cursSwitch();
    changeButtons(saveB);
}


fileError(s,fn)
	register char *s, *fn;
{
	moreMessage("\nCannot ");
	moreMessage(s);
	moreMessage(" file ");
	moreMessage(fn);
}


    struct thing *
doGet(h)
    register struct thing *	h;
{
    register FILE *		fp;
    int 			r;
    char			hostCmd[MAXNAMESIZE+100];

    initMessage();

    if( editDepth != 0 ) {
        moreMessage("Warning: currently editing a macro.\r");
        moreMessage("Continue get file operation?\r");
        if( !RUsure() )
            goto wrapup;

    } else if( h != TNULL ) {
        moreMessage("Warning: adding to existing picture.\r");
        moreMessage("Clear screen first?\r");
        if( RUsure() )
            h = doClear(h);
    }

    changeButtons(DObuttons);

    moreMessage("Get file: ");

    r = getMessage(filename);

    if( (r==1) || (r==2) || (strlen(filename) == 0) ) {
        newMessage("*** get file cancelled ***");
        goto wrapup;
    }

    if (access(filename,4)!=0) {
        fileError("access",filename);
        goto wrapup;
    }

    strcpy( hostCmd, Jpic_pgm );
    strcat( hostCmd, " " );
    strcat( hostCmd, filename );
    strcat( hostCmd, " 2>&1" );

    if ((fp = popen(hostCmd,"r")) == (FILE *) NULL) {
        fileError("open pipe for",filename);
        goto wrapup;
    } 

    cursswitch(&hourglass);
    changeButtons(BLANKbuttons);
    getChar(fp);

    h = readPic(fp,h);

    if( firstThing == TNULL )
        firstThing = h;

    pclose(fp);
    cursSwitch();

   wrapup:
    changeBrush(-1);
    changeButtons(INITbuttons);
    return(h);
}


eraseAll()
{
	cursinhibit();
	clearRegion( brushes[PIC] );
	cursallow();
	if (gridState==GRIDon) {
		drawGrid();
	}
}

	struct thing *
doClear(h)
	struct thing *h;
{
	deleteAllThings(h);
	eraseAll();
	copyFlag = 0;
	textOutName = INITtextOutName;
	changeBrush(-1);
	changeButtons(INITbuttons);
	undo_clear();
	scrollOffset.x = 0;
	scrollOffset.y = 0;
	return(TNULL);
}

    struct thing *
defineMacro( h, offset )
    register struct thing *	h;
    Point             		offset;
{
    Point p, q;
    register struct thing *s, *t, *l =  TNULL;
    Rectangle r;
    struct thing dummy;
    char *z;        /* Temporary alloc space */

    changeBrush(-1);
    /* Test to see if there is enough memory to create this macro */
    if ((z=getSpace(sizeof(struct macro)+sizeof(struct thing))) == NULL) {
        return (h);
    }
    free (z);
    changeButtons(MACRObuttons);

    while( !button12() ) {
	if (P->state & RESHAPED)
	  redrawLayer();
        nap(2);
    }
    
    if( button1() ) {
        /* abort defining macro */
        while( button1() )
            nap(2);
        changeButtons(INITbuttons);
        return( h );
    }

    p = sub( MOUSE_XY, offset );
    q = trackMacro( p, offset );
    r = canon( p, q );

    /* Go through entire list of things moving those which are inside 
       the rectangle r to a new list l.  Must use a dummy thing as an
       end marker.
    */
    t = insert(&dummy,h);  /* Note: t is &dummy->next. */
    while( t != &dummy ) {
        if (inside(r,t->bb)) {
            s = Remove(t);
            l = insert(t,l);

            /* If firstThing happens to point to t,
               reset to TNULL.  It will be reset
               to point to the new macro below. */
            if( firstThing == t )
                 firstThing = TNULL;

            t = s;
        } else {
            t = t->next;
        }
    }
    h = Remove(&dummy);

    if( (t = l) != TNULL ) {
        r = macroBB(l);        /* Get outline of macro */
        do {
            makeRelative(t,r.origin);    /* Make things relative to this box */
            t = t->next;
        } while (t != l);
        p = r.origin;         /* Macro is located at original orig */
        r = rsubp (r, r.origin); /* but the bb is made relative. */
        h = insert(newMacro(p,recordMacro(l,r,NULL)),h);

        if( firstThing == TNULL )
            firstThing = h;

        /* To insure new macro is first thing selected,
           back up h list by two elements! */
        h = h->last->last;
    }

    changeButtons(INITbuttons);
    return(h);
}

	Rectangle 
macroBB(l)
	struct thing *l;
{
	Point p, q, p1, p2;
	register struct thing *t;
	Rectangle r;

	p.x = Xmax; 
	p.y=YBOT; 
	q.x=0; 
	q.y=0;
	if ((t=l) != TNULL) {
		do {
			if ( (t->type == TEXT) || (t->type == ARC) ){
				/* Expand out the bounding box to nearest
				   alignment points to insure alignment
				   of new macro. */
				p1 = alignDown( t->bb.origin );
				p2 = alignUp(   t->bb.corner );
			} else {
				p1 = t->bb.origin;
				p2 = t->bb.corner;
			}
			p.x = min(p.x,p1.x);
			p.y = min(p.y,p1.y);
			q.x = max(q.x,p2.x);
			q.y = max(q.y,p2.y);
			t=t->next;
		} while (t != l);
	}

	/* Add 1 to prevent zero size bounding box. */
	if (p.x == q.x) 
		q.x += 1;
	if (p.y == q.y)
		q.y += 1;

	r.origin = p; 
	r.corner = q;
	return(r);
}


/* Find macro definition given name. */
    struct macro *
findMacro(s) 
    register char *s;
{
    struct macro *m;

    for( m=macroList; 
	 ((m!=MNULL) && (strcmp(s,m->name)!=0)); 
 	 m=m->next) 
	;
    return(m);
}


/* Returns TRUE if macro id is unique. */

    int
uniqMacroId(id)
    register short id;
{
    register struct macro *	m;

    for( m=macroList; m!=MNULL; m=m->next) {
        if( m->id == id )
	    return FALSE;
    }
    return TRUE;
}


/* Returns an available macro id to be used for a new macro. */

    short
newMacroId()
{
    register short  i;

    /* Start from 0 to find an id available.  Prevents big holes. */
    for( i=0 ; !uniqMacroId(i) ; i++ )  
	;
    return i;
}


/* Compare things in macros.  Returns TRUE if macros are the same.  
   Will recurse to check out nested macros.
*/
    BOOL
cmpMacroParts( p1, p2 )
    register struct thing *  p1;
    register struct thing *  p2;
{
    /* Create 2 macro things temporarily, so can call cmpThings. */
    struct thing   t1;
    struct thing   t2;

    struct macro   m1;
    struct macro   m2;

    t1.type = MACRO;
    t2.type = MACRO;

    t1.origin = t2.origin = Pt(0,0);
    t1.otherValues.list = &m1;
    t2.otherValues.list = &m2;

    m1.parts = p1;
    m2.parts = p2;

    return cmpThings( &t1 , &t2 );
}


/* This routine places a new macro at the start of the macro list.
   The head of the list is pointed to by macroList. 
   The bounding box for the macro always has a relative origin of (0,0). 
*/

    struct macro *
recordMacro(list,r,s)
    struct thing * list; /* List of things making up macro */
    Rectangle      r;    /* BB of macro with origin at (0,0) */
    char *         s;    /* Name of macro */
{
    register struct macro *m, *l, *n;
    int  i;

    if( (m = (struct macro *) getSpace(sizeof(struct macro)))
        !=MNULL) {
        if( s == NULL ) {
            /* Macro has no name, so make up one. */
            m->id = newMacroId();
            m->name = getSpace(8); /* 'm' + 6 digits + '\0' */
            sprintf( m->name, "m%d", m->id );

        } else { /* Macro has a name. */
            m->name = s;
            m->id = 0;
            if( (s[0] == 'm') && isdigit(s[1]) ) {
                /* Macro name assumed to be generated name.  Find 
                   and store its id. */
                i = 1;
                while( isdigit(s[i]) ) {
                    m->id = (10 * m->id) + (s[i] - '0');
                    i++;
                }
            }
        }
        m->bb = r;
        m->useCount = 0;
        m->parts = list;

        /* Append macro to the end of the macro list */
        l = MNULL;
        for (n=macroList; n!=MNULL; n=n->next) {
            l=n;
        }
        if (l == MNULL) {
            macroList = m;
        }
        else {
            l->next = m;
        }
    }
    return(m);
}


	int 
inside(r,s)
	Rectangle r,s;
{
	return((r.origin.x <= s.origin.x) && (r.origin.y <= s.origin.y)
	    && (r.corner.x >= s.corner.x) && (r.corner.y >= s.corner.y));
}


  struct thing *
makeRelative(t,p)
  register struct thing *t;
  Point p;
{
	register int i;

	t->origin = sub(t->origin,p);
	switch(t->type) {
	case CIRCLE:
	case ELLIPSE:
	case TEXT: 
		{
			break;
		}
	case LINE: 
		{
			t->otherValues.end = sub(t->otherValues.end,p);
			break;
		}
	case BOX: 
		{
			t->otherValues.corner = sub(t->otherValues.corner,p);
			break;
		}
	case ARC: 
		{
			t->otherValues.arc.start = sub(t->otherValues.arc.start,p);
			t->otherValues.arc.end = sub(t->otherValues.arc.end,p);
			break;
		}
	case SPLINE: 
		{
			for (i=0; i<=t->otherValues.spline.used; i++)  {
				t->otherValues.spline.plist[i] = 
				    sub(t->otherValues.spline.plist[i],p);
			}
			break;
		}
	}
	boundingBox(t);
	return(t);
}

    int 
backspaceOneWord(s,i)
    register char *s;
    register int i;
{
    s[(i>0)? --i : 0] = '\0';
    for ( ; i>0 && (isdigit(s[i-1]) || isletter(s[i-1])); ) {
        s[--i]='\0';
    }
    return(i);
}


#ifdef DMD630

	Rectangle
canon (p1, p2)
	Point p1, p2;
{
	Rectangle r;

	r.origin.x = min (p1.x, p2.x);
	r.origin.y = min (p1.y, p2.y);
	r.corner.x = max (p1.x, p2.x);
	r.corner.y = max (p1.y, p2.y);
	return (r);
}
#endif



cursSwitch ()
{
	if (own() & MOUSE) {
		cursswitch(ptinrect(MOUSE_XY,brushes[PIC])
		    ? &crossHairs : (Texture16 *)0);
	}
}



static int kbdcount = 1;
static int metafg = 0;
static int counting = 0;
static char *oldText = 0;

/* Get a character from keyboard and add to text string associated with thing
   unless character is a control or ESC character in which case the string is 
   edited via a subset (and extension) of emacs operations.  (The CNTL-U &
   CNTL-W are the extensions to keep compatibility with the original cip
   and xcip.)  The charPos is the position where editing occurs in the 
   character string.  Function returns the new charPos after editing.
*/
    int
editText( t, offset, charPos )
    register struct thing *	t;
    Point			offset;
    int                  	charPos; /* position where edited */
{
    register char *		s;
    register int		i,j,k;
    register char		c;

    s = t->otherValues.text.s;
    i = charPos;

    draw(t,offset);  /* erases present string. */

    /* copy part of string before edit cursor into temporary buffer. */
    strncpy(tempString,s,charPos);

    c = kbdchar();

#ifdef DMD5620
    if (c >= 0x82 && c <= 0x89) {
	/* DMD function keys - load in entire string. */
        j = c - 0x82;
        k = 0;
        while ((c = BRAM->pfkeys[j][i].byte) && (++k <= PFKEYSIZE)) {
            if (i >= MAXTEXT) {
                ringbell ();
                break;
            }
            else {
                tempString[i++] = c;
            }
        }
    } else { 
#endif

    if (c == 27 /* ESC */ ) {
	metafg = 1;
    } else if (counting && (c >= '0') && (c <= '9')) {
	/* Build up number of times to repeat argument. */
        kbdcount = 10*kbdcount+c-'0';
    } else if (metafg && (c >= '0') && (c <= '9')) {
	/* Start to build up number of times to repeat argument. */
        counting = 1; 
	metafg = 0;
        kbdcount = c-'0';
    } else {
        while (kbdcount--) {
            if (metafg) {
		/* META operations.  Characters preceded by the ESC. */
		switch (c) {
                case 'f': /* Forward word. */
                case 'd': /* Delete next word. */
                    while (tempString[charPos] && 
			  !(isdigit(tempString[charPos]) || 
			    isletter(tempString[charPos]))) 
		        charPos++;
                    while (isdigit(tempString[charPos]) ||
			   isletter(tempString[charPos]) ) 
                        charPos++;
                    if (c == 'f') i = charPos;
                    break;

                case 'b':     /* Back word. */
                case 127:     /* DELETE: Delete next word. */
                case 'H'-'@': /* ^H: Delete previous word. */
                    while (i && !(isdigit(tempString[i-1]) || 
			   isletter(tempString[i-1]) ) ) 
		        i--;
                    while (i && (isdigit(tempString[i-1]) || 
			   isletter(tempString[i-1]) ) )
		        i--;
                    if (c == 'b') charPos = i;
                    break;
                }
	    } else {
		switch (c) {

                case 'A'-'@': /* ^A: Start of line. */
                    while (i && tempString[i-1] != '\r') i--;
                    charPos = i;
                    break;

                case 'B'-'@': /* ^B: Back 1 character. */
                    if (i) i--;
                    charPos = i;
                    break;

                case 'C'-'@': /* ^C: Capitalize. */
                    if (tempString[i] >= 'a' && tempString[i] <= 'z') 
			tempString[i] -= 32;

                case 'F'-'@': /* ^F: Forward one character. */
                    if (tempString[i]) {
                        i++;
                        charPos = i;
                    }
                    break;

                case 'D'-'@': /* ^D: Delete next character. */
                    if (tempString[charPos])
                        charPos++;
                    break;

                case 'E'-'@': /* ^E: End of line. */
                    while (tempString[i] && tempString[i] != '\r') 
			i++;
                    charPos = i;
                    break;

                case 'H'-'@': /* ^H: Delete one character. */
                    if (i>0) 
			i--;
                    break;

                case 'K'-'@': /* ^K: Kill text after the cursor. */
                    while (tempString[charPos]) 
			charPos++;
                    break;

                case 'U'-'@': /* ^U: Delete all preceding text. */
		    i=0;
		    break;

                case 'W'-'@': /* ^W: Backspace one word. */
		    i = backspaceOneWord( tempString, i );
		    break;

                case 'Y'-'@': /* ^Y: Retrieve last deletion. */
                    if (oldText) {
                        j = 0;
                        while (oldText[j] && i<MAXTEXT)
			    tempString[i++] = oldText[j++];
                    }
                    break;

                case '\r':    /* return */
                case '\n':    /* line feed */
                    c = '\r';  /* put on EOL character */
                    /* FALL THRU to put EOL character in string */

                default: 
                    if (i >= MAXTEXT) {
                        ringbell ();
                    } else {
                        tempString[i++] = c;
                    }
                    break;
                }
	    }
        }

        kbdcount = 1;
	metafg = 0;
	counting = 0;
        j = charPos-i;
        if (j>0) {
	    /* Save old text string. */
            if (oldText != NULL) 
		free(oldText);
            oldText = getSpace(j+1);
            strncpy(oldText,s+i,j);
            oldText[j] = 0;
        }
    }

#ifdef DMD5620
    }
#endif

    /* Copy any part of string that was after edit cursor and null char. */
    for( j=charPos, charPos=i; (tempString[i++] = s[j++]) != '\0'; )
        ;

    free(s);
    s = getSpace(i);
    strcpy(s,tempString);
    t->otherValues.text.s = s;

    draw( t, offset );  /* display new string. */

    return( charPos );
}


drawText(p,s,just,spacing,f)
	Point p;		/* point at center of text */
	register char *s;
	short just;		/* justification */
	short spacing;
	Font *f;
{
	register int line;	/* line number */
	register int lx;	/* line x position */
	register int ly;	/* line y position */
	register int q;
	register int m;
	int num; 		/* number of lines */

	num = numLines(s);

	if( num == 0 )
		return;

	ly = -( ( (fontheight(f)+spacing) * (num-1) ) >> 1 );

	q = 0;

	for(line = 1; line <= num; line++)
	{
		for(m = 0; (s[q] != '\r') && (s[q] != '\0'); q++ )
			tempString[m++] = s[q];
		tempString[m] = '\0';
		q++;
		switch (just) {
		case CENTER:
			lx = p.x - ( strwidth(f, tempString)>>1 );
			break;
		case LEFTJUST:
			lx = p.x;
			break;
		case RIGHTJUST:
			lx = p.x - strwidth(f, tempString);
			break;
		}
		string(f, tempString,&display, Pt(lx, (p.y + ly)), F_XOR);
		ly = ly + (fontheight(f)+spacing);
	}
}


	void 
StringSize(f,spacing,s,h,w)
	register int *h, *w;
	Font *f;
	register char *s;
	short spacing;
{
	register int num;
	register int m,q;
	register int line;

	num = numLines(s);

	*h = (num * fontheight(f)) + ( (num-1) * spacing );

	*w = 0;
	q = 0;

	for(line = 1; line <= num; line++) {
		for(m = 0; (s[q] != '\r') && (s[q] != '\0'); q++)
			tempString[m++] = s[q];
		tempString[m] = '\0';

		*w = max( *w, strwidth(f,tempString) );
		q++;
	}

	*w = *w + 5;
}



#ifdef DMD630
/* Word is 16 bits. */
Word textMarker[] = {		/* marker icon */
	 0x0000, 0x0000, 0x0000, 0x0080,
	 0x0080, 0x01C0, 0x01C0, 0x03E0,
	 0x03E0, 0x07F0, 0x07F0, 0x0FF8,
	 0x0FF8, 0x0FF8, 0x0FF8, 0x0000,
};

Word markerMask[] = {		/* one bit bigger marker icon */
	 0x0000, 0x0000, 0x0080, 0x01C0,
	 0x01C0, 0x03E0, 0x03E0, 0x07F0,
	 0x07F0, 0x0FF8, 0x0FF8, 0x1FFC,
	 0x1FFC, 0x1FFC, 0x1FFC, 0x1FFC,
};
#endif

#ifdef DMD5620
/* Word is 32 bits on DMD 5620. */
Word textMarker[] = {		/* marker icon */
	 0x00000000, 0x00000000, 0x00000000, 0x00800000,
	 0x00800000, 0x01C00000, 0x01C00000, 0x03E00000,
	 0x03E00000, 0x07F00000, 0x07F00000, 0x0FF80000,
	 0x0FF80000, 0x0FF80000, 0x0FF80000, 0x00000000,
};

Word markerMask[] = {		/* one bit bigger marker icon */
	 0x00000000, 0x00000000, 0x00800000, 0x01C00000,
	 0x01C00000, 0x03E00000, 0x03E00000, 0x07F00000,
	 0x07F00000, 0x0FF80000, 0x0FF80000, 0x1FFC0000,
	 0x1FFC0000, 0x1FFC0000, 0x1FFC0000, 0x1FFC0000,
};
#endif


#define MARKERWID	16
#define MARKERHT	16

#ifdef DMDTERM
  Word	bgsave[MARKERHT];	/* Space to save background */

  Bitmap  markerbm = { 
		textMarker,	/* Will be changed to markerMask and back */
		1,
		0, 0, MARKERWID, MARKERHT,
		0
	  };
#endif /* DMDTERM */


	int
moveTextSpot(t,offset)
 	struct thing *	t;
	Point		offset;
{
	register char *	s;
	register int 	i;	/* position in text string */
	int		istart;	/* saves start of line */
	register int	j;	/* position in temp string */
	register int 	line;	/* line number */
	register int 	x;	/* line x position */
	register int 	y;	/* line y position */
	Point		m;	/* mouse position */
	Point		pt;	/* text marker point */
	int		spacing;
	int		prevI;	/* previous position in text string */
	int		w;	/* string or character width */
	int 		num; 	/* number of lines */
	short		just;	/* justification */
	Font *		f;

	s = t->otherValues.text.s;
	f = t->otherValues.text.f->f;
	just = t->otherValues.text.just;
	spacing = fontheight(f) + t->otherValues.text.spacing;

	pt.x = offset.x+16;
	pt.y = offset.y+16;
	cursinhibit();

#ifdef X11
	bitblt( &display, raddp(bgsave.rect,pt),
		&bgsave,bgsave.rect.origin, F_STORE );
#else 
	markerbm.base = bgsave;		/* save initial background */
	bitblt( &display, raddp(markerbm.rect,pt),
		&markerbm,markerbm.rect.origin, F_STORE );
#endif /* X11 */

	prevI = -1;

	do {
		m = sub(MOUSE_XY,offset);

		num = numLines(s);
		x = t->origin.x;
		y = t->origin.y - ( ( spacing * (num-1) ) >> 1 );


		/* Skip over any text lines above mouse position, except
		 * do not skip over last line even if mouse is below it.
	 	 */

		i = 0;
		line = 1;
		for (; (line < num) && (m.y >= (y + spacing)); line++) {

			while (s[i++] != '\r')
				;
			y += spacing;
		}


		/* Form string from only this line's characters. */

		istart = i;	/* Save start of current line */

		for (j=0; (s[i] != '\r') && (s[i] != '\0'); ) {
			tempString[j++] = s[i++];
		}
		tempString[j] = '\0';


		/* Adjust x for justification */

		w = strwidth( f, tempString);
		switch (just) {
		case CENTER:
			x = t->origin.x - ( w >> 1 );
			break;
		case LEFTJUST:
			x = t->origin.x;
			break;
		case RIGHTJUST:
			x = t->origin.x - w;
			break;
		}


		i = istart;	/* Back to start of line */

		/* Find character hit */

		while (( x <= m.x) && (s[i] != '\0') && (s[i] != '\r')) {
			x += fontwidth(f,s[i]);    /* Char width */
			i++;
		}

		if( i != prevI ) {
			prevI = i;

			/* restore background */
#ifdef X11
			bitblt(&bgsave, bgsave.rect, &display, pt, F_STORE);
#else
			markerbm.base = bgsave;
			bitblt(&markerbm, markerbm.rect, &display, pt, F_STORE);
#endif /* X11 */

			pt.x = x - 9 + offset.x;
			pt.y = y + fontheight(f) - 
				(fontheight(f)>>2) -4 + offset.y;

			/* save background */
#ifdef X11
			bitblt( &display, raddp(bgsave.rect,pt),
				&bgsave,bgsave.rect.origin, F_STORE );
			bitblt( &markermaskbm, markermaskbm.rect, 
				&display, pt, F_CLR );
			bitblt( &markerbm, markerbm.rect, &display, pt, F_OR );
#else 
			bitblt( &display, raddp(markerbm.rect,pt),
				&markerbm,markerbm.rect.origin, F_STORE );
			markerbm.base = markerMask;	/* make dark edge  */
			bitblt( &markerbm, markerbm.rect, &display, pt, F_OR );
			markerbm.base = textMarker;	/* draw marker */
			bitblt( &markerbm, markerbm.rect, &display, pt, F_XOR );
#endif /* X11 */

			/* Note: F_OR & F_XOR used when marking on 
			 * highlighted area; F_CLR & F_OR otherwise.
			 */
		}

		nap(2);
	} while( button1() );

	/* restore background */
#ifdef X11
	bitblt( &bgsave, bgsave.rect, &display, pt, F_STORE );
#else
	markerbm.base = bgsave;
	bitblt( &markerbm, markerbm.rect, &display, pt, F_STORE );
#endif /* X11 */

	cursallow();
	return( i );
}


/* Like strcmp in string(3C).  Compares two character strings, returns less
   than, equal to, or greater than 0, according as s1 is lexicographically
   less than, equal to, or greater than s2.
*/
    int 
strcmp(s1,s2) 
    register char *s1, *s2;
{
    while ( *s1!='\0' && *s2!='\0' && *s1==*s2) {
        s1++; s2++;
    }

    if( *s1=='\0' && *s2=='\0' )
	return 0;

    if( *s1=='\0' ) 
	return -1;

    if( *s2=='\0' ) 
	return 1;
    
    if( *s1 < *s2 )
	return -1;
    else
	return 1;
}
