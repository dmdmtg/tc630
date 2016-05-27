#	Copyright (c) 1988 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

# @(#)makefile	1.1.1.2	(5/18/88)

########################################################################
#
#	dmdps
#
########################################################################

### Definitions ###

include ../stand.defs

INSTALLHOST=
INSTALL5620=	$(TCMAN)/1.xdmdpr $(TCMAN)/1.xdmdps $(TCMAN)/1.xdmdfmt \
	$(TCMAN)/1.xdmdscale $(TCMAN)/1.xdmdfconv $(TCMAN)/1.xdmdrot90
INSTALL630=	$(INSTALL5620)
SUBDIRSHOST=	src
SUBDIRS5620=	$(SUBDIRSHOST) serial
SUBDIRS630=	$(SUBDIRSHOST) dmd630

TOOLNAME=	dmdps
TC630=		../..

SOURCE=		README docs/dmdpr.1 docs/dmdps.1 docs/dmdfmt.1 docs/dmdscale.1 docs/dmdfconv.1 docs/dmdrot90.1 makefile

### Targets ###

include ../stand.targ

$(TCMAN)/1.xdmdpr:	docs/dmdpr.1
	nroff -man -T37 $(MANFL) docs/dmdpr.1 > $@
	chmod $(MODEBIN) $@

$(TCMAN)/1.xdmdps:	docs/dmdps.1
	tbl docs/dmdps.1 | nroff -man -T37 $(MANFL) | col > $@
	chmod $(MODEBIN) $@

$(TCMAN)/1.xdmdfmt:	docs/dmdfmt.1
	nroff -man -T37 $(MANFL) docs/dmdfmt.1 > $@
	chmod $(MODEBIN) $@

$(TCMAN)/1.xdmdscale:	docs/dmdscale.1
	nroff -man -T37 $(MANFL) docs/dmdscale.1 > $@
	chmod $(MODEBIN) $@

$(TCMAN)/1.xdmdfconv:	docs/dmdfconv.1
	nroff -man -T37 $(MANFL) docs/dmdfconv.1 > $@
	chmod $(MODEBIN) $@

$(TCMAN)/1.xdmdrot90:	docs/dmdrot90.1
	nroff -man -T37 $(MANFL) docs/dmdrot90.1 > $@
	chmod $(MODEBIN) $@
