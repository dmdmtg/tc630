/*       Copyright (c) 1989 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */

#include "cartextn.h"

extern char *optarg;
extern int optind, opterr;


printglobals()
{
/*
** Print out the globals variables.
** Most of these come from the command line options.
*/
	char status[100];

/*
**  These two include files are now always included because both
**  execram and execrom create a new file.
**
**  -jrg
*/
	printf("\n#include <dmd.h>");
	printf("\n#include <object.h>");

	printf("\nlong __size_text = %d;\n", sizeof_text);
	printf("long __size_data = %d;\n", sizeof_data);
	printf("long __size_bss = %d;\n", sizeof_bss);
	printf("\nint __shared = %d;\n", tflag);
	printf("int __noclrbss = %d;\n",bflag);
	printf("\nlong __progid = 1;\n");
	printf("char *__data = (char *)0;\n");
	printf("char *__bss  = (char *)0;\n");
	

	printf("Rectangle __rect = {0, 0, %d, %d};\n", width, height);
	printf("char __menu_name[] = \"%s\";\n", menu_name);
	strcpy(status, "0");
	if(mflag)
		strcat(status, "|SHOWOFF");
	if(sflag)
		strcat(status, "|SWEEP");
	if(!lflag)
		strcat(status, "|CONNECT");
	printf("long __status = %s;\n\n", status);
	printf("char *__applargv[] = {\n\t%s,\n\t0\n};\n\n", argvs);
	if(ferror(stdout))
		error("cannot write out global variables");
}


printinit()
{
	printf("\n");
	printf("void\n");
	printf("init_appl()\n");
	printf("{\n");
	printf("	extern long _addrSys;\n");
	if(iflag)
		printuserinit();
	else
		printdefaultinit();
	printf("}\n");

	if(ferror(stdout))
		error("cannot write out the body of the program");
}

printdefaultinit()
{
	printf("        Appl appl;\n");
	printf("        register Appl *ap = &appl;\n");
	printf("        extern _start();\n");
	printf("        Obj *realcache();\n");
	printf("        extern void greymore(), newmore();\n");
	printf("\n");
	printf("	_addrSys=*((long *)((*(long *)176) + 6));\n");
	printf("        ap->status = __status;\n");
	printf("        ap->text = (char *)_start;\n");
	printf("        ap->data = __data;\n");
	printf("        ap->bss  = __bss ;\n");
	printf("        ap->argc = sizeof(__applargv)/sizeof(char *) - 1;\n");
	printf("        ap->argv = __applargv;\n");
	printf("        ap->stksize =%8d;\n",zflag);
	printf("        ap->progid = __progid;\n");
	printf("        ap->rect = __rect;\n");
	printf("        ap->caption = __menu_name;\n");
	printf("        ap->next = (struct Tmenu *)0;\n");
	printf("        ap->icon = (struct Bitmap *)0;\n");
	printf("        ap->update = greymore;\n");
	printf("        ap->exec = newmore;\n");
	printf("        realcache (&syscache[APPLCACHE], __applargv[0], %s, ap, sizeof(Appl));\n",shared);
	printf("        if (__noclrbss) _svcbss();\n");   
}

printuserinit()
{
	printf("	_addrSys=*((long *)((*(long *)176) + 6));\n");
	printf("	%s();\n", istring);
}
error(s)
char *s;
{
	fprintf(stderr, "%s: %s\n", progname, s);
	exit(1);
}

