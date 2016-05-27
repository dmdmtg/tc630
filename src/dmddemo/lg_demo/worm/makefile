#       Copyright (c) 1987 AT&T   
#       All Rights Reserved       

#       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   
#       The copyright notice above does not evidence any     
#       actual or intended publication of such source code.  

# @(#)makefile	1.1.1.6	(10/20/87)

include ../../../stand.defs

TC630=../../../..
TOOLNAME=dmddemo/lg_demo/worm
OBJS5620=dmd/but.o dmd/pit.o dmd/worm.o
OBJS630=630/but.o 630/pit.o 630/worm.o
PRODUCTS5620=dmd/worm.m
PRODUCTS630=630/worm.m
INSTALL5620=$(TCLIB)/xdmddemo/dmd/demolib/xworm 
INSTALL630=$(TCLIB)/xdmddemo/630/xworm 
SOURCE= but.c pit.c worm.c but.h


include ../../../stand.targ

$(TCLIB)/xdmddemo/dmd/demolib/xworm:	dmd/worm.m
	cp dmd/worm.m $(TCLIB)/xdmddemo/dmd/demolib/xworm
	chmod $(MODELIB) $(TCLIB)/xdmddemo/dmd/demolib/xworm

$(TCLIB)/xdmddemo/630/xworm:	630/worm.m
	cp 630/worm.m $(TCLIB)/xdmddemo/630/xworm
	$(MSTRIP) -m $(TCLIB)/xdmddemo/630/xworm
	chmod $(MODELIB) $(TCLIB)/xdmddemo/630/xworm

dmd/worm.m:	$(OBJS5620)
	$(MCC) $(MCFLAGS) $(OBJS5620) -o dmd/worm.m

630/worm.m:	$(OBJS630)
	$(MCC) $(MCFLAGS) $(OBJS630) -o 630/worm.m

$(TCTERM)/but.o:	but.h
$(TCTERM)/but.o:	but.c
	$(MCC) $(MCFLAGS) -c but.c
	mv but.o $(TCTERM)/but.o

$(TCTERM)/pit.o:	pit.c
$(TCTERM)/pit.o:	but.h
	$(MCC) $(MCFLAGS) -c pit.c
	mv pit.o $(TCTERM)/pit.o

$(TCTERM)/worm.o:	worm.c
$(TCTERM)/worm.o:	but.h
	$(MCC) $(MCFLAGS) -c worm.c
	mv worm.o $(TCTERM)/worm.o


