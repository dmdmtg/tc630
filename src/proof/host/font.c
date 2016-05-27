/*	Copyright (c) 1987 AT&T	*/
/*	All Rights Reserved	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/


static char _2Vsccsid[]=" @(#)font.c	1.1.1.12 (6/21/90) ";

/* includes */
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include "host.h"
#include "../include/comm.h"
#include <ctype.h>
#include <string.h>

/* defines */
#define True 1
#define False 0
#define Eq(str1, str2) \
	(strcmp (str1, str2) == 0)
#define MaxTries 20

/* types */
typedef struct {
	char name[10];
	int there;	/* font used but does not exist */
	Font font;
} Mount;

/* externals */
extern short proofterm;
extern int errno;
extern Font defont;
extern Font curfont;
extern short debugOpt;
extern short cacheText;
extern int ptsz;
extern char specchar [];
extern int jerq;

#ifdef	DEBUG
	extern FILE *debug;
	extern verbose;
#endif	/* DEBUG */

/* globals */
int newfont;
#define NUMFAMILIES 20
char fname[NUMFAMILIES];
char family[NUMFAMILIES][8];


/* locals */
static int famnum = 0;
static Mount mount[NFONTS];
static char filename[BUFSIZ];

static char fontList [BUFSIZ];


/* procedures */
FILE *scanfonts ();
FILE *openFontFile();

extern char *malloc ();
extern char *strcpy();
extern char *strcat();
extern char *getenv();


/*
 * loads the font dictated by the troff input into the family
 * structure.  This doesn't do anything but make the name known.
 */
getfamily()
{
	register char *s;
	register c, i;

	/* skip over possible "ont", as in "font" */
	while ((c = inchar()) != ' ')
	    /* empty */;

	/* get mount position */
	i = inint();

	/*
	 * skip over what?  This probably exits immediately.
	 * But it gobbles one space.
	 */
	while ((c = inchar()) != ' ')
	    /* empty */;

	if (i >= NUMFAMILIES) {
#	    ifdef DEBUG
	    if (debug)
		fprintf (debug, "family number too large: %d\n", i);
#	    endif /* DEBUG */
	    /* eat up family name */
	    while ((c = inchar()) != '\n' && c != ' ')
		/* empty */;
	    return;
	}

	for (s = family[i]; (c = inchar()) != '\n' && c != ' '; *s++ = c)
	    /* empty */;

	*s = 0;

#	ifdef DEBUG
	    if (debug && verbose)
		fprintf (debug, "newfamily: is %s\n", family [i]);
#	endif /* DEBUG */
}

/*
 * sets the new point size, as dictated by the troff input
 */
newsize()
{
	extern double scalefac;
	extern int scaled;

	/* read the desired point size from troff */
	ptsz = inint();

	/*
	 * if we're in scale mode, scale the request, so it's
	 * appropriate for our needs.
	 */
	if(scaled)
		ptsz = ptsz * scalefac;

	/*
	 * Generate the name of the font file that will contain
	 * the necessary font.
	 * e.g. this generates an fname that might appear like:
	 *	R.8
	 */
	(void)sprintf(fname, "%s.%d", family[famnum], ptsz);

	/* load the font before printing again */
	newfont = 1;
}

/*
 * requests that the mounted font 'n' be used, as dictated by the
 * troff input.
 */
newfamily()
{
	/* get requested mount table entry number from troff */
	famnum = inint();
	if (famnum < 0)
		famnum = 0;

	/* (see comment in newsize, above) */
	(void)sprintf(fname, "%s.%d", family[famnum], ptsz);

	/* load the font before printing again */
	newfont = 1;
}

