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
#include <stdio.h>
#include <sys/types.h>
#include <filehdr.h>
#include <scnhdr.h>
#include <reloc.h>
#include <string.h>
#include <fcntl.h>

/*******************************************************************
**
**	author: James Grenier
**
**	date: 4/23/88
**
**	This program takes a dmda.out file and puts the text and
**	data sections into a C array. It also puts the relocation
**	information into a C array. The it declares the text,
**	data, and bss section sizes as C variables.
**
**	Then it adds a program that takes these C arrays and copies
**	them into RAM and does relocation to run the program.
**
**	A possible future enhancement is to compress the dmda.out
**	text and data sections first.
**
*********************************************************************/

#include "cartglob.h"

long relocstart;

main(argc, argv)
int argc;
char **argv;
{
	int i, c;
	struct scnhdr scnhdr;
	struct filehdr filehdr;
	extern char *optarg;
	extern int optind, opterr;

	if(argc < 2)
		exit(0);

	strcpy(shared, "PERMANENT|SHARED");	/* always shared in ram */
	cartopt(argc,argv);

	/*
	** start reading in the file
	*/

	if(fread(&filehdr, sizeof(filehdr), 1, stdin) != 1)
		error("cannot read file header");
	if(filehdr.f_magic != MC68MAGIC)
		error("not a 68000 a.out file");
	for(i=0; i<filehdr.f_nscns; i++)
	{ /* read in all the section headers */
		if(fread(&scnhdr, sizeof(scnhdr), 1, stdin) != 1)
			error("cannot read section header");
		if(strcmp(scnhdr.s_name, ".cprs") == 0)
			error("compressed files are not supported");
		if(strcmp(scnhdr.s_name, ".text") == 0)
		{
			inbytes += scnhdr.s_size;
			numreloc += scnhdr.s_nreloc;
			sizeof_text = scnhdr.s_size;
			relocstart = scnhdr.s_relptr;
		}
		if(strcmp(scnhdr.s_name, ".data") == 0)
		{
			inbytes += scnhdr.s_size;
			numreloc += scnhdr.s_nreloc;
			sizeof_data = scnhdr.s_size;
		}
		if(strcmp(scnhdr.s_name, ".bss") == 0)
			sizeof_bss = scnhdr.s_size;
	}

	/*
	** start outputing the file
	*/

/*
**  This was moved to the routine printglobals.
**  -jrg
**
	if(printf("\n#include <dmd.h>\n#include <object.h>\n\n") < 0)
		error("cannot write to output file");
**
*/
	makearray();
	makereloc();
	printglobals();
	printprog();
	printinit();

	/*
	** compile the program
	** NOW DONE IN make_cart
	*/
	/* makeofile(); */
	/* makeCfile(); */

	fprintf(realstdout, "%s null init_appl 1 %01d %s\n",outfile,dflag,outfile);
	exit(0);
}


