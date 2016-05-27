#       Copyright (c) 1987 AT&T   
#       All Rights Reserved       

#       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   
#       The copyright notice above does not evidence any     
#       actual or intended publication of such source code.  

# @(#)makefile	1.1.1.1	(6/20/90)

include ../../../../stand.defs

OBJS5620=dmd/mlife.o
OBJS630=630/mlife.o

PRODUCTS5620=dmd/life.m
PRODUCTS630=630/life.m

INSTALL5620=$(TCLIB)/xdmddemo/dmd/demolib/xlife 
INSTALL630=$(TCLIB)/xdmddemo/630/xlife 

TC630=../../../../..
TOOLNAME=dmddemo/lg_demo/life/term

SOURCE= makefile mlife.c

include ../../../../stand.targ

$(TCLIB)/xdmddemo/dmd/demolib/xlife:	dmd/life.m
	cp dmd/life.m $@
	chmod $(MODELIB) $@

$(TCLIB)/xdmddemo/630/xlife:	630/life.m
	cp 630/life.m $@
	$(MSTRIP) -m $@
	chmod $(MODELIB) $@

$(TCTERM)/life.m:	$(TCTERM)/mlife.o
	$(MCC) $(MCFLAGS)  $(TCTERM)/mlife.o -o $@

$(TCTERM)/mlife.o:	mlife.c
	$(MCC) $(MCFLAGS) -c mlife.c
	mv mlife.o $@

