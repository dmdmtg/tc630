#	Copyright (c) 1987 AT&T
#	All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

# @(#)Makefile	1.1.1.13 90/06/21 13:31:44

########################################################################
#
#	proof
#
########################################################################

### Definitions ###

include ../../stand.defs

PRODUCTS5620=	$(TCTERM)/proof.m
PRODUCTS630=	$(TCTERM)/proof.m

INSTALL5620=	$(TCLIB)/dmd/xproof.m
INSTALL630=	$(TCLIB)/630/xproof.m

TOOLNAME=	proof/term
TC630=		../../..
SOURCE= 	Makefile infont.c slave.c
OBJECTS=	$(TCTERM)/slave.o $(TCTERM)/infont.o
OBJS5620=	$(OBJECTS)
OBJS630=	$(OBJECTS)

LINT=		lint
# until there is a MISC target in stand.targ, being able to do
# both the 5620 and the 630 will be difficult.
LNFLAGS=	-n -I$(DMD)/include -I.. -I$(Tc)/src/misc -DDMD630
LNFILES=	infont.ln slave.ln
LINTLIB=	$(TCSRC)/misc/llib-l630.ln

### Targets ###

include ../../stand.targ

$(TCLIB)/$(TCTERM)/xproof.m:	$(TCLIB)/$(TCTERM) $(TCTERM) $(TCTERM)/proof.m
	/bin/cp $(TCTERM)/proof.m $@
#	@if [ $(TCTERM) != dmd ]; then	\
		echo '\t' $(MSTRIP) $@;	\
		$(MSTRIP) -m $@;	\
	fi
	chmod $(MODELIB) $@

$(TCTERM)/proof.m:	$(OBJECTS) $(TCTERM)
	$(MCC) $(MCFLAGS) -o $@ $(OBJECTS)

$(TCTERM):
	-mkdir $@

$(TCLIB)/$(TCTERM):
	-mkdir $(TCLIB) $@

$(TCTERM)/infont.o:	../include/comm.h
$(TCTERM)/infont.o:	infont.c
	$(MCC) $(MCFLAGS) -c infont.c
	mv $(@F) $(TCTERM)

$(TCTERM)/slave.o:	../include/comm.h
$(TCTERM)/slave.o:	slave.c
	$(MCC) $(MCFLAGS) -c slave.c
	mv $(@F) $(TCTERM)


lint:	$(LNFILES) Makefile ../include/comm.h $(LINTLIB)
	$(LINT) $(LNFLAGS) $(LNFILES) $(LINTLIB)

.SUFFIXES:	.ln
.c.ln:
	$(LINT) $(LNFLAGS) -c $< $(LINTLIB)
$(LNFILES):	../include/comm.h
