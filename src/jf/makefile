#	Copyright (c) 1987 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

# @(#)makefile	1.1.1.14	(6/20/90)

########################################################################
#
#	jf
#
########################################################################

### Definitions ###

include ../stand.defs

PRODUCTSHOST=
PRODUCTS5620=	$(TCTERM)/jf.m 1.xjf xjf
PRODUCTS630=	$(PRODUCTS5620)

INSTALLHOST=
INSTALL5620=	$(TCBIN)/xjf			\
		$(TCLIB)/dmdtools/xjf		\
		$(TCLIB)/$(TCTERM)/xjf.m	\
		$(TCMAN)/1.xjf

INSTALL630=	$(INSTALL5620)

TOOLNAME=	jf
TC630=		../..
SOURCE=		README makefile brotate.c buttons.c charsubs.c filesubs.c fontsubs.c foo2.c foo3.c getopt.c hollow.c kbdstring.c lexit3.c magnify.c main.c menuhit3.c mice.c myget.c outfont.c reshaped.c rsupport.c spaceman.c strcat2.c sfont.h jf.h jf.sh ver.sh tmenuhit.c tmenuhit.h xjf.1

OBJSHOST=
OBJS630=	$(TCTERM)/brotate.o	\
		$(TCTERM)/buttons.o	\
		$(TCTERM)/charsubs.o	\
		$(TCTERM)/filesubs.o	\
		$(TCTERM)/fontsubs.o	\
		$(TCTERM)/foo2.o	\
		$(TCTERM)/foo3.o	\
		$(TCTERM)/getopt.o	\
		$(TCTERM)/hollow.o	\
		$(TCTERM)/kbdstring.o	\
		$(TCTERM)/lexit3.o	\
		$(TCTERM)/magnify.o	\
		$(TCTERM)/main.o	\
		$(TCTERM)/menuhit3.o	\
		$(TCTERM)/mice.o	\
		$(TCTERM)/myget.o	\
		$(TCTERM)/outfont.o	\
		$(TCTERM)/reshaped.o	\
		$(TCTERM)/rsupport.o	\
		$(TCTERM)/spaceman.o	\
		$(TCTERM)/strcat2.o

OBJS5620=	$(OBJS630) \
		$(TCTERM)/tmenuhit.o

LINT=		lint
# until there is a MISC target in stand.targ, being able to do
# both the 5620 and the 630 will be difficult.
LNFLAGS=	-n -I$(DMD)/include -I.. -I$(Tc)/src/misc -DDMD630
LNFILES=	brotate.ln	\
		buttons.ln	\
		charsubs.ln	\
		filesubs.ln	\
		fontsubs.ln	\
		foo2.ln		\
		foo3.ln		\
		getopt.ln	\
		hollow.ln	\
		kbdstring.ln	\
		lexit3.ln	\
		magnify.ln	\
		main.ln		\
		menuhit3.ln	\
		mice.ln		\
		myget.ln	\
		outfont.ln	\
		reshaped.ln	\
		rsupport.ln	\
		spaceman.ln	\
		strcat2.ln

LINTLIB=	$(TCSRC)/misc/llib-l630.ln

### Targets ###


include ../stand.targ

$(TCMAN)/1.xjf:	1.xjf
	cp $(@F) $@
	chmod $(MODELIB) $@

1.xjf:	xjf.1
	nroff -man -T37 $(MANFL) xjf.1 > 1.xjf

$(TCBIN)/xjf:	../startup.sh
	cp ../startup.sh $@
	chmod $(MODEBIN) $@

$(TCLIB)/dmdtools/xjf:	xjf
	cp xjf $@
	chmod $(MODEBIN) $@

xjf:	ver.sh jf.sh
	sed "s+DeFdMd+$(DEFDMD)+g" ver.sh jf.sh > xjf


$(TCLIB)/$(TCTERM)/xjf.m:	$(TCTERM)/jf.m
	cp $(TCTERM)/jf.m $@
#	@if [ $(TCTERM) != dmd ]; then	\
		echo '\t' $(MSTRIP) $@;	\
		$(MSTRIP) -m $@;	\
	fi
	chmod $(MODELIB) $@

dmd/jf.m:	$(OBJS5620)
	$(MCC) $(MCFLAGS) -o $@ $(OBJS5620)

630/jf.m:	$(OBJS630)
	$(MCC) $(MCFLAGS) -o $@ $(OBJS630)


