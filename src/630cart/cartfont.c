/*       Copyright (c) 1989 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


#include <stdio.h>
#include <string.h>

/*
**	Author: James Grenier
**
**	Date: 4/25/88
**
**	This program creates a file suitable for placing in a cartridge
**	for the 630 MTG. The program created will put into the 630's
**	font cache those fonts specified on stdin. It uses a slightly
**	modified version of "fontconvert" to generate the C declaration
**	of a font file produced by jf.
**
**	Format of stdin file:
**
**	<font file name>	[<font menu name>]
**		.			.
**		.			.
**		.			.
**
**	<font file name> is the path name to the font. The font file
**	must have been produced by jf. The format is the same as that
**	used by the applications cip and proof.
**
**	<font menu name> is the name used by the terminal for the font.
**	This is the name that will appear on wproc's button 2 font submenu.
**	If is contains spaces, the name must be quoted ("). If no name
**	is given, the font file name is used.
**
**	This program creates the file font.c by running fontconvert on
**	each of the font files. (It assumes fontconvert is in your path.)
**	Then it compiles it with dmdcc into font.o. Then it runs mc68ld
**	on this to produce font.C for the cartridge.
**
**	On stdout it writes "font.C null main" for another cartridge
**	building tool to use.
*/

char fontmenu[100][60];		/* font menu names */
int numfonts;			/* number of fonts */
char *progname;			/* name of this program */
int dflag;			/* debugging */

#define isspace(c) (((c) == ' ') || ((c) == '\t') || ((c) == '\n'))

main(argc, argv)
int argc;
char **argv;
{
	FILE *fout;
	char buf[256];

	progname = argv[0];
	if(argc > 1 && strcmp(argv[1], "-d") == 0)
		dflag++;
	if((fout = fopen("font.c", "w")) == 0)
		error("unable to open font.c");
	if(fprintf(fout, "\n#include <dmd.h>\n#include <font.h>\n#include <object.h>\n\n")
	    < 0 || fflush(fout))
		error("unable to write to font.c");
	while(gets(buf) == buf)
		addfont(buf);
	fout = freopen("font.c", "a", fout);	/* to keep things in sync */
	printprog(fout);
/***	compile(); ***/
	printf("font.c null main 3 %01d font.c\n",dflag);
	exit(0);
}

addfont(s)
char *s;
{
	char fontfile[128];
	char command[256];
	char *t;

	while(*s && isspace(*s)) s++;
	if(*s == '\0')
		return;
	t = fontfile;
	while(*s && !isspace(*s))
		*t++ = *s++;
	*t = '\0';
	while(*s && isspace(*s)) s++;
	t = fontmenu[numfonts];
	if(*s == '\0')
	{ /* use the font file name */
		*t++ = '"';
		if(strrchr(fontfile, '/'))
			strcpy(t, strrchr(fontfile, '/') + 1);
		else
			strcpy(t, fontfile);
		strcat(t, "\"");
	} else if(*s == '"')
		strcpy(t, s);
	else
	{
		*t++ = '"';
		while(*s && !isspace(*s))
			*t++ = *s++;
		*t++ = '"';
		*t = '\0';
	}
	sprintf(command, "$DMDTOOLS/dmdtools/xfontconvert -h -n F%d -j %s >> font.c", numfonts, fontfile);
	fprintf(stderr, "executing: %s\n", command);
	if(system(command))
		error("xfontconvert failed");
	numfonts++;
}

error(s)
char *s;
{
	fprintf(stderr, "%s: %s\n", progname, s);
	exit(1);
}

printprog(fout)
FILE *fout;
{
	int i;

	fprintf(fout, "\n");
	fprintf(fout, "\n");
	fprintf(fout, "struct fnt {\n");
	fprintf(fout, "	Font *f;\n");
	fprintf(fout, "	char *name;\n");
	fprintf(fout, "};\n");
	fprintf(fout, "\n");
	fprintf(fout, "struct fnt newfont[] = {\n");
	for(i=0; i<numfonts; i++)
		fprintf(fout, "	(Font *)&F%d, %s,\n", i, fontmenu[i]);
	fprintf(fout, "};\n");
	fprintf(fout, "\n");
	fprintf(fout, "main()\n");
	fprintf(fout, "{\n");
	fprintf(fout, "	Obj *obj;\n");
	fprintf(fout, "	Obj *f_cache();\n");
	fprintf(fout, "	Obj *findfont();\n");
	fprintf(fout, "	struct fnt *s;\n");
	fprintf(fout, "	struct fnt *ends;\n");
	fprintf(fout, "\n");
	fprintf(fout, "	_addrSys = *((long *)((*(long *)176) + 6));\n");
	fprintf(fout, "	ends = newfont + (sizeof(newfont)/sizeof(struct fnt));\n");
	fprintf(fout, "	for(s=newfont; s < ends; s++)\n");
	fprintf(fout, "	{\n");
	fprintf(fout, "		if(!findfont(s->name) && (obj = f_cache(s->name,s->f)))\n");
	fprintf(fout, "			if(!reqobj(P, obj))\n");
	fprintf(fout, "				uncache(obj);\n");
	fprintf(fout, "	}\n");
	fprintf(fout, "}\n");
	fprintf(fout, "\n");
	fprintf(fout, "\n");
	fflush(fout);
	if(ferror(fout))
		error("failed to write main program into font.c");
}

compile()
{
	if(system("dmdcc -c font.c"))
		error("dmdcc failed");
	if(!dflag)
		unlink("font.c");
	if(system("mc68ld -o font.C -L$DMD/lib -r $DMD/lib/crtm.o font.o -ljx -lj -lfw -lc"))
		error("mc68ld failed");
	if(!dflag)
		unlink("font.o");
}