Font
getfont(inf)
	FILE *inf;
{
	register c, i, n, *p;
	Font f;


	/*
	 * read n, the first short of the requested font file:
	 * it holds the number of chars in the font (wouldnt't
	 * it be easier to use a getw()? - maybe this version is more
	 * machine independent).
	 */
	c = getc(inf);
	n = (c<<8) | (getc(inf)&0377);

	/*
	 * the host version of the font data structure contains merely
	 * a pointer to an array of shorts - one for each char.
	 */
	f.width = (int *) malloc(n*sizeof(int));
	if (f.width == NULL) {
		/*
		 * there's no more ram left. Use the default font.
		 */
		(void)fclose(inf);
		return defont;
	}

	f.height = (unsigned int)getc (inf);
	f.ascent = (unsigned int)getc (inf);

	/* ignore the height, ascent, and unused fields of the font file */
	for (i = 0; i < 4; i++)
		(void)getc(inf);

	/*
	 * now read the Fontchar array in the font file: note that the
	 * font file is organized differently from the Font data
	 * structure: the info field precedes the Bitmap field.
	 */
	p = f.width;
	do {
		/* ignore the x, top, bottom, and left fields */
		for (i = 0; i < 5; i++)
			(void)getc(inf);

		/* get the "width" field */
		*p++ = getc(inf);

		/*
		 * For DMD , skip past 2 chars we padded with for infont!
		 *
		 */
		(void)getc(inf);
		(void)getc(inf);
	} while (--n > 0);

	/*
	 * done with the font file for now:
	 * Leave it the way you left it.
	 */
	rewind (inf);

	/* return with the font data structure */
	return(f);
}


