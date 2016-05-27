#       Copyright (c) 1987 AT&T   
#       All Rights Reserved       

#       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   
#       The copyright notice above does not evidence any     
#       actual or intended publication of such source code.  

# @(#)makefile	1.1.1.6	(10/20/87)

include ../../../stand.defs

TC630=../../../..
TOOLNAME=dmddemo/lg_demo/calc
OBJS5620=dmd/cell.o dmd/key.o dmd/main.o
OBJS630=630/cell.o 630/key.o 630/main.o
PRODUCTS5620=dmd/calc.m
PRODUCTS630=630/calc.m
INSTALL5620=$(TCLIB)/xdmddemo/dmd/demolib/xcalc 
INSTALL630=$(TCLIB)/xdmddemo/630/xcalc 
SOURCE= cell.c key.c main.c

include ../../../stand.targ

$(TCLIB)/xdmddemo/dmd/demolib/xcalc:	dmd/calc.m
	cp dmd/calc.m $(TCLIB)/xdmddemo/dmd/demolib/xcalc
	chmod $(MODELIB) $(TCLIB)/xdmddemo/dmd/demolib/xcalc

$(TCLIB)/xdmddemo/630/xcalc:	630/calc.m
	cp 630/calc.m $(TCLIB)/xdmddemo/630/xcalc
	$(MSTRIP) -m $(TCLIB)/xdmddemo/630/xcalc
	chmod $(MODELIB) $(TCLIB)/xdmddemo/630/xcalc

dmd/calc.m:	$(OBJS5620)
	$(MCC) $(MCFLAGS) $(OBJS5620) -o dmd/calc.m

630/calc.m:	$(OBJS630)
	$(MCC) $(MCFLAGS) $(OBJS630) -o 630/calc.m

$(TCTERM)/cell.o:	cell.c
	$(MCC) $(MCFLAGS) -c cell.c
	mv cell.o $(TCTERM)/cell.o

$(TCTERM)/key.o:	key.c
	$(MCC) $(MCFLAGS) -c key.c
	mv key.o $(TCTERM)/key.o

$(TCTERM)/main.o:	main.c
	$(MCC) $(MCFLAGS) -c main.c
	mv main.o $(TCTERM)/main.o


