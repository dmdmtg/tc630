#	Copyright (c) 1988 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

# @(#)makefile	1.1.1.1	(5/6/88)

########################################################################
#
#	dmdps
#
########################################################################

### Definitions ###

include ../../stand.defs

PRODUCTSHOST=	dmdfmt dmdscale dmdfconv dmdrot90
PRODUCTS5620=	$(PRODUCTSHOST)
PRODUCTS630=	$(PRODUCTS5620)

INSTALLHOST=	$(TCBIN)/xdmdfmt $(TCBIN)/xdmdscale \
			$(TCBIN)/xdmdfconv $(TCBIN)/xdmdrot90 
INSTALL5620=	$(INSTALLHOST) $(TCBIN)/xdmdpr
INSTALL630=	$(INSTALL5620)

TC630=		../../..
TOOLNAME=	dmdps/src

SOURCE=		bit_h.h dmdfconv.c dmdfmt.c dmdio.c \
		dmdpr.sh dmdrot90.c dmdscale.c impress.h udmd.h \
		makef.src makefile

OBJSHOST=	dmdfconv.o dmdfmt.o dmdio.o dmdrot90.o dmdscale.o
OBJS630=	$(OBJSHOST)
OBJS5620=	$(OBJS630)

### Targets ###

include ../../stand.targ

$(TCBIN)/xdmdfconv:	dmdfconv
	cp dmdfconv $@
	chmod $(MODEBIN) $@

$(TCBIN)/xdmdfmt:	dmdfmt
	cp dmdfmt $@
	chmod $(MODEBIN) $@

$(TCBIN)/xdmdpr:	ver.sh dmdpr.sh
	cat ver.sh dmdpr.sh > $@
	chmod $(MODEBIN) $@

$(TCBIN)/xdmdrot90:	dmdrot90
	cp dmdrot90 $@
	chmod $(MODEBIN) $@

$(TCBIN)/xdmdscale:	dmdscale
	cp dmdscale $@
	chmod $(MODEBIN) $@

include makef.src
