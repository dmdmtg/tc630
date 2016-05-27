#	Copyright (c) 1987 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

# @(#)makefile	1.1.1.11	(11/16/87)

include ../stand.defs

TC630=../..
TOOLNAME=ucache
OBJSHOST=
OBJS5620=
OBJS630=ucache.o
PRODUCTSHOST=
PRODUCTS5620=
PRODUCTS630=ucache.m 1.xucache xucache
INSTALLHOST=
INSTALL5620=
INSTALL630=$(TCBIN)/xucache $(TCLIB)/630/xucache.m \
	$(TCLIB)/dmdtools/xucache $(TCMAN)/1.xucache
SOURCE=README makefile ucache.c ucache.sh xucache.1 ver.sh


include ../stand.targ

$(TCBIN)/xucache:	../startup.sh
	cp ../startup.sh $(TCBIN)/xucache
	chmod $(MODEBIN) $(TCBIN)/xucache

$(TCLIB)/dmdtools/xucache:	xucache
	cp xucache $(TCLIB)/dmdtools/xucache
	chmod $(MODEBIN) $(TCLIB)/dmdtools/xucache

$(TCLIB)/630/xucache.m:	ucache.m
	cp ucache.m $(TCLIB)/630/xucache.m
	$(MSTRIP) -m $(TCLIB)/630/xucache.m
	chmod $(MODELIB) $(TCLIB)/630/xucache.m

$(TCMAN)/1.xucache:	1.xucache
	cp 1.xucache $(TCMAN)/1.xucache
	chmod $(MODELIB) $(TCMAN)/1.xucache

xucache: ucache.sh ver.sh
	sed "s+DeFdMd+$(DEFDMD)+g" ver.sh ucache.sh > xucache
	chmod +x xucache

ucache.m:	$(OBJS630)
	$(MCC) $(MCFLAGS) $(OBJS630) -o ucache.m

1.xucache: xucache.1
	nroff -man -T37 $(MANFL) xucache.1 > 1.xucache

ucache.o:       ucache.c
	$(MCC) $(MCFLAGS) -c ucache.c

