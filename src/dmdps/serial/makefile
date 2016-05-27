#	Copyright (c) 1988 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

# @(#)makefile	1.1.1.2	(5/6/88)

########################################################################
#
#	dmdps
#
########################################################################

### Definitions ###

include ../../stand.defs

PRODUCTS5620=	dmdps.m
PRODUCTS630=

INSTALL5620=	$(TCBIN)/xdmdps		\
		$(TCLIB)/dmdtools/xdmdps	\
		$(TCLIB)/dmd/xdmdps.m
INSTALL630=

TOOLNAME=	dmdps/serial
TC630=		../../..
PS_SRC=		../src

SOURCE=		$(PS_SRC)/dmdps.c	\
		$(PS_SRC)/doUNIXwork.c	\
		$(PS_SRC)/dumper.c	\
		$(PS_SRC)/newread.c	\
		$(PS_SRC)/newsend.c	\
		$(PS_SRC)/parse_args.c	\
		$(PS_SRC)/printer.c	\
		$(PS_SRC)/print630.c	\
		$(PS_SRC)/printbmap.c	\
		$(PS_SRC)/put.c		\
		$(PS_SRC)/readbitmap.c	\
		$(PS_SRC)/readpfd.c	\
		$(PS_SRC)/sendbitmap.c	\
		$(PS_SRC)/showbitmap.c	\
		$(PS_SRC)/sprintc.c	\
		$(PS_SRC)/tiles.c	\
		$(PS_SRC)/bit_h.h	\
		$(PS_SRC)/defs.h	\
		$(PS_SRC)/dmdps.h	\
		$(PS_SRC)/icons.h	\
		$(PS_SRC)/intrpcbs.h	\
		$(PS_SRC)/menus.h	\
		$(PS_SRC)/pfd.h		\
		$(PS_SRC)/printer.h	\
		$(PS_SRC)/think.h	\
		$(PS_SRC)/icon/blank	\
		$(PS_SRC)/icon/copier	\
		$(PS_SRC)/icon/cross	\
		$(PS_SRC)/icon/deadmouse	\
		$(PS_SRC)/icon/f1	\
		$(PS_SRC)/icon/f2	\
		$(PS_SRC)/icon/l1	\
		$(PS_SRC)/icon/menu2	\
		$(PS_SRC)/icon/menu3	\
		$(PS_SRC)/icon/mouse1	\
		$(PS_SRC)/icon/mouse2	\
		$(PS_SRC)/icon/mouse3	\
		$(PS_SRC)/icon/stiptext	\
		$(PS_SRC)/icon/sweepcursor	\
		$(PS_SRC)/icon/target	\
		$(PS_SRC)/icon/thumbsdown	\
		$(PS_SRC)/icon/thumbsup	\
		$(PS_SRC)/icon/tile_a	\
		$(PS_SRC)/icon/tile_b	\
		$(PS_SRC)/icon/tile_c	\
		$(PS_SRC)/icon/tile_d	\
		$(PS_SRC)/icon/tile_e	\
		$(PS_SRC)/icon/tile_f	\
		$(PS_SRC)/dmdps.sh	\
		makefile		\
		makef.serial		\
		ver.sh

OBJS5620=	dmdps.o doUNIXwork.o dumper.o newread.o newsend.o \
		parse_args.o printbmap.o printer.o readbitmap.o \
		readpfd.o sendbitmap.o showbitmap.o sprintc.o tiles.o
OBJS630=

### Targets ###


include ../../stand.targ

$(TCBIN)/xdmdps:	../../startup.sh
	cp ../../startup.sh $@
	chmod $(MODEBIN) $@

$(TCLIB)/dmdtools/xdmdps:	ver.sh $(PS_SRC)/dmdps.sh
	cat ver.sh $(PS_SRC)/dmdps.sh > $@
	chmod $(MODEBIN) $@


$(TCLIB)/$(TCTERM)/xdmdps.m:	dmdps.m
	cp dmdps.m $@
#	@if [ $(TCTERM) != dmd ]; then	\
		echo '\t' $(MSTRIP) $@;	\
		$(MSTRIP) -m $@;	\
	fi
	chmod $(MODELIB) $@

include makef.serial
