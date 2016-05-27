#       Copyright (c) 1987 AT&T   
#       All Rights Reserved       

#       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   
#       The copyright notice above does not evidence any     
#       actual or intended publication of such source code.  

# @(#)makefile	1.1.1.1	(8/12/92)

include ../stand.defs

TC630=../..
TOOLNAME=xcip
OBJSHOST=
OBJS5620=$(TCTERM)/buttons.o $(TCTERM)/cip.o $(TCTERM)/commands.o \
	$(TCTERM)/draw.o $(TCTERM)/edit.o $(TCTERM)/icons.o $(TCTERM)/menus.o \
	$(TCTERM)/readPic.o $(TCTERM)/pic.o $(TCTERM)/space.o \
	$(TCTERM)/things.o $(TCTERM)/track.o $(TCTERM)/transform.o \
	$(TCTERM)/tmenuhit.o
OBJS630=$(OBJS5620)
OBJSX11=$(OBJS5620)
SOURCE=buttons.c cip.c commands.c draw.c edit.c icons.c menus.c pic.c \
	readPic.c space.c things.c tmenuhit.c tmenuhit.h track.c transform.c \
	xcip.sh xcipfront.sh logo.pic cip.h version.h xcip.1 README

PRODUCTSHOST=
PRODUCTS5620=$(TCTERM)/xcip.m 1.xcip
PRODUCTS630=$(PRODUCTS5620)
INSTALLHOST=
INSTALL5620=$(TCLIB)/$(TCTERM)/xcip.m $(TCMAN)/1.xcip \
	$(TCBIN)/xcip $(TCLIB)/dmdtools/xcip
INSTALL630=$(INSTALL5620)
SUBDIRSHOST=JPIC
SUBDIRS5620=$(SUBDIRSHOST)
SUBDIRS630=$(SUBDIRSHOST)


include ../stand.targ

$(TCBIN)/xcip: xcipfront.sh
	cp xcipfront.sh $(TCBIN)/xcip
	chmod $(MODEBIN) $(TCBIN)/xcip

$(TCLIB)/dmdtools/xcip: xcip.sh
	sed "s+DeFdMd+$(DEFDMD)+g" xcip.sh >$(TCLIB)/dmdtools/xcip
	chmod $(MODEBIN) $(TCLIB)/dmdtools/xcip

$(TCLIB)/dmd/xcip.m:   dmd/xcip.m
	cp dmd/xcip.m $(TCLIB)/dmd/xcip.m
	chmod $(MODELIB) $(TCLIB)/dmd/xcip.m

$(TCLIB)/630/xcip.m:   630/xcip.m
	cp 630/xcip.m $(TCLIB)/630/xcip.m
#	$(MSTRIP) -m $(TCLIB)/630/xcip.m
	chmod $(MODELIB) $(TCLIB)/630/xcip.m

$(TCMAN)/1.xcip:       1.xcip
	cp 1.xcip $(TCMAN)/1.xcip
	chmod $(MODELIB) $(TCMAN)/1.xcip

630/xcip.m:     $(OBJS630)
	$(MCC) $(MCFLAGS) $(OBJS630) -o 630/xcip.m

dmd/xcip.m:     $(OBJS5620)
	$(MCC) $(MCFLAGS) $(OBJS5620) -o dmd/xcip.m

1.xcip:        xcip.1
	nroff -man -T37 $(MANFL) xcip.1 > 1.xcip


########
#  X11 version of cip
########
XFLAGS=-DBSD
# for SunOS
# XFLAGS=-DBSD -I/usr/openwin/include -L/usr/openwin/lib
# for SYSVR4:
# XFLAGS=-DSYSV -DSVR4 -lnsl -lsocket
# for Solaris
# XFLAGS=-DSYSV -DSVR4 -I/usr/openwin/include -L/usr/openwin/lib -lnsl -lsocket

x11: LIBDMDX11/libj.a
	$(MAKE) TCTERM=X11 MCC=cc DMD="" \
		MCFLAGS="-DX11 -ILIBDMDX11 $(XFLAGS) $(EXTRACFLAGS)" xcip.x11