getstate()
{
	char command[BUFSIZ];
	register i;
	char *env;
	extern int notmpx;
	char *fontPath;
	FILE *defp;		/* default font file pointer */
	extern char *Progname;


	/*
	 * Build the paths to the font directories
	 */

	fontList[0] = '\0';
	if ((fontPath = getenv ("FONT")) != NULL)
		strcpy(fontList, fontPath);

	/*
	 * add $DMDLIB/../xfont and $DMD/font to the fontList
	 */
	if ((env = getenv("DMDLIB")) != NULL) {
		if (fontList[0] != '\0')
			(void)strcat(fontList, ":");
		(void)strcat(fontList, env);
		(void)strcat(fontList, "/../xfont");
	}
	if ((env = getenv("DMD")) == NULL) {
#ifdef DEFDMD
		env = DEFDMD;
#endif
	}
	if (env != NULL) {
		if (fontList[0] != '\0')
			(void)strcat(fontList, ":");
		(void)strcat(fontList, env);
		(void)strcat(fontList, "/font");
	}
#	ifdef DEBUG
	    if(debug) {
		(void)fprintf(debug,
		    "getstate: font list: %s\n", fontList);
	    }
#	endif /* DEBUG */

	defp = openFontFile ("defont");
	if (defp == NULL) {
		fprintf (stderr, "getstate: couldn't find default font file - set FONT environment var\n\r");
		return NoDefont;
	}

	/*
	 * The following 2 clauses - up to the test of DMDTERM - were
	 * previously enclosed in the if-clause of whether proof is
	 * already loaded in the terminal.  I don't follow that.  That
	 * would indicate that the various font information wouldn't be
	 * initialized when proof is executed from proofterm.  That seems
	 * to be wrong, although I can't find anywhere where it would be
	 * catastrophic, in order to test my theory.  The reason for this
	 * appears to be that the information stored in font structures
	 * on the host are really used for very little.
	 */

	/*
	 * getfont() gets and returns a font.  If it fails, it returns
	 * defont.  If it's failed to get the default font
	 * (i.e. the following call fails), the returned value
	 * (i.e. the current value of defont) is zero, which
	 * is presumably an invalid font.  It is also to be
	 * presumed that nobody has set up defont by this time. ??
	 * Incidently, getfont() can fail only because of lack of space.
	 */
	defont = getfont(defp);
	if (defont.width == 0) {
		fprintf (stderr,
			"getstate: couldn't find default font file - set FONT environment var\n\r");
		return NoDefont;
	}

	/* fill in the mount table for the default font */
	(void)strcpy(mount[0].name,"defont");
	mount[0].there = 1;
	mount[0].font = defont;

	/* clear the rest of the mount table */
	for(i = 1; i < NFONTS; i++)
		mount[i].name[0] = 0;

	/*
	 * Determine what terminal we're talking to in order to
	 * tailor the protocol accordingly.
	 */
	if ((env = getenv ("DMDTERM")) != NULL) {
	    if (Eq (env, "dmd"))
		proofterm = 1;
	}

	/*
	 * if proofterm isn't even desired (only possible on 630) -or-
	 * if stdout is not associated with a terminal device (?) -or-
	 * if proof  is not already running in this layer, then
	 *   download it and return 0.
	 *   otherwise, just return 1.
	 *
	 *	stdout	  dmd
	 *     + tty	 absent	- download
	 *     + tty	present - don't
	 *     * pipe	 absent - download
	 *     * pipe	present - download
	 * * but don't forget: if the stdout is piped, the following
	 *   ioctl will yield a non-layered device, so proof.j will
	 *   be loaded! (disabled)
	 * + These are the only cases that are really interesting.
	 *
	 * If caching is active, then there is no necessity to check
	 * whether proof.m is resident on the terminal: all that
	 * logic is handled by dmdld.  Thus, dmdld should always be
	 * called, and the garbage of polling the terminal for proof
	 * can be eliminated.  Since proofterm is not supported for
	 * the 630 - then when !proofterm, try to download.
	 */
	/*			   stdout==pipe  dmd==absent */
	if (!proofterm || !isatty(1) || !verify()) {
		/*
		 * build the command to download and run proof.
		 * If dmdtool variables DMDLD and DMDLIB are not set,
		 *  default to "dmdld" and $DMD/lib respectively.
		 */
		char *dmdld, *dmdlib;
		if ((dmdld = getenv ("DMDLD")) == NULL)
			dmdld = "dmdld";
		if ((dmdlib = getenv ("DMDLIB")) == NULL) {
			char dmdlibbuf[256];
			if ((env = getenv ("DMD")) == NULL) {
#ifdef DEFDMD
				env = DEFDMD;
#else
				fprintf(stderr, "$DMD must be set\n");
				return NoDMD;
#endif
			}
			strcpy(dmdlibbuf, env);
			strcat(dmdlibbuf, "/lib");
			dmdlib = dmdlibbuf;
		}

		/*
		 * Redirect stdin, stdout, and stderr to a file descriptor
		 *    that is a tty.
		 */
		(void)sprintf(command,
				"%s %s %s/%s.m %s %s <&%d >&0 2>&0",
			dmdld,
			debugOpt? "-z" : "",
			dmdlib,
			Progname,
			cacheText? "-c" : "",
			proofterm? "-p" : "",
			jerq);

#		ifdef DEBUG
		    if (debug)
			(void)fprintf (debug, "%s\n", command);
#		endif /* DEBUG */

		if(system(command)!=0){
			perror ("getstate: proof couldn't load: ");
			fprintf (stderr, "\n\r");
			/* retcode indicates that proof must exit gracefully */
			return LoadFailed;
		}

		/* retcode indicates that proof was just loaded */
		return LoadComplete;
	}
	/* retcode indicates that proof was previously loaded */
	return AlreadyLoaded;
}


#ifdef DEBUG
	static short caught;
#endif /* DEBUG */

alarmcatch()
{
	caught = True;
}

/* returns True if proof running */
verify()
{
	register rval;
	char c;

#	ifdef DEBUG
	    caught = False;
#	endif /* DEBUG */
	(void)signal(SIGALRM, alarmcatch);

	c=REQ;		/* host to term req for NAK if proof running */
	(void)write(jerq, &c, 1);

	(void)alarm(3);			/* set up for time out */
	(void)read(jerq, &c, 1);	/* if alarm c is still a REQ (ACK) */
	(void)alarm(0);

	rval = (c == NAK);
#	ifdef DEBUG
	    if(debug)
		(void)fprintf(debug,
		    "verify: read 0%o%s\n",
		    c, caught? ", #### Timed out ####" : "");
#	endif /* DEBUG */

	if(rval)
		(void)read(jerq, &c, 1);	/* suck up newline */

	return rval;
}

