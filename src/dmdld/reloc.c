/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */

/* @(#)reloc.c	1.1.1.1	(5/2/89) */

/* reloc.c
**
** fast relocator routines for 630
*/
#include <stdio.h>
#include <filehdr.h>
#include <reloc.h>
#include <scnhdr.h>

#ifndef R_RELLONG  /* In case /usr/include/reloc.h does not know about 68K sgs */
#define R_RELLONG	021
#endif

extern char *malloc();

char *imagebuf;
extern unsigned long imagesize;
extern unsigned long relocinfosize;
extern unsigned long reloc_offset;
extern unsigned long physaddr;
extern int debug;

/* relocinit - Alloc memory and read image to be downloaded along with
** relocation information.
**
** There are assumptions here about 32bit integers. With 16 bit integers,
** we would not be able to relocate a file with imagesize
** greater than 64K without modifying this code. This is an unfortunate
** effect of malloc() and sbrk() taking an int rather than long for their
** size argument.
*/
relocinit()
{
	unsigned long readsize;
	extern struct filehdr fileheader;

	readsize = imagesize + relocinfosize;
	if( (imagebuf = malloc(readsize)) == NULL)
		error(0, "Program too large to relocate", (char *)0);
	Read(imagebuf, readsize);
	/* All of the pointers in the COFF file are relative to the
	** beginning of the file. Imagebuf only contains raw data and
	** relocation information, which means it does not contain filehdr
	** and section headers. Here I adjust the imagebuf pointer to where
	** it would be if the buffer started at the beginning of the file,
	** so that file pointers will be easier to deal with in relocseg()
	** below. Pretty tricky, huh?
	*/
	imagebuf -= (sizeof(struct filehdr)+(fileheader.f_nscns*sizeof(struct scnhdr)));
}

/* relocseg() - relocate a section
**
** This routine relocates the section specified by the section pointer sp passed
** as a parameter. The section is relocated to the address stored in the global
** variable reloc_offset. The routine returns a pointer to the beginning of
** the memory buffer containing the relocated code.
**
** Actual relocation is quite simple. Go through each relocation entry and
** add reloc_offset to the text pointed to by reloc.r_vaddr. Files ready to
** be downloaded will have no undefined external references, so the symbol
** table can be ignored completely.
** 
** Much of the complexity of this routine stems from the fact that some hosts
** (like 3B's) can only do long operations on mod 4 boundries. Since the 68000
** can do long operations on mod 2 boundries, we have problems. This has two
** effects:
** 
** 	1) The reloc structure is 10 bytes, and is stored in the COFF file
** 	   as a 10 byte quantity. Therefore, every second structure will
** 	   not be on a mod 4 boundry. This means that the reloc structures
** 	   from the COFF file must be moved to temporary storage before
** 	   being referenced.
** 
** 	2) We are adding reloc_offset to a long quantity. In the
** 	   COFF file, that long quantity may not be on a mod 4 boundry.
** 	   Again, we have to copy to temporary storage before adding
** 	   reloc_offset.
*/
char *
relocseg(sp)
struct scnhdr *sp;
{
	register unsigned short *text;
	/* use char *realrp instead of struct reloc *realrp to avoid gcc
	  optimization of memcpy that assumes alignment on structure pointers */
	char *realrp;
	register int i;
	register char *sva0p; /* section virtual address zero pointer */
	struct reloc rp;
	unsigned long tmp;

	sva0p = imagebuf + sp->s_scnptr - sp->s_vaddr;
	realrp = imagebuf + sp->s_relptr;
	/*copy to pre-allocated structure for alignment required on some hosts*/
	memcpy(&rp, realrp, RELSZ);

	if (debug)
		fprintf (stderr, "\nSection %s: 0x%x relocation entries", sp->s_name, sp->s_nreloc);
	for(i = 0; i < (int) sp->s_nreloc; i++) {

		if(rp.r_type != R_RELLONG) {
			if(debug)
				fprintf(stderr, "reloc type 0%o unknown\n", rp.r_type);
			error(0, "Unknown relocation type", (char *)0);
		}

		/* The following assumes 68000 byte ordering
		** in the file, but it is host independent 
		*/
		text = (unsigned short *) (sva0p + rp.r_vaddr);
		tmp = (*text << 16) + *(text+1);

		/* This check is only needed for takeover download with relocation:
		** If we download the firmware file, the vector table is not downloaded
		** and must not be relocated. However this function is not descriminate
		** and relocate all variables, no matter if they are in the .text, .data
		** or .vector sections.
		** Therefore, we say that all entries that have an address lower than
		** the first address of the file is out of the range of the file,
		** therefore should not be relocated.
		*/  
		if (tmp >= physaddr)
			tmp += reloc_offset;

		*text++ = (unsigned short) (tmp >> 16);
		*text = (unsigned short) tmp;

		realrp += RELSZ;
		memcpy(&rp, realrp, RELSZ);
	}
	sp->s_paddr += reloc_offset;
	return((char *)(imagebuf + sp->s_scnptr));
}
