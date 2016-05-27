/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)getopt.c	1.1.1.4	(3/31/88)";

/*	@(#)getopt.c	1.5	*/
/*	3.0 SID #	1.2	*/
/*LINTLIBRARY*/

/* defines */
#define NULL	0
#define EOF	(-1)

/* globals */
int	optind;
int	optopt;
char	*optarg;
int	optptr;

/* procedures */
extern int strcmp();
extern char *strchr();


int
getopt(argc, argv, opts)
int	argc;
char	**argv, *opts;
{
	register int c;
	register char *cp;

	if(optptr == 1)
		if(
		  optind >= argc ||
		  argv[optind][0] != '-' ||
		  argv[optind][1] == '\0'
		)
			return(EOF);
		else if(strcmp(argv[optind], "--") == NULL) {
			optind++;
			return(EOF);
		}

	optopt = c = argv[optind][optptr];

	if(c == ':' || (cp=strchr(opts, c)) == NULL) {
		if(argv[optind][++optptr] == '\0') {
			optind++;
			optptr = 1;
		}
		return('?');
	}
	if(*++cp == ':') {
		if(argv[optind][optptr+1] != '\0')
			optarg = &argv[optind++][optptr+1];
		else if(++optind >= argc) {
			optptr = 1;
			return('?');
		} else
			optarg = argv[optind++];
		optptr = 1;
	} else {
		if(argv[optind][++optptr] == '\0') {
			optptr = 1;
			optind++;
		}
		optarg = NULL;
	}
	return(c);
}
