#include "cip.h"

extern Point align();
extern struct thing * editThing();
extern int currentBrush, copyFlag, thingSelected, editDepth;
extern Rectangle brushes[];
extern int gridState, videoState, buttonState;
extern void printInfo();
extern void printNum();
extern void save_old();
extern void save_new();

int charPos = 0;

int same_obj = 1;

#ifdef DMDTERM
    char *resizeText[] = {
	" Open window ", 
	" Quit editor ", 
	NULL};
    Menu resizeMenu = {resizeText};

    Texture16 menucursor = {
	0xFFC0, 0x8040, 0x8040, 0x8040, 0xFFC0, 0xFFC0, 0xFE00, 0xFEF0,
	0x80E0, 0x80F0, 0x80B8, 0xFE1C, 0x800E, 0x8047, 0x8042, 0xFFC0};
#endif 


  struct thing *
doMouseButtons(t,offset) 
  register struct thing *t;
  Point			offset;
{
  Point 		m;
  Rectangle *		b;
  struct thing *	t1;
  struct thing *	tmp;

  if (CORRECTSIZE) {

#ifdef DMDTERM
    if (P->state & MOVED ) {
      /* Window has been moved so must reset global variables. */
      Xmin = P->layer->rect.origin.x + BORDER;
      Ymin = P->layer->rect.origin.y + BORDER;

      drawOffset.x = Xmin;
      drawOffset.y = YPIC;
      brushInit();
      P->state &= ~(MOVED|RESHAPED);

    } else 
#endif 

    if (P->state & RESHAPED) {
      /* Redraw screen if layer has been reshaped */
      redrawLayer();

#ifdef X11
      return(t);
#endif

    }

    if ((own () & MOUSE) && ptinrect (MOUSE_XY, brushes[PIC])) {
      if (P->cursor != &crossHairs) {
	cursswitch (&crossHairs);
      }
    } else {
      if (P->cursor == &crossHairs) {
	cursswitch ((Texture16 *)0);
      }
    }

#ifdef DMDTERM
  } else {
    /* Not the correct size.  Don't go to main menu until layer is full size */
    cursswitch(&menucursor);

    while ( !button3() ) {
      sleep(2);
#ifdef DMD630
      if( (P->state & RESHAPED) && !(P->state & MOVED) )
#endif
#ifdef DMD5620
      if( (P->state & RESHAPED) )
#endif
      {
	drawToolIcon();
        P->state &= ~RESHAPED;
      }
    }

    cursswitch ((Word *)NULL);

    switch ( menuhit(&resizeMenu, 3) ) {
	case 0: /* Open window */
	  {
	    resizeLayer();
	    break;
	  }
	case 1: /* Exit editor */
	  {
	    if (RUsure ())  
		exit();
	    break;
	  }
    }
    return (t);
#endif
  }


  /* Add drawing frame absolute position and scrolling offset to offset. */
  offset = add( add(drawOffset, scrollOffset), offset );
  wait( MOUSE|KBD );
  m = sub(MOUSE_XY,offset);


  /* BUTTON 1 PRESSED */

  if ( button1() ) {

    /* Delete empty text string as no longer editing it */
    if( thingSelected && (t->type==TEXT) )
      if( t->otherValues.text.s[0] == '\0' )
        t = deleteThing(t);

    b = Select(t, add(m,offset), offset);
    if ( b == &brushes[PIC]) {

      /* Pressed inside picture drawing area */
      copyFlag=0;
      t1 = selectThing(m,t);
      if (thingSelected==1) {
	drawSelectionLines(t,offset);
      }
      if (t1 != TNULL) {
	t = t1;
	thingSelected = 1;
	changeBrush(-1);
	same_obj = 0;
	if (t->type==MACRO||t->type==TEXT) {
	  changeButtons(MOVEbuttons);
	} else {
	  changeButtons(EDITbuttons);
	}

	flashThing(t,offset);

	printInfo(t);

	if(t->type==TEXT) {
	  charPos = moveTextSpot(t,offset);
	} else {
	  while( button1() )
	      nap(2);
	}

	drawSelectionLines(t,offset);
	flashThing(t,offset);

      } else {

	/* Nothing selected in picture area */
	thingSelected = 0;
	changeBrush(-1);
	changeButtons( INITbuttons );
	initMessage();
      }

    } else {

	/* Pressed in brush, edit level or some outside picture area */

	initMessage();
	same_obj = 0;

	if (thingSelected==1) {
		drawSelectionLines(t,offset);
		thingSelected = 0;
	}
	if (b != (Rectangle *) NULL ) {
		/* Brush area selected */
		copyFlag=0;
		thingSelected = 0;
		switch( currentBrush ) {
		case TEXT:
		case BOX:
		case SPLINE:
		case LINE:
			changeButtons(GLOBbuttons);
			break;
		default:
			changeButtons(DRAWbuttons);
			break;
		}
	}
    }
    for (; button1(); nap(2))
		; 


  /* BUTTON 2 PRESSED */

  } else if( button2() ) {
      if( thingSelected ) { 
	if( copyFlag ) {
	  drawSelectionLines( t, offset );
	  t = insert( copyThing( t, align(m), offset, 1 ), t )->last;
	  drawSelectionLines( t, offset );
	  /* clear button 2 - only one copy per click */
	  for ( ; button2(); nap(2)) ;
	} else {
	  if( !same_obj ) {
	     same_obj = 1;
	     save_old( t );
	  }
	  t = editThing(m,offset,t);
	  save_new( t );
	}

      } else {
	/* No thing is selected */
	if ((currentBrush>=0)&&(ptinrect(add(m,offset),brushes[PIC]))) {
	    if (currentBrush==SPLINE) 
		changeButtons(SPLINEbuttons);

	    save_old( TNULL );   

	    t = place(currentBrush,m,t,offset);

	    if( firstThing == TNULL )
		firstThing = t;

	    save_new( t );

	    /* Backup one object, so that when object is selected, then
	       the newly created object will be selected first.  Note,
	       that selectThing() goes to next object in list. 
	    */
	    if( t != TNULL ) t = t->last;

	    switch( currentBrush ) {
	    case TEXT:
	    case BOX:
	    case SPLINE:
	    case LINE:
	  	changeButtons(GLOBbuttons);
	  	break;
	    default:
	    	changeButtons(DRAWbuttons);
	  	break;
	    }
	} else {
	    moveAll();
	}
      }


  /* BUTTON 3 PRESSED */

  } else if (button3()) {
     if (thingSelected) {
	  	if( !same_obj ) {
		     same_obj = 1;
		     save_old( t );
		}
		tmp = t; /* Note: t may be deleted, so must use tmp. */
		t = displayThingMenu( m, &tmp, offset );
		printInfo( tmp );
		save_new( tmp );
     } else {
		t = displayCommandMenu( t, offset );
     }


  /* KEYBOARD INPUT RECEIVED */

  } else if (own() & KBD ) {
      if (thingSelected && (t->type==TEXT) ) {
	if( !same_obj ) {
	     same_obj = 1;
	     save_old( t );
	}
        charPos = editText(t,offset,charPos);
        boundingBox(t);
	printInfo(t);
	save_new( t );
      } else {
	/* Ignore text, but must eat it up to prevent hanging. */
	kbdchar();
      }
  }

  return(t);
}

