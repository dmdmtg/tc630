#	Copyright (c) 1987 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

# @(#)makefile	1.1.1.12	(6/20/90)
#

include ../stand.defs

TC630=../..
TOOLNAME=hp2621
OBJSHOST=
OBJS5620= $(TCTERM)/backup.o $(TCTERM)/clear.o $(TCTERM)/curse.o \
	$(TCTERM)/defs.o $(TCTERM)/getnxtchar.o $(TCTERM)/handlekbd.o \
	$(TCTERM)/hp2621.o $(TCTERM)/newhp.o $(TCTERM)/newline.o \
	$(TCTERM)/number.o $(TCTERM)/pt.o $(TCTERM)/readmenu.o \
	$(TCTERM)/realmain.o $(TCTERM)/scroll.o $(TCTERM)/waitchar.o
OBJS630= $(OBJS5620) $(TCTERM)/setjwin.o

PRODUCTSHOST=
PRODUCTS5620=$(TCTERM)/hp2621.m 1.xhp2621 xhp2621
PRODUCTS630=$(PRODUCTS5620)
INSTALLHOST=
INSTALL5620=$(TCBIN)/xhp2621 $(TCLIB)/$(TCTERM)/xhp2621.m \
	$(TCLIB)/dmdtools/xhp2621 $(TCMAN)/1.xhp2621
INSTALL630=$(INSTALL5620)
SOURCE=README backup.c clear.c curse.c defs.c getnxtchar.c handlekbd.c \
	hp2621.c hp2621.h hp2621.sh newhp.c newline.c number.c pt.c \
	readmenu.c realmain.c scroll.c waitchar.c xtipple.c \
	xhp2621.1 ver.sh


include ../stand.targ

$(TCBIN)/xhp2621:	../startup.sh
	cp ../startup.sh $(TCBIN)/xhp2621
	chmod $(MODEBIN) $(TCBIN)/xhp2621

$(TCLIB)/dmdtools/xhp2621:	xhp2621
	cp xhp2621 $(TCLIB)/dmdtools/xhp2621
	chmod $(MODEBIN) $(TCLIB)/dmdtools/xhp2621

$(TCLIB)/dmd/xhp2621.m:	dmd/hp2621.m
	cp dmd/hp2621.m $(TCLIB)/dmd/xhp2621.m
	chmod $(MODELIB) $(TCLIB)/dmd/xhp2621.m

$(TCLIB)/630/xhp2621.m:	630/hp2621.m
	cp 630/hp2621.m $(TCLIB)/630/xhp2621.m
	$(MSTRIP) -m $(TCLIB)/630/xhp2621.m
	chmod $(MODELIB) $(TCLIB)/630/xhp2621.m

$(TCMAN)/1.xhp2621:	1.xhp2621
	cp 1.xhp2621 $(TCMAN)/1.xhp2621
	chmod $(MODELIB) $(TCMAN)/1.xhp2621

xhp2621:	hp2621.sh ver.sh
	sed "s+DeFdMd+$(DEFDMD)+g" ver.sh hp2621.sh > xhp2621

dmd/hp2621.m:	$(OBJS5620)
		$(MCC) $(MCFLAGS) $(OBJS5620) -o dmd/hp2621.m

630/hp2621.m:	$(OBJS630)
		$(MCC) $(MCFLAGS) $(OBJS630) -o 630/hp2621.m

1.xhp2621:	xhp2621.1
	nroff -man -T37 $(MANFL) xhp2621.1 > 1.xhp2621

$(TCTERM)/backup.o:	backup.c
$(TCTERM)/backup.o:	hp2621.h
	$(MCC) $(MCFLAGS) -c backup.c
	mv backup.o $(TCTERM)/backup.o

$(TCTERM)/clear.o:	clear.c
$(TCTERM)/clear.o:	hp2621.h
	$(MCC) $(MCFLAGS) -c clear.c
	mv clear.o $(TCTERM)/clear.o

$(TCTERM)/curse.o:	curse.c
$(TCTERM)/curse.o:	hp2621.h
	$(MCC) $(MCFLAGS) -c curse.c
	mv curse.o $(TCTERM)/curse.o

$(TCTERM)/defs.o:	defs.c
$(TCTERM)/defs.o:	hp2621.h
	$(MCC) $(MCFLAGS) -c defs.c
	mv defs.o $(TCTERM)/defs.o

$(TCTERM)/getnxtchar.o:	getnxtchar.c
$(TCTERM)/getnxtchar.o:	hp2621.h
	$(MCC) $(MCFLAGS) -c getnxtchar.c
	mv getnxtchar.o $(TCTERM)/getnxtchar.o

$(TCTERM)/handlekbd.o:	handlekbd.c
$(TCTERM)/handlekbd.o:	hp2621.h
	$(MCC) $(MCFLAGS) -c handlekbd.c
	mv handlekbd.o $(TCTERM)/handlekbd.o

$(TCTERM)/hp2621.o:	hp2621.c
$(TCTERM)/hp2621.o:	hp2621.h
	$(MCC) $(MCFLAGS) -c hp2621.c
	mv hp2621.o $(TCTERM)/hp2621.o

$(TCTERM)/newhp.o:	newhp.c
$(TCTERM)/newhp.o:	hp2621.h
	$(MCC) $(MCFLAGS) -c newhp.c
	mv newhp.o $(TCTERM)/newhp.o

$(TCTERM)/newline.o:	newline.c
$(TCTERM)/newline.o:	hp2621.h
	$(MCC) $(MCFLAGS) -c newline.c
	mv newline.o $(TCTERM)/newline.o

$(TCTERM)/number.o:	number.c
$(TCTERM)/number.o:	hp2621.h
	$(MCC) $(MCFLAGS) -c number.c
	mv number.o $(TCTERM)/number.o

$(TCTERM)/pt.o:	pt.c
$(TCTERM)/pt.o:	hp2621.h
	$(MCC) $(MCFLAGS) -c pt.c
	mv pt.o $(TCTERM)/pt.o

$(TCTERM)/readmenu.o:	readmenu.c
$(TCTERM)/readmenu.o:	hp2621.h
	$(MCC) $(MCFLAGS) -c readmenu.c
	mv readmenu.o $(TCTERM)/readmenu.o

$(TCTERM)/realmain.o:	realmain.c
$(TCTERM)/realmain.o:	hp2621.h
	$(MCC) $(MCFLAGS) -c realmain.c
	mv realmain.o $(TCTERM)/realmain.o

$(TCTERM)/scroll.o:	scroll.c
$(TCTERM)/scroll.o:	hp2621.h
	$(MCC) $(MCFLAGS) -c scroll.c
	mv scroll.o $(TCTERM)/scroll.o

$(TCTERM)/waitchar.o:	waitchar.c
$(TCTERM)/waitchar.o:	hp2621.h
	$(MCC) $(MCFLAGS) -c waitchar.c
	mv waitchar.o $(TCTERM)/waitchar.o

$(TCTERM)/setjwin.o:	setjwin.c
$(TCTERM)/setjwin.o:	hp2621.h
	$(MCC) $(MCFLAGS) -c setjwin.c
	mv setjwin.o $(TCTERM)/setjwin.o

$(TCTERM)/xtipple.o:	xtipple.c
$(TCTERM)/xtipple.o:	hp2621.h
	$(MCC) $(MCFLAGS) -c xtipple.c
	mv xtipple.o $(TCTERM)/xtipple.o