$(TCTERM)/brotate.o:	brotate.c
	$(MCC) $(MCFLAGS) -c brotate.c
	mv $(@F) $(TCTERM)

$(TCTERM)/buttons.o:	buttons.c
	$(MCC) $(MCFLAGS) -c buttons.c
	mv $(@F) $(TCTERM)

$(TCTERM)/charsubs.o:	jf.h sfont.h
$(TCTERM)/charsubs.o:	charsubs.c
	$(MCC) $(MCFLAGS) -c charsubs.c
	mv $(@F) $(TCTERM)

$(TCTERM)/filesubs.o:	jf.h sfont.h
$(TCTERM)/filesubs.o:	filesubs.c
	$(MCC) $(MCFLAGS) -c filesubs.c
	mv $(@F) $(TCTERM)

$(TCTERM)/fontsubs.o:	jf.h sfont.h
$(TCTERM)/fontsubs.o:	fontsubs.c
	$(MCC) $(MCFLAGS) -c fontsubs.c
	mv $(@F) $(TCTERM)

$(TCTERM)/foo2.o:	foo2.c
	$(MCC) $(MCFLAGS) -c foo2.c
	mv $(@F) $(TCTERM)

$(TCTERM)/foo3.o:	foo3.c
	$(MCC) $(MCFLAGS) -c foo3.c
	mv $(@F) $(TCTERM)

$(TCTERM)/getopt.o:	getopt.c
	$(MCC) $(MCFLAGS) -c getopt.c
	mv $(@F) $(TCTERM)

$(TCTERM)/hollow.o:	jf.h sfont.h
$(TCTERM)/hollow.o:	hollow.c
	$(MCC) $(MCFLAGS) -c hollow.c
	mv $(@F) $(TCTERM)

$(TCTERM)/kbdstring.o:	kbdstring.c
	$(MCC) $(MCFLAGS) -c kbdstring.c
	mv $(@F) $(TCTERM)

$(TCTERM)/lexit3.o:	lexit3.c
	$(MCC) $(MCFLAGS) -c lexit3.c
	mv $(@F) $(TCTERM)

$(TCTERM)/magnify.o:	magnify.c
	$(MCC) $(MCFLAGS) -c magnify.c
	mv $(@F) $(TCTERM)

dmd/main.o:	tmenuhit.h
$(TCTERM)/main.o:	jf.h sfont.h
$(TCTERM)/main.o:	main.c
	$(MCC) $(MCFLAGS) -c main.c
	mv $(@F) $(TCTERM)

$(TCTERM)/menuhit3.o:	menuhit3.c
	$(MCC) $(MCFLAGS) -c menuhit3.c
	mv $(@F) $(TCTERM)

$(TCTERM)/mice.o:	mice.c
	$(MCC) $(MCFLAGS) -c mice.c
	mv $(@F) $(TCTERM)

$(TCTERM)/myget.o:	jf.h sfont.h
$(TCTERM)/myget.o:	myget.c
	$(MCC) $(MCFLAGS) -c myget.c
	mv $(@F) $(TCTERM)

$(TCTERM)/outfont.o:	outfont.c
	$(MCC) $(MCFLAGS) -c outfont.c
	mv $(@F) $(TCTERM)

$(TCTERM)/reshaped.o:	reshaped.c
	$(MCC) $(MCFLAGS) -c reshaped.c
	mv $(@F) $(TCTERM)

$(TCTERM)/rsupport.o:	rsupport.c
	$(MCC) $(MCFLAGS) -c rsupport.c
	mv $(@F) $(TCTERM)

$(TCTERM)/spaceman.o:	jf.h sfont.h
$(TCTERM)/spaceman.o:	spaceman.c
	$(MCC) $(MCFLAGS) -c spaceman.c
	mv $(@F) $(TCTERM)

$(TCTERM)/strcat2.o:	strcat2.c
	$(MCC) $(MCFLAGS) -c strcat2.c
	mv $(@F) $(TCTERM)

$(TCTERM)/tmenuhit.o:	tmenuhit.h
$(TCTERM)/tmenuhit.o:	tmenuhit.c
	$(MCC) $(MCFLAGS) -c tmenuhit.c
	mv $(@F) $(TCTERM)


lint:	$(LNFILES) Makefile $(LINTLIB)
	$(LINT) $(LNFLAGS) $(LNFILES) $(LINTLIB)

.SUFFIXES:	.ln
.c.ln:
	$(LINT) $(LNFLAGS) -c $< $(LINTLIB)
