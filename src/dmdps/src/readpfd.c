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
#include "dmdps.h"
#include "pfd.h"

#define BUFSIZE 512
#define MAXTOK	10
char *tokens[MAXTOK];
short t;

#define INULL (Texture16 *)NULL

#define CHARSPERLINE	40
char errstr[CHARSPERLINE];
extern formp();
extern char *octxpnd(/* char * */);

read_pfd(pfd,prname)
struct Printerdefs *pfd;
char prname[];
{
	FILE *filep;
	char buffer[BUFSIZE];
	short c,i;

	filep=fopen(prname,"r");
	if (filep == (FILE *) NULL)
		return(-1);

	while ( (c=getc(filep)) != EOF) {
		for (i=0,t=0;((c !='\n')&&(c != EOF));c=getc(filep)) 
			switch (c) {
			case ' ':		/* token seperator */
			case '\t':
				if (mktok(buffer,i))
					i=0,t++;
				break;
			case '\\': 		/* quote character */
				c=getc(filep); 
				if (c == '\n') 	/* line continuation */
					c=getc(filep); 
				/* no break */
			default:		/* token character */
				buffer[i++]=c;
				break;
			}
		/* finish up the last token */
		if (mktok(buffer,i))
			t++;
		fill_pfd(pfd);
	}
	fclose(filep);
	return 0;
}

mktok(buffer,i)
char buffer[];
short i;
{
	if (i>0) {
		tokens[t] = (char *) alloc(i+1);
		strncpy(tokens[t],buffer,i);
		tokens[t][i]='\0';
		return 1;
	} else
		return 0;
}

freetok(a,b)
int a,b;
{
	if (b<=0)
		b=a;
	for (;a<=b;a++)
		free(tokens[a]);
}

fill_pfd(pfd)
struct Printerdefs *pfd;
{
	int i;

	/* just do it sequentially for now. */
	if ( strcmp(tokens[0],"flowchars") == 0 ) {
		pfd->flowchars=atoi(tokens[1]);
		freetok(1,t-1);
	
	} else if ( strcmp(tokens[0],"swab") == 0 ) {
		pfd->swab=atoi(tokens[1]);
		freetok(1,t-1);
	
	} else if ( strcmp(tokens[0],"flowticks") == 0 ) {
		pfd->flowticks=atoi(tokens[1]);
		freetok(1,t-1);
	
	} else if ( strcmp(tokens[0],"widthes") == 0 ) {
		for ( i = 1 ; i < MAXRES ; i++)
			if (i<t) {
				pfd->width[i-1]=atoi(tokens[i]);
				freetok(i,i);
			} else
				pfd->width[i-1]=pfd->width[0];

	} else if ( strcmp(tokens[0],"initstr") == 0 ) {
		for ( i = 1 ; i < MAXRES ; i++)
			if (i<t) {
				pfd->initstr[i-1]=octxpnd(tokens[i]);
			} else
				pfd->initstr[i-1]=pfd->initstr[0];

	} else if ( strcmp(tokens[0],"rowinit") == 0 ) {
		for ( i = 1 ; i < MAXRES ; i++)
			if (i<t) {
				pfd->rowinit[i-1]=octxpnd(tokens[i]);
			} else
				pfd->rowinit[i-1]=pfd->rowinit[0];

	} else if ( strcmp(tokens[0],"slicesize") == 0 ) {
		pfd->slicesize=atoi(tokens[1]);
		freetok(1,t-1);

	} else if ( strcmp(tokens[0],"passes") == 0 ) {
		pfd->passes=atoi(tokens[1]);
		freetok(1,t-1);

	} else if ( strcmp(tokens[0],"fudge") == 0 ) {
		pfd->fudge=atoi(tokens[1]);
		freetok(1,t-1);

	} else if ( strcmp(tokens[0],"upORdown") == 0 ) {
		pfd->upORdown=atoi(tokens[1]);
		freetok(1,t-1);

	} else if ( strcmp(tokens[0],"resetstr") == 0 ) {
		pfd->resetstr=octxpnd(tokens[1]);

	} else if ( strcmp(tokens[0],"graphicCR") == 0 ) {
		pfd->graphicCR=octxpnd(tokens[1]);

	} else if ( strcmp(tokens[0],"outform") == 0 ) {
		pfd->outform=octxpnd(tokens[1]);

	} else if ( tokens[0][0] != '#' ){ /* not a comment */
		sprintf(errstr,"%s: unknown struct member",tokens[0]);
		formp(INULL,INULL,INULL,errstr);
		freetok(1,t-1);
	}
	freetok(0,0);
}

isdigit(c)
char c;
{
	if ( c <= '7' && c >= '0' )
		return 1;
	else
		return 0;
}

char *octxpnd(str)
char *str;
{
	int i,j;
	char *outstr;

	/* outstr (with help from j) holds the translated str */
	outstr = (char *) alloc( strlen(str)+1) ;
	j=0; 

	for (i=0 ; str[i] ; i++ ) 
	    switch ( str[i] ) {
	    case '\\': 	if (isdigit(str[i+1]) &&  isdigit(str[i+2])
			&&  isdigit(str[i+3])) {
				outstr[j++]=(str[i+1]-'0')*64
					   +(str[i+2]-'0')*8
					   +(str[i+3]-'0');
				i+=3;
			} else
	    			outstr[j++] = str[i];
			break;
	    default: 	outstr[j++] = str[i];
	    }

	outstr[j]='\0';
	strcpy(str,outstr);
	free(outstr);
	return(str);
}

