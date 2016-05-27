#	Copyright (c) 1987 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

# @(#)makefile	1.1.1.5	(10/9/87)

include ../stand.defs

CFLAGS=-I$(DMDSYS)/include -I../inc $(EXTRACFLAGS)
LDFLAGS=
TC630=../..
TOOLNAME=jx
OBJSHOST=jx.o ver.o
OBJS5620=$(OBJSHOST)
OBJS630=$(OBJSHOST)
PRODUCTSHOST=jx sysint
PRODUCTS630=$(PRODUCTSHOST) 1.xjx
PRODUCTS5620=$(PRODUCTS630)
#start.sh;  binary & manual
INSTALL5620=
#start.sh; binary & manual
INSTALLHOST=$(TCLIB)/dmdtools/xjx $(TCLIB)/dmdtools/xsysint
INSTALL630=$(INSTALLHOST) $(TCBIN)/xjx $(TCMAN)/1.xjx
INSTALL5620=$(INSTALL630)
SOURCE=jx.c sysint.c comm.h xjx.1 makefile ver.c README


include ../stand.targ

$(TCBIN)/xjx:	../startup.sh
	cp ../startup.sh $(TCBIN)/xjx
	chmod $(MODEBIN) $(TCBIN)/xjx

$(TCLIB)/dmdtools/xjx:	jx
	cp jx $(TCLIB)/dmdtools/xjx
	chmod $(MODEBIN) $(TCLIB)/dmdtools/xjx

#install sysint binary.
$(TCLIB)/dmdtools/xsysint:	sysint	
	cp sysint $(TCLIB)/dmdtools/xsysint
	chmod $(MODEBIN) $(TCLIB)/dmdtools/xsysint

#install manual page.
$(TCMAN)/1.xjx:	1.xjx
	cp 1.xjx $(TCMAN)/1.xjx
	chmod $(MODELIB) $(TCMAN)/1.xjx

#
#Build jx & sysint binaries and manual pages.
#
jx:	$(OBJS630)
	@HOSTAGENT=""; set -x; \
	for F in $(LIBWINPATHS); do \
		if [ -f "$$F" ]; then HOSTAGENT=$$F; break; fi; done; \
	$(CC) -o jx $(OBJS630) $$HOSTAGENT $(CFLAGS)

jx.o:	jx.c 
jx.o:	../inc/quiet.h
	@if [ -n "$(DEFDMD)" ];then D=-DDEFDMD=\"$(DEFDMD)\";else D=;fi;set -x;\
		$(CC) $(CFLAGS) $$D -c jx.c

sysint:	sysint.c
sysint:	comm.h
	$(CC) -o sysint sysint.c $(CFLAGS)

1.xjx: xjx.1
	tbl xjx.1 | nroff -man -T37 $(MANFL) > 1.xjx