cartopt(argc,argv)
int argc;
char **argv;
{
	int i,c,fd,len;
	char chr,*cp;

	progname = argv[0];
	infile = argv[1];
	opterr = 1;	/*disable optional error message display */
	optind = 2;	/*start with second argument*/
	while((c = getopt(argc, argv, "dm:M:tTcClLsSr:R:v:V:z:Z:")) != EOF)
		switch(c) {
		case 'd':
			dflag++;
			break;
		case 'm':
		case 'M':
			strcpy(menu_name, optarg);
			mflag++;
			break;
		case 't':
		case 'T':
			tflag++;
			break;
		case 'c':
		case 'C':
			if(lflag) {
				fprintf(stderr, "%s: both 'L' and 'C' flag specifiled for file '%s'\n",
					argv[0], infile);
				exit(1);
			}
			cflag++;
			break;
		case 'l':
		case 'L':
			if(cflag) {
				fprintf(stderr, "%s: both 'L' and 'C' flag specifiled for file '%s'\n",
					argv[0], infile);
				exit(1);
			}
			lflag++;
			break;
		case 's':
		case 'S':
			sflag++;
			break;
		case 'r':
		case 'R':
			if(sscanf(optarg, "%d,%d", &width, &height) != 2)
			{
				fprintf(stderr, "%s: default rectangle given bad argument\n",
				    argv[0]);
				exit(1);
			}
			rflag++;
			break;
		case 'v':
		case 'V':
			vflag++;
			argvs = optarg;
			break;
		case 'z':
		case 'Z':
/*
			if ( (len = strlen(optarg)) > 8 ) {
			   fprintf(stderr,"%s: stack: more than 8 characters in input\n", argv[0]);
			   exit(1);
			}
*/
			cp=optarg;
			if ( *cp == '+' || *cp == '-') cp++;
			while ( (chr = *cp++) ) {
			   if ( chr <  '0' || chr >  '9' ) {
			   	fprintf(stderr,"%s: stack: not a positive integer\n", argv[0]);
			   	exit(1);
			   }
			}
			zflag=atol(optarg);
/*
			if ( zflag > 9999999l  ) {
		 	   fprintf(stderr,"%s: stack: value out of bound\n", argv[0]);
			   exit(1);
			}
*/
			break;	
		case '?':
			fprintf(stderr, "%s: bad option\n", argv[0]);
			exit(1);
			break;
		}

	/* done parsing options, now get to work */
	
	freopen(infile, "r", stdin);
	if((realstdout = fdopen(fd = dup(1), "w")) == 0)
		error("couldn't copy standard out");
	fcntl(fd, F_SETFD, 1);
	if(strrchr(infile, '/'))
		strcpy(outfile, strrchr(infile, '/') + 1);
	else
		strcpy(outfile, infile);
	if(!vflag)
	{
		argvs = argvbuf;
		strcpy(argvbuf, "\"");
		strcat(argvbuf, outfile);
		strcat(argvbuf, "\"");
	}
	if(!mflag)
		strcpy(menu_name, outfile);
	if(strlen(outfile) > 12)
		outfile[12] = '\0';
	strcat(outfile, ".c");
	freopen(outfile, "w", stdout);

/* Next four lines of code not needed anymore */
	strcpy(ofile, outfile);
	ofile[strlen(ofile) - 1] = 'o';
	strcpy(Cfile, ofile);
	Cfile[strlen(Cfile) - 1] = 'C';

	if(!rflag)
		sflag++;	/* default to sweepable */
}


makereloc()
{ /* generate the C array for relocation information */
	struct reloc reloc;

	if(printf("\nlong reloc[] = {\n") < 0)
		error("cannot write relocation array");
	fseek(stdin, relocstart, 0);
	while(numreloc--)
	{ /* read in all the relocation entries */
		if(fread(&reloc, RELSZ, 1, stdin) != 1)
			error("cannot read relocation information");
		if(reloc.r_type != R_RELLONG)
			error("relocation type unknown");
		/* only the r_vaddr field is important */
		if(printf("\t0x%.6x,\n", reloc.r_vaddr) < 0)
			error("cannot write relocation array");
	}
	if(printf("};\n") < 0)
		error("cannot write relocation array");
}

