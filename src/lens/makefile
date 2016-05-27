#	Copyright (c) 1987 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

# @(#)makefile	1.1.1.12	(6/20/90)

include ../stand.defs

TC630=../..
TOOLNAME=lens
OBJSHOST=
OBJS5620=$(TCTERM)/lens.o $(TCTERM)/magnify.o $(TCTERM)/nmagnify.o
OBJS630=630/lens.o 630/magnify.o 630/nmagnify.o
PRODUCTSHOST=
PRODUCTS5620=$(TCTERM)/lens.m 1.xlens xlens
PRODUCTS630=$(PRODUCTS5620)
INSTALLHOST=
INSTALL5620=$(TCBIN)/xlens $(TCLIB)/$(TCTERM)/xlens.m \
	$(TCLIB)/dmdtools/xlens $(TCMAN)/1.xlens
INSTALL630=$(INSTALL5620)
SOURCE=README lens.c lens.sh magnify.c nmagnify.c xlens.1 ver.sh


include ../stand.targ

$(TCBIN)/xlens:	../startup.sh
	cp ../startup.sh $(TCBIN)/xlens
	chmod $(MODEBIN) $(TCBIN)/xlens

$(TCLIB)/dmdtools/xlens:	xlens
	cp xlens $(TCLIB)/dmdtools/xlens
	chmod $(MODEBIN) $(TCLIB)/dmdtools/xlens

$(TCLIB)/dmd/xlens.m:	dmd/lens.m
	cp dmd/lens.m $(TCLIB)/dmd/xlens.m
	chmod $(MODELIB) $(TCLIB)/dmd/xlens.m

$(TCLIB)/630/xlens.m:	630/lens.m
	cp 630/lens.m $(TCLIB)/630/xlens.m
	$(MSTRIP) -m $(TCLIB)/630/xlens.m
	chmod $(MODELIB) $(TCLIB)/630/xlens.m

$(TCMAN)/1.xlens:	1.xlens
	cp 1.xlens $(TCMAN)/1.xlens
	chmod $(MODELIB) $(TCMAN)/1.xlens

xlens: lens.sh ver.sh
	sed "s+DeFdMd+$(DEFDMD)+g" ver.sh lens.sh > xlens

dmd/lens.m:	$(OBJS5620)
	$(MCC) $(MCFLAGS) $(OBJS5620) -o dmd/lens.m

630/lens.m:	$(OBJS630)
	$(MCC) $(MCFLAGS) $(OBJS630) -o 630/lens.m

1.xlens: xlens.1
	nroff -man -T37 $(MANFL) xlens.1 > 1.xlens

$(TCTERM)/lens.o:	lens.c
	$(MCC) $(MCFLAGS) -c lens.c
	mv lens.o $(TCTERM)/lens.o

$(TCTERM)/magnify.o:	magnify.c
	$(MCC) $(MCFLAGS) -c magnify.c
	mv magnify.o $(TCTERM)/magnify.o

$(TCTERM)/nmagnify.o:	nmagnify.c
	$(MCC) $(MCFLAGS) -c nmagnify.c
	mv nmagnify.o $(TCTERM)/nmagnify.o

