#       Copyright (c) 1987 AT&T   
#       All Rights Reserved       

#       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   
#       The copyright notice above does not evidence any     
#       actual or intended publication of such source code.  

# @(#)makefile	1.1.1.14	(6/16/88)

include ../stand.defs

TC630=../..

OBJSHOST=
OBJS5620=
OBJS630=
PRODUCTSHOST=
PRODUCTS5620=1.xdmddemo xdmddemo
PRODUCTS630=$(PRODUCTS5620)
INSTALLHOST=
INSTALL5620=$(TCLIB)/dmdtools/xdmddemo $(TCBIN)/xdmddemo $(TCMAN)/1.xdmddemo
INSTALL630=$(INSTALL5620)
SUBDIRSHOST=lg_demo
SUBDIRS5620=sm_demo $(SUBDIRSHOST)
SUBDIRS630=$(SUBDIRS5620)
TOOLNAME=dmddemo
SOURCE=README xdmddemo.sh xdmddemo.1 ver.sh

include ../stand.targ

$(TCBIN)/xdmddemo:   ../startup.sh
	cp ../startup.sh $(TCBIN)/xdmddemo
	chmod $(MODEBIN) $(TCBIN)/xdmddemo

$(TCMAN)/1.xdmddemo: 1.xdmddemo
	cp 1.xdmddemo $(TCMAN)/1.xdmddemo
	chmod $(MODELIB) $(TCMAN)/1.xdmddemo

$(TCLIB)/dmdtools/xdmddemo:  xdmddemo
	cp xdmddemo $(TCLIB)/dmdtools/xdmddemo
	chmod $(MODEBIN) $(TCLIB)/dmdtools/xdmddemo

xdmddemo: xdmddemo.sh ver.sh
	sed "s+DeFdMd+$(DEFDMD)+g" ver.sh xdmddemo.sh > xdmddemo

1.xdmddemo:  xdmddemo.1
	nroff -man -T37 $(MANFL) xdmddemo.1 > 1.xdmddemo