int findfont(name)
char *name;
{
	register char *s;
	register Mount *m;
	FILE *ouf = NULL;
	int c;
	extern int scaled;
	int retcode;

#	ifdef DEBUG
	    if(debug)
		(void)fprintf(debug,"findfont: font '%s' needed\n",name);
#	endif /* DEBUG */

	/* reset flag that says we need to download a font */
	newfont = 0;

	/*
	 * Look for the requested font in the mounted-fonts table.
	 */
	for(m = mount; m != &mount[NFONTS]; m++) {
		/* skip unused mount table entries */
		if(m->name[0] == 0)
			continue;

#		ifdef DEBUG
		    if(debug && verbose)
			(void)fprintf (debug,
			    "findfont: mount table entry %d: name %s%s, font is %x\n",
			    m-mount,
			    m->name,
			    m->there? " is there" : " is missing",
			    m->font.width);
#		endif /* DEBUG */

		if (Eq (name, m->name)) {
			/*
			 * if the sought font name is in the mount tbl
			 * but the flag that indicates that it is
			 * "there" is false, then use mount tbl entry 0
			 */
			if(m->there == 0)
				m = mount;

			/*
			 * if this is the font that is current, then
			 * don't bother sending anything to the term.
			 */
			if (curfont.width == m->font.width)
				return(1);	/* font ready */

			/*
			 * if the font is in the host-side's table,
			 * it's gotta be in the term, too.  So just
			 * make the found name current, and notify
			 * the terminal.
			 */
			curfont = m->font;
			outchar(FONT);
			outchar(m-mount);
#			ifdef DEBUG
			    if(debug)
				(void)fprintf(debug,
				    "findfont: select font %d\n",m-mount);
#			endif /* DEBUG */
			return(1);		/* font ready */
		}
	}

	/*
	 * notify the slave that a new font is needed, and
	 * what its name will be.
	 */
	outchar(NEWFONT);
	for(s = name; *s; s++)
		outchar(*s);
	outchar(EOT);

#	ifdef DEBUG
	    if(debug)
		(void)fprintf(debug, "findfont: ask for font '%s'\n",name);
#	endif /* DEBUG */

	/*
	 * get the slave's response, including the ftab index.
	 * The ftab index is where the font is located in the
	 * terminal's ftab, or else where it'll go if it's not
	 * already there.
	 */
	c = interm();		/* either ACK or GETFONT */
	m = &mount[interm()];	/* ftab index */

	/* enter the font name into the font table */
	(void)strcpy(m->name,name);

	/* build the host font file name and try to open it */
	ouf = scanfonts (name);

	/* load the font */
	if(ouf != NULL) {
		/*
		 * get the host version of the font: an array of
		 * the widths of each char in the font.
		 *
		 * Note that it is possible that the terminal already
		 * has this font: that will be determined when "c" is
		 * tested shortly.  In any case, this call to
		 * getfont() makes the font known to the host side.
		 */
		m->font = getfont (ouf);
		curfont = m->font;
		m->there = 1;
		if(c == GETFONT) {
			outchar(ACK);
			sendfont(ouf);
#			ifdef DEBUG
			    if(debug)
				(void)fprintf(debug,
				    "findfont: (entry %d) sent ACK and font\n",
				    m-mount);
#			endif /* DEBUG */
		}
		/* else, the terminal already has the font */

		(void)fclose(ouf);	/* font ready */
		
		retcode = 1;		/* print out debug msg at bottom */
	}
	/* the font file couldn't be opened.  Use the default */
	else {
#		ifdef DEBUG
		    if(debug)
			(void)fprintf(debug,
			    "findfont: fopen failed on %s: %d\n",
			    filename, errno);
#		endif /* DEBUG */
		m->font = defont;
		curfont = m->font;
		m->there = 0;
		if(c == GETFONT) {
			/* abort the font download */
			outchar(NAK);

			/* request the default font */
			outchar(FONT);
			outchar(0);
#			ifdef DEBUG
			    if(debug)
				(void)fprintf(debug,
				    "findfont: send NAK (entry %d), font 0\n",
				    m-mount);
#			endif /* DEBUG */
		}
		(void)fclose(ouf);
		if((ouf = fopen("/tmp/.missing","a")) != NULL) {
			(void)fprintf(ouf,"%s\n",name);
			(void)fclose(ouf);
		}
		retcode = 0;	/* font missing (defont ready) */
	}

#	ifdef DEBUG
	    if (debug)
		(void)fprintf (debug, "findfont: font send completed\n");
#	endif /* DEBUG */

	return retcode;
}


