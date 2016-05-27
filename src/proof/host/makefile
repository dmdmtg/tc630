#	Copyright (c) 1987 AT&T
#	All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

# @(#)Makefile	1.1.1.13 90/06/21 13:31:38


########################################################################
#
#	proof
#
########################################################################

### Definitions ###

include ../../stand.defs

PRODUCTSHOST=	proof
PRODUCTS5620=	$(PRODUCTSHOST)
PRODUCTS630=	$(PRODUCTSHOST)

INSTALLHOST=	$(TCLIB)/dmdtools/xproof
INSTALL5620=	$(TCBIN)/xproof $(INSTALLHOST)
INSTALL630=	$(INSTALL5620)

TOOLNAME=	proof/host
TC630=		../../..
SOURCE=		Makefile dir.h font.c host.h proof.c ver.c
OBJSHOST=	proof.o font.o ver.o
OBJS5620=	$(OBJSHOST)
OBJS630=	$(OBJSHOST)

DEBUG=		-DDEBUG
CFLAGS=		-I$(DMDSYS)/include -I../../inc $(DEBUG) \
			-DProofmod=\"xproof.m\" $(EXTRACFLAGS)

LINT=		lint
LNFLAGS=	-I$(DMDSYS)/include -I.. -DProofmod=\"\"
LNFILES=	font.ln proof.ln

### Targets ###


include ../../stand.targ

$(TCBIN)/xproof:	../../startup.sh
	cp ../../startup.sh $@
	chmod $(MODEBIN) $@

$(TCLIB)/dmdtools/xproof:	proof
	cp proof $@
	chmod $(MODEBIN) $@

proof:	$(OBJSHOST) 
	$(CC) -o $@ $(OBJSHOST) $(CFLAGS)

proof.o:	../include/comm.h
proof.o:	host.h
proof.o:	proof.c
font.o:	../include/comm.h
font.o:	host.h
font.o:	font.c
	@if [ -n "$(DEFDMD)" ];then D=-DDEFDMD=\"$(DEFDMD)\";else D=;fi;set -x;\
		$(CC) $(CFLAGS) $$D -c font.c


lint:	$(LNFILES) Makefile ../include/comm.h
	$(LINT) $(LNFLAGS) $(LNFILES)

.SUFFIXES:	.ln
.c.ln:
	$(LINT) $(LNFLAGS) -c $<
$(LNFILES):	../include/comm.h