moveAll()
{
	extern Rectangle BBpic;
	Point offset;
	Rectangle r;

	eraseAll();

	findBBpic( firstThing );

	/*
	 * I don't entirely understand why these offsets are the
	 *   ones needed (and especially why the drawingOffset and
	 *   BBpic.origin need to be both in 'r' and in 'offset'), 
	 *   but after a lot of trial and error I found that these are
	 *   what is needed.  - Dave Dykstra, 12/21/95
	 */

	r = raddp(BBpic, add(drawOffset, scrollOffset));
	offset = add(drawOffset, BBpic.origin);

	xbox( raddp(r, offset) );

	r = moveBox(MOUSE_XY, r, offset, 1);

	newMessage( "Offset was " );
	printPt( scrollOffset );

	scrollOffset = r.origin;

	moreMessage( " now " );
	printPt( scrollOffset );

        doRedraw();
}


  struct thing *
place(b,p,h,os)
  register int b;
  Point p, os;
  struct thing *h;
{
  register struct thing *t;
  register Point *plist, *olist;
  register int i, used, room;
  struct thing dummy;
  register char *	s;

  p = align(p);

  switch (b) {
    case CIRCLE: {
      t = newCircle(p);
      if( t != TNULL ) {
	t->otherValues.radius = 2;
	draw(t,os);
	h = insert(t,h);
	track(p,os,GROWCIRCLE,t);
      }
      break;	
    }
    case BOX: {
      t = newBox(Rpt(p,p));
      if( t != TNULL ) {
	h = insert(t,h);
	track(p,os,BOX,t);
	draw(t,os);
      }
      break;	
    }
    case ELLIPSE: {
      t = newEllipse(p);
      if( t != TNULL ) {
	t->otherValues.ellipse.ht = 2;
	t->otherValues.ellipse.wid = 2;
	draw(t,os);
	h = insert(t,h);
	track(p,os,ELLIPSE,t);
      }
      break;	
    }
    case LINE: {
      t = newLine( p, p );
      if( t != TNULL ) {
	h = insert(t,h);
	track(p,os,LINE,t);
	draw(t,os);
      }
      break;	
    }
    case ARC: {
      t = newArc( p, p );
      if( t != TNULL ) {
	h = insert(t,h);
	track(p,os,ARC,t);
	draw(t,os);
      }
      break;	
    }
    case SPLINE: {
      if ((plist = (Point *)getSpace(5*sizeof(Point)))!=(Point *)NULL) {
	h = dummy_insert(&dummy,h);
	h->type = -1;  /* Set this to invalid value so printInfo() called
			later will not print. */
	plist[1]=p;
	used = 1; room = 3;
	do {
	  if (used==room) {
	    olist = plist;
	    room <<= 1;
	    plist = (Point *) getSpace((room+2)*sizeof(Point));
	    if (plist==(Point *)NULL) {
	      draw (&dummy, os);
	      h = Remove (&dummy);
	      plist = olist;		/* Free list later */
	      used = 0;
	      break;
	    }
	    for (i=1; i<=used; i++) {
	      plist[i]=olist[i];
	    }
	    free(olist);
	  }
	  if (button2()) {
	    ++used;
	    plist[used]= track(plist[used-1],os,LINE,h);
	    xsegment(add(os,plist[used-1]),
	              add(os,plist[used]));
	    initMessage();
	    printNum( (short) used );
	    moreMessage( " points" );
	  }
	  nap(2);
	  if (P->state & RESHAPED) {
	    redrawLayer();
	    drawZigZag(os,plist,used);
	  }
	} while (!button3());

	for (; button3(); ) 
	  nap(2);

	drawZigZag(os,plist,used);
	if (used>2) {
	  t = newSpline(++used,room,plist);
	  draw(t,os);
	  printInfo(t);
	} else {
	  t = TNULL;
	  free (plist);
	}
	h = Remove(&dummy);
	h = insert(t,h);
      }
      break;
    }
    case TEXT: {
      int	cursorOn;
      int	charPos;
      Point	m;

      cursorOn = 1;
      charPos  = 0;

      s = getSpace(1);
      s[0] = '\0';
      t = newText(p,s);
      printInfo(t);
      if( t==TNULL )
	break;

      while( button123() )
	nap(2);
      cursswitch( &textCursor );
      changeButtons(EXITbuttons);
      clearKeyboard();

      m = mouse.xy;

      while (1) {
	wait( KBD | MOUSE );
	if (P->state & RESHAPED) {
	  redrawLayer();
	  draw(t,os);
	}

	if( own() & KBD ) {
	  if( cursorOn ) {
            cursSwitch();
	    cursinhibit();
	    cursorOn = 0;
	  }

	  charPos = editText(t,os,charPos);

	  printInfo(t);
	} 
	
	if( !eqpt(m,mouse.xy) && !cursorOn ) {
	    cursorOn = 1;
	    m = mouse.xy;
	    cursallow();
            cursSwitch();
	}

	if( own() & MOUSE ) {
	  if( button123() )
	    break;
	}
      }; 

      if( !cursorOn ) {
	  cursorOn = 1;
	  cursallow();
          cursSwitch();
      }

      while( button123() )
	nap(2);

      h = insert(t,h);

      if( t->otherValues.text.s[0] == '\0' ) {
	h = deleteThing(t);
	t = TNULL;
	initMessage();
      }

      break;
    }
  }


  if( t == TNULL ) {
    return( h );

  } else {
    boundingBox(t);

    if( eqpt(t->bb.origin,t->bb.corner) ) {
      /* thing created has no size so remove it */
      h = deleteThing(t);
      return( h );

    } else {
      return( t );
    }
  }
}


clearKeyboard()
{
  for (; kbdchar() != -1; ) ;
}