xcip.x11: $(OBJSX11)
	$(CC) -o xcip.x11 $(OBJSX11) LIBDMDX11/libj.a $(MCFLAGS) -lX11

LIBDMDX11/libj.a: FRC
	cd LIBDMDX11; $(MAKE) XFLAGS="$(XFLAGS)" libj.a

clean: cleanx11

cleanx11:
	cd LIBDMDX11; $(MAKE) clean
	rm -f X11/*.o

clobber: clobberx11

clobberx11: cleanx11
	rm -f xcip.x11

FRC:


########
#	CIP logo
#
#   Convert a series of lines in logo.pic to C data in logo.data.
#
########

logo.data: logo.pic
	grep '^line  from' logo.pic | \
	sed -e 's/^line  from \([0-9][0-9]*\),\([0-9][0-9]*\) to \([0-9][0-9]*\),\([0-9][0-9]*\)/\1,\2,\3,\4,/' \
	> logo.data



########
#
#	dependencies and rules 
#
########

$(TCTERM)/buttons.o:	buttons.c
$(TCTERM)/buttons.o:	cip.h
	$(MCC) $(MCFLAGS) -c buttons.c
	mv buttons.o $(TCTERM)/buttons.o

$(TCTERM)/cip.o:	cip.c
$(TCTERM)/cip.o:	cip.h
$(TCTERM)/cip.o:	tmenuhit.h
$(TCTERM)/cip.o:	version.h
	$(MCC) $(MCFLAGS) -c cip.c
	mv cip.o $(TCTERM)/cip.o

$(TCTERM)/commands.o:	commands.c
$(TCTERM)/commands.o:	cip.h
	$(MCC) $(MCFLAGS) -c commands.c
	mv commands.o $(TCTERM)/commands.o

$(TCTERM)/draw.o:	draw.c
$(TCTERM)/draw.o:	cip.h
$(TCTERM)/draw.o: logo.data
	$(MCC) $(MCFLAGS) -c draw.c
	mv draw.o $(TCTERM)/draw.o

$(TCTERM)/edit.o:	edit.c
$(TCTERM)/edit.o:	cip.h
	$(MCC) $(MCFLAGS) -c edit.c
	mv edit.o $(TCTERM)/edit.o

$(TCTERM)/icons.o:	icons.c
$(TCTERM)/icons.o:	cip.h
	$(MCC) $(MCFLAGS) -c icons.c
	mv icons.o $(TCTERM)/icons.o

$(TCTERM)/menus.o:	menus.c
$(TCTERM)/menus.o:	cip.h
$(TCTERM)/menus.o:	tmenuhit.h
	$(MCC) $(MCFLAGS) -c menus.c
	mv menus.o $(TCTERM)/menus.o

$(TCTERM)/pic.o:	pic.c
$(TCTERM)/pic.o:	cip.h
	$(MCC) $(MCFLAGS) -c pic.c
	mv pic.o $(TCTERM)/pic.o

$(TCTERM)/readPic.o:	readPic.c
$(TCTERM)/readPic.o:	cip.h
	$(MCC) $(MCFLAGS) -c readPic.c
	mv readPic.o $(TCTERM)/readPic.o

$(TCTERM)/space.o:	space.c
$(TCTERM)/space.o:	cip.h
	$(MCC) $(MCFLAGS) -c space.c
	mv space.o $(TCTERM)/space.o

$(TCTERM)/things.o:	things.c
$(TCTERM)/things.o:	cip.h
	$(MCC) $(MCFLAGS) -c things.c
	mv things.o $(TCTERM)/things.o

$(TCTERM)/tmenuhit.o:	tmenuhit.c
$(TCTERM)/tmenuhit.o:	tmenuhit.h
	$(MCC) $(MCFLAGS) -c tmenuhit.c
	mv tmenuhit.o $(TCTERM)/tmenuhit.o

$(TCTERM)/track.o:	track.c
$(TCTERM)/track.o:	cip.h
	$(MCC) $(MCFLAGS) -c track.c
	mv track.o $(TCTERM)/track.o

$(TCTERM)/transform.o:	transform.c
$(TCTERM)/transform.o:	cip.h
	$(MCC) $(MCFLAGS) -c transform.c
	mv transform.o $(TCTERM)/transform.o


