#       Copyright (c) 1987 AT&T   
#       All Rights Reserved       

#       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   
#       The copyright notice above does not evidence any     
#       actual or intended publication of such source code.  

# @(#)makefile	1.1.1.10	(2/26/88)


include ../stand.defs

PRODUCTSHOST=
PRODUCTS5620=	1.xtdmd
PRODUCTS630=	$(PRODUCTS5620)

INSTALLHOST=
INSTALL5620=	$(TCMAN)/1.xtdmd
INSTALL630=	$(INSTALL5620)

SUBDIRSHOST=	host
SUBDIRS5620=	$(SUBDIRSHOST) term
SUBDIRS630=	$(SUBDIRS5620)

TOOLNAME=	tdmd
TC630=		../..
SOURCE=		h/tdmd.h makefile xtdmd.1 README



include ../stand.targ

$(TCMAN)/1.xtdmd:	1.xtdmd
	cp 1.xtdmd $@
	chmod $(MODELIB) $@

1.xtdmd: xtdmd.1
	nroff -man -T37 $(MANFL) xtdmd.1 > $@
