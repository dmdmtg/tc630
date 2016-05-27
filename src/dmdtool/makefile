# Makefile for the 630 MTG Cartridge Utilities

#       Copyright (c) 1987 AT&T   
#       All Rights Reserved       

#       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   
#       The copyright notice above does not evidence any     
#       actual or intended publication of such source code.  

include ../stand.defs

CFLAGS=-g
TC630=../..
TOOLNAME=dmdtool
SOURCE=README dmdtool.sh 


OBJSHOST=
OBJS630=
OBJS5620=
PRODUCTSHOST=
PRODUCTS630=
PRODUCTS5620=
INSTALLHOST=
INSTALL630=$(TCLIB)/dmdtools/dmdtool
INSTALL5620=$(INSTALL630)

include ../stand.targ

$(TCLIB)/dmdtools/dmdtool:	dmdtool.sh
	sed 's+DeFdMd+$(DEFDMD)+g' dmdtool.sh >$(TCLIB)/dmdtools/dmdtool
	chmod $(MODEBIN) $(TCLIB)/dmdtools/dmdtool

########
#
#	All dependencies and rules not explicitly stated
#	(including header and nested header dependencies)
#
########
