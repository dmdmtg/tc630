/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/


static char _2Vsccsid[]="@(#)32ld:32reloc.c	2.4";
/* fast relocator for 5620's */

#include "stdio.h"
#include "filehdr.h"	/* on sys V release 1, FBOMAGIC isn't here... */
#ifndef FBOMAGIC
#define FBOMAGIC	0560
#endif
#include "aouthdr.h"
#include "reloc.h"
#include "scnhdr.h"
#include "sys/types.h"
#include "sys/stat.h"
extern char *malloc();

int textorg;
struct stat stb;
char *buf;
struct scnhdr *ts, *ds, *bs;
struct filehdr *fhdr;
long swapb4();

#ifdef TEST
#define DEBUG(X,Y)	printf(X,Y)
#else
#define DEBUG(X,Y)
#endif

main(argc, argv)
char **argv;
{	int fd;
	struct aouthdr *ahdr;
	if(argc != 4) {
		fprintf(stderr, "usage: %s -b addr file\n",argv[0]);
		exit(1);
	}
	sscanf(argv[2], "%x", &textorg);
	fd = open(argv[3], 0);
	if(fd < 0) {
		perror(argv[3]);
		exit(1);
	}
	fstat(fd, &stb);
	if(stb.st_size <= sizeof(struct filehdr)) {
		perror("stdin");
		exit(1);
	}
	buf = malloc(stb.st_size);
	if(read(fd, buf, stb.st_size) != stb.st_size) {
		perror("read");
		exit(1);
	}
	fhdr = (struct filehdr *) buf;
	if(fhdr->f_magic != FBOMAGIC) {
		fprintf(stderr, "reloc: file magic was 0%o expected 0%o\n",
			fhdr->f_magic, FBOMAGIC);
		exit(1);
	}
	if(fhdr->f_nscns != 3) {
		fprintf(stderr, "reloc: file has %d sections instead of 3\n",
			fhdr->f_nscns);
		exit(1);
	}

	ts = (struct scnhdr *) (buf + sizeof(struct filehdr) + fhdr->f_opthdr);
	ds = ts + 1;	/* is this arithmetic ok? */
	bs = ds + 1;
	reloc(ts);
	reloc(ds);
	reloc(bs);
	fhdr->f_flags |= F_RELFLG;

	ahdr = (struct aouthdr *) (buf + sizeof(struct filehdr));
	ahdr->entry += textorg;
	ahdr->text_start += textorg;
	ahdr->data_start += textorg;
	ahdr->vstamp = 0;
	fhdr->f_symptr = fhdr->f_nsyms = 0;
	write(1, buf, stb.st_size);
	exit(0);
}

reloc(sp)
struct scnhdr *sp;
{	struct reloc *rp, copyarea;
	/* use char *realrp instead of struct reloc *realrp to avoid gcc
	  optimization of memcpy that assumes alignment on structure pointers */
	char *realrp;
 	int i;
 	long space;
	unsigned char *to;
	realrp = buf + sp->s_relptr;
	/*copy to pre-allocated structure for alignment required on some hosts*/
	rp = &copyarea;
	memcpy(rp, realrp, RELSZ);
	for(i = 0; i < (int) sp->s_nreloc; i++) {
		switch(rp->r_type) {
		default:
			fprintf(stderr, "reloc type 0%o unknown\n", rp->r_type);
			exit(1);
		case R_DIR32:
			DEBUG(" R_DIR32 (%d)",rp->r_type);
			to = (unsigned char *) (buf + sp->s_scnptr + rp->r_vaddr - sp->s_vaddr);
			/* data is big endian */
			space = (to[0] << 24) + (to[1] << 16) +
				    (to[2] << 8) + to[3];
			space += textorg;
			to[0] = space >> 24;
			to[1] = space >> 16;
			to[2] = space >> 8;
			to[3] = space;

			DEBUG("space=%x\n",space);  
			break;

		case R_DIR32S:
			DEBUG(" R_DIR32S (%d)",rp->r_type); 
			to = (unsigned char *) (buf + sp->s_scnptr + rp->r_vaddr - sp->s_vaddr);
			/* data is little endian */
			space = (to[3] << 24) + (to[2] << 16) +
				    (to[1] << 8) + to[0];
			space += textorg;
			to[3] = space >> 24;
			to[2] = space >> 16;
			to[1] = space >> 8;
			to[0] = space;

			DEBUG("space=%x\n",space);  
		}
		realrp += RELSZ;
		memcpy(rp, realrp, RELSZ);
	}
	sp->s_vaddr += textorg;
	sp->s_paddr += textorg;
	sp->s_relptr = sp->s_lnnoptr = 0;
	sp->s_nreloc = sp->s_nlnno = 0;
}

