#       Copyright (c) 1987 AT&T   
#       All Rights Reserved       

#       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   
#       The copyright notice above does not evidence any     
#       actual or intended publication of such source code.  

# @(#)makefile	1.1.1.6	(11/16/87)

include ../stand.defs

COFFINCS=../inc/coff/filehdr.h ../inc/coff/scnhdr.h
CFLAGS=-I../inc/coff $(EXTRACFLAGS)
TC630=../..
TOOLNAME=zap630
SOURCE=README makefile zap630.c xzap630.1 ver.c
OBJSHOST=zap630.o ver.o
OBJS630=$(OBJSHOST)
OBJS5620=
PRODUCTSHOST=zap630
PRODUCTS630=$(PRODUCTSHOST) 1.xzap630
PRODUCTS5620=
INSTALLHOST=$(TCBIN)/xzap630
INSTALL630=$(INSTALLHOST) $(TCMAN)/1.xzap630
INSTALL5620=

include ../stand.targ

zap630:	$(OBJS630)
	$(CC) -o zap630 $(OBJS630) $(CFLAGS)

zap630.o: $(COFFINCS) # make sure build links them in

$(TCBIN)/xzap630:	zap630
	cp zap630 $(TCBIN)/xzap630
	chmod $(MODEBIN) $(TCBIN)/xzap630

$(TCMAN)/1.xzap630:	1.xzap630
	cp 1.xzap630 $(TCMAN)/1.xzap630
	chmod $(MODELIB) $(TCMAN)/1.xzap630

1.xzap630: xzap630.1
	nroff -man -T37 $(MANFL) xzap630.1 > 1.xzap630

########
#
#	All dependencies and rules not explicitly stated
#	(including header and nested header dependencies)
#
########

zap630.o:	zap630.c
	$(CC) $(CFLAGS) -c zap630.c

