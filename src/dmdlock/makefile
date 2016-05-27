#	Copyright (c) 1988 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

# @(#)makefile	1.1.1.4	(5/24/88)

########################################################################
#
#	dmdlock
#
########################################################################

### Definitions ###

include ../stand.defs

PRODUCTSHOST=	makelock
PRODUCTS5620=	$(PRODUCTSHOST) xdmdlock $(TCTERM)/dmdlock.m 1.xdmdlock
PRODUCTS630=	$(PRODUCTS5620)

INSTALLHOST=	$(TCLIB)/xdmdlock/makelock

INSTALL5620=	$(INSTALLHOST)			\
		$(TCBIN)/xdmdlock		\
		$(TCLIB)/dmdtools/xdmdlock	\
		$(TCLIB)/$(TCTERM)/xdmdlock.m	\
		$(TCMAN)/1.xdmdlock

INSTALL630=	$(INSTALL5620)

TOOLNAME=	dmdlock
TC630=		../..

SOURCE=		README crypt.c dmdlock.1 dmdlock.c dmdlock.sh io_exp.h \
		machdep.h makelock.c stdpass.c stdpass.h swapper.h vsc.h \
		makefile

OBJSHOST=	makelock.o hcrypt.o hstdpass.o

OBJST5620=	$(TCTERM)/crypt.o	\
		$(TCTERM)/dmdlock.o	\
		$(TCTERM)/stdpass.o

OBJS5620=	$(OBJSHOST) $(OBJST5620)

OBJST630=	$(OBJST5620)		\
		$(TCTERM)/changeswap.o	\
		$(TCTERM)/changeswap.o	\
		$(TCTERM)/createswap.o	\
		$(TCTERM)/destroysw.o	\
		$(TCTERM)/disablesw.o	\
		$(TCTERM)/enableswap.o	\
		$(TCTERM)/getcurssp.o	\
		$(TCTERM)/getssp.o	\
		$(TCTERM)/makeuncurr.o	\
		$(TCTERM)/remaplayer.o	\
		$(TCTERM)/swapspace.o

OBJS630=	$(OBJSHOST) $(OBJST630)

### Targets ###

include ../stand.targ

$(TCMAN)/1.x$(TOOLNAME):	1.x$(TOOLNAME)
	cp 1.x$(TOOLNAME) $@
	chmod $(MODEBIN) $@

1.x$(TOOLNAME):	$(TOOLNAME).1
	nroff -man -T37 $(MANFL) $(TOOLNAME).1 > $@

$(TCBIN)/x$(TOOLNAME):	../startup.sh
	cp ../startup.sh $@
	chmod $(MODEBIN) $@

$(TCLIB)/dmdtools/x$(TOOLNAME):	$(TOOLNAME).sh
	sed "s+DeFdMd+$(DEFDMD)+g" $(TOOLNAME).sh > $@
	chmod $(MODEBIN) $@

$(TCLIB)/x$(TOOLNAME)/makelock: makelock
	mkdir $(TCLIB)/x$(TOOLNAME) 2>/dev/null || true
	cp makelock $@

$(TCLIB)/$(TCTERM)/x$(TOOLNAME).m:	$(TCTERM)/$(TOOLNAME).m
	cp $(TCTERM)/$(TOOLNAME).m $@
	chmod $(MODELIB) $@

dmd/$(TOOLNAME).m:	$(OBJST5620)
	$(MCC) $(MCFLAGS) -o $@ $(OBJST5620)

630/$(TOOLNAME).m:	$(OBJST630)
	$(MCC) $(MCFLAGS) -o $@ $(OBJST630)


makelock: makelock.o hcrypt.o hstdpass.o
	$(CC) -o makelock makelock.o hcrypt.o hstdpass.o $(CFLAGS)

makelock.o: makelock.c stdpass.h
	$(CC) $(CFLAGS) -c makelock.c

hcrypt.o: crypt.c
	$(CC) $(CFLAGS) -c crypt.c
	mv crypt.o hcrypt.o

hstdpass.o: stdpass.c stdpass.h
	$(CC) $(CFLAGS) -c stdpass.c
	mv stdpass.o hstdpass.o

$(TCTERM)/crypt.o:	crypt.c
	$(MCC) $(MCFLAGS) -c crypt.c
	mv $(@F) $(TCTERM)

$(TCTERM)/dmdlock.o:	machdep.h
$(TCTERM)/dmdlock.o:	io_exp.h
$(TCTERM)/dmdlock.o:	vsc.h
$(TCTERM)/dmdlock.o:	swapper.h
$(TCTERM)/dmdlock.o:	stdpass.h
$(TCTERM)/dmdlock.o:	dmdlock.c
	$(MCC) $(MCFLAGS) -c dmdlock.c
	mv $(@F) $(TCTERM)

$(TCTERM)/stdpass.o:	stdpass.c stdpass.h
	$(MCC) $(MCFLAGS) -c stdpass.c
	mv $(@F) $(TCTERM)

$(TCTERM)/changeswap.o	: libssp/changeswap.s
	$(MCC) $(MCFLAGS) -c libssp/changeswap.s
	mv $(@F) $(TCTERM)

$(TCTERM)/createswap.o	: libssp/createswap.s
	$(MCC) $(MCFLAGS) -c libssp/createswap.s
	mv $(@F) $(TCTERM)

$(TCTERM)/destroysw.o	: libssp/destroysw.s
	$(MCC) $(MCFLAGS) -c libssp/destroysw.s
	mv $(@F) $(TCTERM)

$(TCTERM)/disablesw.o	: libssp/disablesw.s
	$(MCC) $(MCFLAGS) -c libssp/disablesw.s
	mv $(@F) $(TCTERM)

$(TCTERM)/enableswap.o	: libssp/enableswap.s
	$(MCC) $(MCFLAGS) -c libssp/enableswap.s
	mv $(@F) $(TCTERM)

$(TCTERM)/getcurssp.o	: libssp/getcurssp.s
	$(MCC) $(MCFLAGS) -c libssp/getcurssp.s
	mv $(@F) $(TCTERM)

$(TCTERM)/getssp.o	: libssp/getssp.s
	$(MCC) $(MCFLAGS) -c libssp/getssp.s
	mv $(@F) $(TCTERM)

$(TCTERM)/makeuncurr.o	: libssp/makeuncurr.s
	$(MCC) $(MCFLAGS) -c libssp/makeuncurr.s
	mv $(@F) $(TCTERM)

$(TCTERM)/remaplayer.o	: libssp/remaplayer.s
	$(MCC) $(MCFLAGS) -c libssp/remaplayer.s
	mv $(@F) $(TCTERM)

$(TCTERM)/swapspace.o	: libssp/swapspace.s
	$(MCC) $(MCFLAGS) -c libssp/swapspace.s
	mv $(@F) $(TCTERM)

