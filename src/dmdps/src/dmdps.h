/* */
/*									*/
/*	Copyright (c) 1987,1988,1989,1990,1991,1992   AT&T		*/
/*			All Rights Reserved				*/
/*									*/
/*	  THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T.		*/
/*	    The copyright notice above does not evidence any		*/
/*	   actual or intended publication of such source code.		*/
/*									*/
/* */
#ifdef OLD
#include <jerq.h>
#include <font.h>
#include <layer.h>
#include <blitio.h>
#else
#ifdef DMD630
#include <dmd.h>
#include <font.h>
#include <dmdio.h>
#include <5620.h>
#include <object.h>
#else
#include <dmd.h>
#include <font.h>
#include <blitio.h>
#define point2layer
#include <pandora.h>
#endif
#define debug point2layer
#endif

