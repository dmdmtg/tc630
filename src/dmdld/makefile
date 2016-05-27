#       Copyright (c) 1987 AT&T   
#       All Rights Reserved       

#       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   
#       The copyright notice above does not evidence any     
#       actual or intended publication of such source code.  

# @(#)makefile	1.1.1.6	(11/16/87)

include ../stand.defs

COFFINCS=../inc/coff/filehdr.h ../inc/coff/scnhdr.h ../inc/coff/reloc.h

CFLAGS=-I$(DMDSYS)/include -I../inc/coff -I../inc $(EXTRACFLAGS)
TC630=../..
TOOLNAME=dmdld
SOURCE=README dmd.h dmdld.c io.c load.h pcheck.c proto.c proto.h \
	reloc.c xdmdld.1 ver.c

OBJSHOST=dmdld.o proto.o pcheck.o io.o reloc.o shakecode.o ver.o
OBJS5620=
OBJS630=$(OBJSHOST)
PRODUCTSHOST=dmdld
PRODUCTS630=$(PRODUCTSHOST) 1.xdmdld
PRODUCTS5620=
INSTALLHOST=$(TCBIN)/xdmdld
INSTALL630=$(INSTALLHOST) $(TCMAN)/1.xdmdld
INSTALL5620=
CLEANHOST=$(OBJSHOST) shake.o
CLEAN630=$(CLEANHOST)

# don't clean out shakecode.c; that way it can be used
#  on machines that don't have dmdcc or mc68dis

include ../stand.targ

dmdld:	$(OBJSHOST)
	@HOSTAGENT=""; set -x; \
	for F in $(LIBWINPATHS); do \
		if [ -f "$$F" ]; then HOSTAGENT=$$F; break; fi; done; \
	$(CC) -o dmdld $(OBJS630) $$HOSTAGENT $(CFLAGS)

$(OBJS630): $(COFFINCS) # make sure build links them in

$(TCBIN)/xdmdld:	dmdld
		cp dmdld $(TCBIN)/xdmdld
		chmod $(MODEBIN) $(TCBIN)/xdmdld

$(TCMAN)/1.xdmdld:	1.xdmdld
	cp 1.xdmdld $(TCMAN)/1.xdmdld
	chmod $(MODELIB) $(TCMAN)/1.xdmdld

1.xdmdld: xdmdld.1
	nroff -man -T37 $(MANFL) xdmdld.1 > 1.xdmdld

shakecode.o : shakecode.c

shakecode.c : shake.s makecode.sh
	if [ \( ! -x $(MCC) -o ! -x $(MDIS) \) -a -f shakecode.c ]; then \
		echo "shakecode.c not recreated"; \
	else	$(MCC) -c shake.s; \
		sh -x makecode.sh $(MDIS) shake.o Handshakecode >shakecode.c; \
	fi

########
#
#	All dependencies and rules not explicitly stated
#	(including header and nested header dependencies)
#
########

dmdld.o:	dmdld.c
dmdld.o:	../inc/quiet.h
dmdld.o:	dmd.h
dmdld.o:	proto.h
dmdld.o:	load.h
	$(CC) $(CFLAGS) -c dmdld.c

pcheck.o:	pcheck.c
	$(CC) $(CFLAGS) -c pcheck.c

proto.o:	proto.c
proto.o:	proto.h
	$(CC) $(CFLAGS) -c proto.c

io.o:	io.c
io.o:	proto.h
io.o:	load.h
	$(CC) $(CFLAGS) -c io.c

reloc.o:	reloc.c
	$(CC) $(CFLAGS) -c reloc.c