FILE *openFontFile (name)
	char *name;
{
	register char *fontdir = fontList;	/* font dir list pointer */
	FILE *ouf = NULL;


	 while ((fontdir != NULL) && (*fontdir != '\0')) {
		register char *fdnptr = filename;	/* font dir name */

		/* transfer next font dir to font file name area */
		while (*fontdir != ':' && *fontdir != '\0') {
			if (!isspace (*fontdir)) {
				*fdnptr = *fontdir;
				fdnptr++;
			}
			fontdir++;
		}

		/*
		 * set fontdir to next font dir possibility
		 * If *fontdir is not ':', then it must be a string
		 * terminator, which will be detected by while loop.
		 */
		if (*fontdir == ':')
			fontdir++;

		/* found end of font list.  Terminate name so far */
		*fdnptr = '\0';

		/*
		 * if null font dir specified, ignore:
		 * there are no fonts in the root dir!
		 */
		if (*filename == '\0')
			continue;

		/* convert font dir name to font file name for test */
		(void)strcat (filename, "/");
		(void)strcat (filename, name);

#		ifdef DEBUG
			if (debug && verbose)
				(void)fprintf (debug,
					"openFontFile: test font file %s\n",
					filename);
#		endif /* DEBUG */

		/* try to open the desired font file */
		if((ouf = fopen(filename, "r")) != NULL)
			break;
	};

#	ifdef	DEBUG
	    if(debug)
		(void)fprintf(debug,
			"openFontFile: open font file %s\n", filename);
#	endif	/* DEBUG */

	return ouf;
}


sendfont(inf)
	FILE *inf;
{
	register n;
	register char *p;
	char inbuf[BUFSIZ];

	while ((n = fread (inbuf, 1, BUFSIZ, inf)) > 0) {
		for (p = inbuf; n-- > 0;)
			outchar (*p++);
	}
}


/*
 * This routine will check the font directory for as close of a match
 * as possible for the desired font.  If a match is not found after
 * about twenty point sizes away,  the default font will be used.
 * Thanks to Steve Keefe for the essence of this algorithm.
 */
FILE *scanfonts(name)
	char *name;
{
#	define	Smaller	0
#	define	Bigger	1
	FILE	*ffp;	/* font file pointer */
	int	newsz = ptsz;
	int	direction = Smaller;
	int	tries = 1;
	char    fontbuf [BUFSIZ];


	while (tries < MaxTries) {
		if (name[0] == 'S') {
		    sprintf(fontbuf, "S.%d/%s", newsz, specchar);
		}
		else {
		    sprintf(fontbuf, "%s.%d", family[famnum], newsz);
		}

#		ifdef DEBUG
		    if (debug && verbose)
			fprintf (debug,
			    "scanfonts: Try to open %s, direction is %s\n",
			    fontbuf, direction? "Bigger" : "Smaller");
#		endif /* DEBUG */

		if ((ffp = openFontFile (fontbuf)) == NULL) {
			/* We must look for another size font */
			switch(direction) {
			case Smaller:
				/* try a smaller value */
				if((newsz = ptsz - tries) <= 0) {
					/* 
					* there are no fonts smaller so try
					* another larger font.
					*/
					newsz = ptsz + tries;
					tries++;
				}
				direction = Bigger;
				break;
			case Bigger:
				/* try a larger size */
				newsz = ptsz + tries;
				tries++;
				direction = Smaller;
				break;

			}	/* end case */
		}		/* end if */
		else
			/* found a suitable font */
			break;
	}			/* end while */

	return ffp;
}

