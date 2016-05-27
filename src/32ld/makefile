#       Copyright (c) 1987 AT&T   
#       All Rights Reserved       

#       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   
#       The copyright notice above does not evidence any     
#       actual or intended publication of such source code.  

# @(#)makefile	1.1.1.6	(11/16/87)

include ../stand.defs

COFFINCS=../inc/coff/aouthdr.h ../inc/coff/filehdr.h \
	    ../inc/coff/scnhdr.h ../inc/coff/reloc.h
CFLAGS=-I$(DMDSYS)/include -I../inc/coff -I../inc $(EXTRACFLAGS)
TC630=../..
TOOLNAME=32ld
SOURCE=README 32ld.c pcheck.c proto.c proto.h ver.c \
	32reloc.c 32version.c 32ld.1

OBJS32LD=32ld.o pcheck.o proto.o ver.o
OBJSHOST=$(OBJS32LD) 32reloc.o 32version.o
OBJS630=
OBJS5620=$(OBJSHOST)
PRODUCTSHOST=32ld 32reloc 32version
PRODUCTS630=
PRODUCTS5620=$(PRODUCTSHOST) 1.32ld
INSTALLHOST=$(TCBIN)/32ld $(TCBIN)/32reloc $(TCBIN)/32version
INSTALL630=
INSTALL5620=$(INSTALLHOST) $(TCMAN)/1.32ld

include ../stand.targ

32ld:	$(OBJS32LD)
	@HOSTAGENT=""; set -x; \
	for F in $(LIBWINPATHS); do \
		if [ -f "$$F" ]; then HOSTAGENT=$$F; break; fi; done; \
	$(CC) -o 32ld $(OBJS32LD) $$HOSTAGENT $(CFLAGS)

$(OBJS32LD): $(COFFINCS) # make sure build links them in

$(TCBIN)/32ld:	32ld
		cp 32ld $(TCBIN)/32ld
		chmod $(MODEBIN) $(TCBIN)/32ld

32reloc: 32reloc.o
	$(CC) -o 32reloc 32reloc.o $(CFLAGS)

$(TCBIN)/32reloc:	32reloc
		cp 32reloc $(TCBIN)/32reloc
		chmod $(MODEBIN) $(TCBIN)/32reloc

32version: 32version.o
	@HOSTAGENT=""; set -x; \
	for F in $(LIBWINPATHS); do \
		if [ -f "$$F" ]; then HOSTAGENT=$$F; break; fi; done; \
	$(CC) -o 32version 32version.o $$HOSTAGENT $(CFLAGS)

$(TCBIN)/32version:	32version
		cp 32version $(TCBIN)/32version
		chmod $(MODEBIN) $(TCBIN)/32version

$(TCMAN)/1.32ld:	1.32ld
	cp 1.32ld $(TCMAN)/1.32ld
	chmod $(MODELIB) $(TCMAN)/1.32ld

1.32ld: 32ld.1
	nroff -man -T37 $(MANFL) 32ld.1 > 1.32ld

########
#
#	All dependencies and rules not explicitly stated
#	(including header and nested header dependencies)
#
########

32ld.o:		proto.h
32ld.o:		../inc/quiet.h
	@ECFLAGS="-DNO_AGENTVERSION"; set -x; \
	for F in $(LIBWINPATHS); do \
		if [ -f "$$F" ]; then ECFLAGS=""; break; fi; done; \
	$(CC) $(CFLAGS) $$ECFLAGS -c 32ld.c

pcheck.o:	pcheck.c
	$(CC) $(CFLAGS) -c pcheck.c

proto.o:	proto.c
proto.o:	proto.h
	$(CC) $(CFLAGS) -c proto.c

32reloc.o:	32reloc.c
	$(CC) $(CFLAGS) -c 32reloc.c

32version.o:	32version.c proto.h
	@ECFLAGS="-DNO_AGENTVERSION"; set -x; \
	for F in $(LIBWINPATHS); do \
		if [ -f "$$F" ]; then ECFLAGS=""; break; fi; done; \
	$(CC) $(CFLAGS) $$ECFLAGS -c 32version.c

