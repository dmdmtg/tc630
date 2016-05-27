#       Copyright (c) 1987 AT&T
#         All Rights Reserved

#       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#       The copyright notice above does not evidence any
#       actual or intended publication of such source code.

# @(#)makefile	1.1.1.12	(6/21/90)

include ../stand.defs

PRODUCTSHOST=
PRODUCTS5620=	1.xproof
PRODUCTS630=	$(PRODUCTS5620)

INSTALLHOST=
INSTALL5620=	$(TCMAN)/1.xproof
INSTALL630=	$(INSTALL5620)

SUBDIRSHOST=	host
SUBDIRS5620=	$(SUBDIRSHOST) term
SUBDIRS630=	$(SUBDIRS5620)

TOOLNAME=	proof
TC630=		../..
SOURCE=		README include/comm.h makefile xproof.1 

include ../stand.targ

$(TCMAN)/1.xproof:	1.xproof $(TCMAN)
	cp 1.xproof $(TCMAN)/1.xproof
	chmod $(MODELIB) $(TCMAN)/1.xproof

1.xproof: xproof.1
	nroff -man -T37 $(MANFL) xproof.1 > 1.xproof

$(TCMAN):
	-mkdir $@
