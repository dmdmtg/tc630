# Makefile for the 630 MTG Cartridge Utilities

#       Copyright (c) 1987 AT&T   
#       All Rights Reserved       

#       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   
#       The copyright notice above does not evidence any     
#       actual or intended publication of such source code.  

include ../stand.defs

COFFINCS=../inc/coff/filehdr.h ../inc/coff/scnhdr.h ../inc/coff/reloc.h

CFLAGS=-I../inc/coff -I../inc $(EXTRACFLAGS)
TC630=../..
TOOLNAME=630cart
SOURCE=README x630cart.1 makefile aconv.c cartfont.c cartutil.c \
	execram.c execrom.c fontconvrt.c getpeid.c ver.c \
	630cart.sh make_cart.sh cartextn.h cartglob.h \
	SAMPLE/Cplayer.make SAMPLE/SAMPLEfontlist SAMPLE/SAMPLEramlist \
	SAMPLE/SAMPLEromlist 


OBJSHOST=cartutil.o execram.o execrom.o
OBJS630=$(OBJSHOST)
OBJS5620=
PRODUCTSHOST=aconv cartfont execram execrom fontconvert getpeid
PRODUCTS630=$(PRODUCTSHOST) 1.x630cart
PRODUCTS5620=
INSTALLHOST=$(TCLIB)/dmdtools/xaconv $(TCLIB)/dmdtools/xgetpeid \
	$(TCLIB)/dmdtools/xfontconvert $(TCLIB)/dmdtools/xcartfont \
	$(TCLIB)/dmdtools/xexecram $(TCLIB)/dmdtools/xexecrom
INSTALL630=$(INSTALLHOST) \
	$(TCBIN)/x630cart $(TCLIB)/dmdtools/x630cart \
	$(TCMAN)/1.x630cart
INSTALL5620=

include ../stand.targ

$(OBJS630): $(COFFINCS) # make sure build links them in

1.x630cart: x630cart.1
	nroff -man -T37 $(MANFL) x630cart.1 > 1.x630cart

$(TCMAN)/1.x630cart: 1.x630cart
	cp 1.x630cart $(TCMAN)/1.x630cart
	chmod $(MODELIB) $(TCMAN)/1.x630cart

$(TCBIN)/x630cart:	../startup.sh
	cp ../startup.sh $(TCBIN)/x630cart
	chmod $(MODEBIN) $(TCBIN)/x630cart

$(TCLIB)/dmdtools/x630cart:	630cart.sh
	sed "s+DeFdMd+$(DEFDMD)+g" 630cart.sh >$(TCLIB)/dmdtools/x630cart
	chmod $(MODEBIN) $(TCLIB)/dmdtools/x630cart

$(TCLIB)/make_cart:	make_cart.sh
	sed "s+DeFdMd+$(DEFDMD)+g" make_cart.sh >$(TCLIB)/dmdtools/xmake_cart
	chmod $(MODEBIN) $(TCLIB)/dmdtools/xmake_cart

$(TCLIB)/dmdtools/xaconv: aconv
	cp aconv $(TCLIB)/dmdtools/xaconv
	chmod $(MODEBIN) $(TCLIB)/dmdtools/xaconv

$(TCLIB)/dmdtools/xgetpeid: getpeid
	cp getpeid $(TCLIB)/dmdtools/xgetpeid
	chmod $(MODEBIN) $(TCLIB)/dmdtools/xgetpeid

$(TCLIB)/dmdtools/xfontconvert:	fontconvert
	cp fontconvert $(TCLIB)/dmdtools/xfontconvert
	chmod $(MODEBIN) $(TCLIB)/dmdtools/xfontconvert
	
$(TCLIB)/dmdtools/xcartfont:	cartfont
	cp cartfont $(TCLIB)/dmdtools/xcartfont
	chmod $(MODEBIN) $(TCLIB)/dmdtools/xcartfont

$(TCLIB)/dmdtools/xexecram:	execram
	cp execram $(TCLIB)/dmdtools/xexecram
	chmod $(MODEBIN) $(TCLIB)/dmdtools/xexecram

$(TCLIB)/dmdtools/xexecrom:	execrom
	cp execrom $(TCLIB)/dmdtools/xexecrom
	chmod $(MODEBIN) $(TCLIB)/dmdtools/xexecrom


########
#
#	All dependencies and rules not explicitly stated
#	(including header and nested header dependencies)
#
########

aconv:	aconv.c
	$(CC) -o aconv aconv.c $(CFLAGS)

getpeid: getpeid.c
	$(CC) -o getpeid getpeid.c $(CFLAGS)

fontconvert:	fontconvrt.c
	$(CC) -o fontconvert fontconvrt.c $(CFLAGS)

cartfont: cartfont.c
	$(CC) -o cartfont cartfont.c $(CFLAGS)

execram:	execram.o cartutil.o
	$(CC) -o execram cartutil.o execram.o $(CFLAGS)

execrom:	execrom.o cartutil.o
	$(CC) -o execrom cartutil.o execrom.o $(CFLAGS)

cartutil.o:	cartutil.c
cartutil.o:	cartextn.h

execrom.o:	execrom.c
execrom.o:	cartglob.h

execram.o:	execram.c
execram.o:	cartglob.h

