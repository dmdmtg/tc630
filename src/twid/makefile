#	Copyright (c) 1987 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

# @(#)makefile	1.1.1.11	(6/20/90)

include ../stand.defs

TC630=../..
TOOLNAME=twid
OBJSHOST=
OBJS5620=$(TCTERM)/twid.o $(TCTERM)/cursor.o $(TCTERM)/data.o \
	$(TCTERM)/discture.o $(TCTERM)/rotate.o $(TCTERM)/bitmap.o
OBJS630=$(OBJS5620)
SOURCE=twid.c cursor.c data.c discture.c rotate.c bitmap.c twid.h \
	twid.sh README xtwid.1 ver.sh

PRODUCTSHOST=
PRODUCTS5620=$(TCTERM)/twid.m 1.xtwid xtwid
PRODUCTS630=$(PRODUCTS5620)
INSTALLHOST=
INSTALL5620=$(TCBIN)/xtwid $(TCLIB)/$(TCTERM)/xtwid.m \
	$(TCLIB)/dmdtools/xtwid $(TCMAN)/1.xtwid
INSTALL630=$(INSTALL5620)


include ../stand.targ

$(TCBIN)/xtwid:	../startup.sh
	cp ../startup.sh $(TCBIN)/xtwid
	chmod $(MODEBIN) $(TCBIN)/xtwid

$(TCLIB)/dmdtools/xtwid:	xtwid
	cp xtwid $(TCLIB)/dmdtools/xtwid
	chmod $(MODEBIN) $(TCLIB)/dmdtools/xtwid

$(TCLIB)/dmd/xtwid.m:	dmd/twid.m
	cp dmd/twid.m $(TCLIB)/dmd/xtwid.m
	chmod $(MODELIB) $(TCLIB)/dmd/xtwid.m

$(TCLIB)/630/xtwid.m:	630/twid.m
	cp 630/twid.m $(TCLIB)/630/xtwid.m
	$(MSTRIP) -m $(TCLIB)/630/xtwid.m
	chmod $(MODELIB) $(TCLIB)/630/xtwid.m

$(TCMAN)/1.xtwid:	1.xtwid
	cp 1.xtwid $(TCMAN)/1.xtwid
	chmod $(MODELIB) $(TCMAN)/1.xtwid

xtwid: ver.sh twid.sh
	sed "s+DeFdMd+$(DEFDMD)+g" ver.sh twid.sh > xtwid

630/twid.m:	$(OBJS630)
	$(MCC) $(MCFLAGS) $(OBJS630) -o 630/twid.m

dmd/twid.m:	$(OBJS5620)
	$(MCC) $(MCFLAGS) $(OBJS5620) -o dmd/twid.m

1.xtwid:	xtwid.1
	nroff -man -T37 $(MANFL) xtwid.1 > 1.xtwid

$(TCTERM)/bitmap.o:	bitmap.c
	$(MCC) $(MCFLAGS) -c bitmap.c
	mv bitmap.o $(TCTERM)/bitmap.o

$(TCTERM)/cursor.o:	twid.h
$(TCTERM)/cursor.o:	cursor.c
	$(MCC) $(MCFLAGS) -c cursor.c
	mv cursor.o $(TCTERM)/cursor.o

$(TCTERM)/data.o: twid.h
$(TCTERM)/data.o:	data.c
	$(MCC) $(MCFLAGS) -c data.c
	mv data.o $(TCTERM)/data.o

$(TCTERM)/discture.o:	discture.c
	$(MCC) $(MCFLAGS) -c discture.c
	mv discture.o $(TCTERM)/discture.o

$(TCTERM)/rotate.o:	rotate.c
	$(MCC) $(MCFLAGS) -c rotate.c
	mv rotate.o $(TCTERM)/rotate.o

$(TCTERM)/twid.o:	twid.h
$(TCTERM)/twid.o:	twid.c
	$(MCC) $(MCFLAGS) -c twid.c
	mv twid.o $(TCTERM)/twid.o

