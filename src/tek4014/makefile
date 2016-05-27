#       Copyright (c) 1987 AT&T
#         All Rights Reserved

#       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#       The copyright notice above does not evidence any
#       actual or intended publication of such source code.

# @(#)makefile	1.1.1.14	(6/20/90)

include ../stand.defs

TC630=../..
TOOLNAME=tek4014
OBJSHOST=
OBJS5620=$(TCTERM)/dline.o $(TCTERM)/jek.o $(TCTERM)/flash.o $(TCTERM)/tiny.o \
	$(TCTERM)/bigapl.o $(TCTERM)/tinyapl.o $(TCTERM)/dclip.o \
	$(TCTERM)/jdlineto.o  $(TCTERM)/dlsegment.o
OBJS630=$(OBJS5620)
PRODUCTSHOST=
PRODUCTS5620=$(TCTERM)/tek4014.m 1.xtek4014 xtek4014
PRODUCTS630=$(PRODUCTS5620)
INSTALLHOST=
INSTALL5620=$(TCBIN)/xtek4014 $(TCLIB)/$(TCTERM)/xtek4014.m \
	$(TCLIB)/dmdtools/xtek4014 $(TCMAN)/1.xtek4014
INSTALL630=$(INSTALL5620)
SOURCE=bigapl.c dclip.c dline.c dline.h dlsegment.c flash.c jdlineto.c \
	jek.c tiny.c tinyapl.c README tek4014.sh xtek4014.1 \
	ver.sh


include ../stand.targ

$(TCBIN)/xtek4014:	../startup.sh
	cp ../startup.sh $(TCBIN)/xtek4014
	chmod $(MODEBIN) $(TCBIN)/xtek4014

$(TCLIB)/dmdtools/xtek4014:	xtek4014
	cp xtek4014 $(TCLIB)/dmdtools/xtek4014
	chmod $(MODEBIN) $(TCLIB)/dmdtools/xtek4014

$(TCLIB)/dmd/xtek4014.m:	dmd/tek4014.m
	cp dmd/tek4014.m $(TCLIB)/dmd/xtek4014.m
	chmod $(MODELIB) $(TCLIB)/dmd/xtek4014.m

$(TCLIB)/630/xtek4014.m:	630/tek4014.m
	cp 630/tek4014.m $(TCLIB)/630/xtek4014.m
	$(MSTRIP) -m $(TCLIB)/630/xtek4014.m
	chmod $(MODELIB) $(TCLIB)/630/xtek4014.m

$(TCMAN)/1.xtek4014:	1.xtek4014
	cp 1.xtek4014 $(TCMAN)/1.xtek4014
	chmod $(MODELIB) $(TCMAN)/1.xtek4014

xtek4014:	tek4014.sh ver.sh
	sed "s+DeFdMd+$(DEFDMD)+g" ver.sh tek4014.sh >xtek4014

dmd/tek4014.m:	$(OBJS5620) 
	$(MCC) $(MCFLAGS) $(OBJS5620) -o dmd/tek4014.m 

630/tek4014.m:	$(OBJS630) 
	$(MCC) $(MCFLAGS) $(OBJS630) -o 630/tek4014.m 

1.xtek4014:	xtek4014.1
	tbl -TX xtek4014.1 | nroff -man -T37 $(MANFL) | col > 1.xtek4014


########
#
#	All dependencies and rules not explicitly stated
#	(including header and nested header dependencies)
#
########

$(TCTERM)/bigapl.o:	bigapl.c
	$(MCC) $(MCFLAGS) -c bigapl.c
	mv bigapl.o $(TCTERM)/bigapl.o

$(TCTERM)/dclip.o:	dclip.c
$(TCTERM)/dclip.o:	dline.h
	$(MCC) $(MCFLAGS) -c dclip.c
	mv dclip.o $(TCTERM)/dclip.o

$(TCTERM)/dline.o:	dline.c
$(TCTERM)/dline.o:	dline.h
	$(MCC) $(MCFLAGS) -c dline.c
	mv dline.o $(TCTERM)/dline.o

$(TCTERM)/dlsegment.o:	dline.h
$(TCTERM)/dlsegment.o:	dlsegment.c
	$(MCC) $(MCFLAGS) -c dlsegment.c
	mv dlsegment.o $(TCTERM)/dlsegment.o

$(TCTERM)/flash.o:	flash.c
	$(MCC) $(MCFLAGS) -c flash.c
	mv flash.o $(TCTERM)/flash.o

$(TCTERM)/jdlineto.o:	dline.h
$(TCTERM)/jdlineto.o:	jdlineto.c
	$(MCC) $(MCFLAGS) -c jdlineto.c
	mv jdlineto.o $(TCTERM)/jdlineto.o

$(TCTERM)/jek.o:	jek.c
	$(MCC) $(MCFLAGS) -c jek.c
	mv jek.o $(TCTERM)/jek.o

$(TCTERM)/tiny.o:	tiny.c
	$(MCC) $(MCFLAGS) -c tiny.c
	mv tiny.o $(TCTERM)/tiny.o

$(TCTERM)/tinyapl.o:	tinyapl.c
	$(MCC) $(MCFLAGS) -c tinyapl.c
	mv tinyapl.o $(TCTERM)/tinyapl.o
	
