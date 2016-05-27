#       Copyright (c) 1987 AT&T   
#       All Rights Reserved       

#       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   
#       The copyright notice above does not evidence any     
#       actual or intended publication of such source code.  

# @(#)makefile	1.1.1.5	(10/20/87)

include ../../../stand.defs

TC630=../../../..
TOOLNAME=dmddemo/lg_demo/smiley
OBJS5620=dmd/explode.o dmd/smiley.o dmd/smileyf.o
OBJS630=630/explode.o 630/smiley.o 630/smileyf.o
PRODUCTS5620=dmd/smiley.j
PRODUCTS630=630/smiley.m
INSTALL5620=$(TCLIB)/xdmddemo/dmd/demolibsa/xsmiley 
INSTALL630=$(TCLIB)/xdmddemo/630/xsmiley 
SOURCE= explode.c smiley.c smileyf.c

include ../../../stand.targ

$(TCLIB)/xdmddemo/dmd/demolibsa/xsmiley:	dmd/smiley.j
	cp dmd/smiley.j $(TCLIB)/xdmddemo/dmd/demolibsa/xsmiley
	chmod $(MODELIB) $(TCLIB)/xdmddemo/dmd/demolibsa/xsmiley

$(TCLIB)/xdmddemo/630/xsmiley:	630/smiley.m
	cp 630/smiley.m $(TCLIB)/xdmddemo/630/xsmiley
	$(MSTRIP) -m $(TCLIB)/xdmddemo/630/xsmiley
	chmod $(MODELIB) $(TCLIB)/xdmddemo/630/xsmiley

dmd/smiley.j:	$(OBJS5620)
	$(MCC) $(MCFLAGS) -J $(OBJS5620) -o dmd/smiley.j

630/smiley.m:	$(OBJS630)
	$(MCC) $(MCFLAGS) $(OBJS630) -o 630/smiley.m

630/explode.o:	explode.c
	$(MCC) $(MCFLAGS) -c explode.c
	mv explode.o 630/explode.o

630/smiley.o:	smiley.c
	$(MCC) $(MCFLAGS) -c smiley.c
	mv smiley.o 630/smiley.o

630/smileyf.o:	smileyf.c
	$(MCC) $(MCFLAGS) -c smileyf.c
	mv smileyf.o 630/smileyf.o

dmd/explode.o:	explode.c
	$(MCC) $(MCFLAGS) -J -c explode.c
	mv explode.o dmd/explode.o

dmd/smiley.o:	smiley.c
	$(MCC) $(MCFLAGS) -J -c smiley.c
	mv smiley.o dmd/smiley.o

dmd/smileyf.o:	smileyf.c
	$(MCC) $(MCFLAGS) -J -c smileyf.c
	mv smileyf.o dmd/smileyf.o