makearray()
{ /* generate the text/data C array */
	int i;
	unsigned short c;

	if(printf("\nshort text_data[] = {") < 0)
		error("cannot write text_data array");
	for(i=0; i<inbytes; i+=2)
	{ /* read in all the text/data information */
		if((i%16) == 0)
			if(printf("\n") < 0)
				error("cannot write text_data array");
		if(fread(&c, 2, 1, stdin) < 1)
			error("cannot read text image");
		if(printf(" 0x%.4x,", c) < 0)
			error("cannot write text_data array");
	}
	if(printf("\n};\n") < 0)
		error("cannot write text_data array");
}
printprog()
{
/*
** print out the body of the program
*/
	printf("\n");
	printf("\n");
	printf("char *lalloc();\n");
	printf("\n");
	printf("/**************************************************************\n");
	printf("**\n");
	printf("**	author: James Grenier\n");
	printf("**\n");
	printf("**	date: 4/23/88\n");
	printf("**\n");
	printf("**	This program copies another program into ram, does\n");
	printf("**	the necessary relocation, and executes it. This\n");
	printf("**	program is intended to be part of a program that\n");
	printf("**	puts applications into a cartridge.\n");
	printf("**\n");
	printf("**	A possible future enhancement is to support a compressed\n");
	printf("**	text/data image.\n");
	printf("**\n");
	printf("****************************************************************/\n");
	printf("\n");
	printf("main(argc,argv)\n");
	printf("int argc;\n");
	printf("char **argv;\n");
	printf("{\n");
	printf("	register char *tdb;		/* text, data, bss image in ram */\n");
	printf("	register char *p, *s, *endp;\n");
	printf("	register int i;\n");
	printf("	register Obj *obj;\n");
	printf("	register char **t, **newargv;\n");
	printf("	register long argsize;\n");
	printf("\n");
	printf("	argsize = (argc + 1) * sizeof(char *);\n");
	printf("	for(p=argv[0],i=argc; i; i--, p++)\n");
	printf("		argsize += strlen(p) + 1;\n");
	printf("	newargv = (char **)lalloc(argsize);\n");
	printf("	tdb = lalloc(__size_text + __size_data + __size_bss);\n");
	printf("	if(tdb == 0 || newargv == 0)\n");
	printf("	{ /* failed to allocate the ram image */\n");
	printf("		request(KBD);\n");
	printf("		string(&largefont, \"alloc failed, hit any key to continue\",\n");
	printf("		    &display, Drect.origin, F_STORE);\n");
	printf("		wait(KBD);\n");
	printf("		exit();\n");
	printf("	}\n");
	printf("	t = newargv;\n");
	printf("	s = ((char *)newargv) + (argc + 1) * sizeof(char *);\n");
	printf("	for(i=0; i<argc; i++)\n");
	printf("	{\n");
	printf("		*t++ = s;\n");
	printf("		strcpy(s,argv[i]);\n");
	printf("		s += strlen(s) + 1;\n");
	printf("	}\n");
	printf("	P->uargv = newargv;\n");
	printf("	endp = tdb + __size_text + __size_data;\n");
	printf("	for(p=tdb, s=(char *)text_data; p<endp; p++, s++)\n");
	printf("		*p = *s;	/* copy saved image to ram */\n");
	printf("	for(i=0; i<sizeof(reloc)/4; i++)	/* do relocation */\n");
	printf("		*(long *)(tdb + reloc[i]) += (long)tdb;\n");
	printf("	/* remove my request for this application object */\n");
	printf("	unreqcache(&syscache[APPLCACHE], P);\n");
	printf("	/* update text, data, bss so that dmdpi can work on the new image */\n");
	printf("	P->text = tdb;\n");
	printf("	P->data = tdb + __size_text;\n");
	printf("	P->bss = tdb + __size_text + __size_data;\n");
	printf("	if(__shared && (obj = findobj(\"s\", __applargv[0])))\n");
	printf("	{ /* use the RAM copy from now on*/\n");
	printf("		obj->info.cappl->text = P->text;\n");
	printf("		obj->info.cappl->data = P->data;\n");
	printf("		obj->info.cappl->bss = P->bss;\n");
	printf("		allocown(tdb, (char *)0);\n");
	printf("	}\n");
	printf("	asm(\"jmp (%%a2)\");	/* execute the program */\n");
	printf("	/* WARNING --- this program assumes tdb is register a2 */\n");
	printf("}\n");

	if(ferror(stdout))
		error("cannot write out the body of the program");
}

/*
** compile the .c file into a .o file
*/

/* Note commented out ....
makeofile()
{
	char command[128];

	strcpy(command, "dmdcc -c ");
	strcat(command, outfile);
	freopen("/dev/tty", "w", stdout);
	if(system(command))
		error("dmdcc failed");
	if(!dflag)
		unlink(outfile);
}
*/

/*
** make a .C file from the .o file
** this requires special options to the loader
*/

/* Note Commented out
makeCfile()
{
	char command[256];

	strcpy(command, "mc68ld -o ");
	strcat(command, Cfile);
	strcat(command, " -L$DMD/lib -r $DMD/lib/crtm.o ");
	strcat(command, ofile);
	strcat(command, " -ljx -lj -lfw -lc");
	if(system(command))
		error("mc68ld failed");
	fprintf(realstdout, "%s null init_appl 1\n", Cfile);
	if(!dflag)
		unlink(ofile);
}
*/
