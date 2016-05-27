#       Copyright (c) 1987 AT&T   
#       All Rights Reserved       

#       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   
#       The copyright notice above does not evidence any     
#       actual or intended publication of such source code.  

# @(#)Makefile	1.1.1.7	(5/24/88)

include ../../stand.defs

PRODUCTS5620=	$(TCTERM)/t$(TCTERM).m
PRODUCTS630=	$(TCTERM)/t$(TCTERM).m

INSTALL5620=	$(TCLIB)/$(TCTERM)/xt$(TCTERM).m
INSTALL630=	$(TCLIB)/$(TCTERM)/xt$(TCTERM).m

TOOLNAME=	tdmd/term
TC630=		../../..
SOURCE=		Makefile dlarc.c dlsegment.c getopt.c jplot.c

OBJECTS=	$(TCTERM)/jplot.o \
		$(TCTERM)/dlarc.o \
		$(TCTERM)/dlsegment.o \
		$(TCTERM)/getopt.o
OBJS630=	$(OBJECTS)
OBJS5620=	$(OBJECTS)

LINT=		lint
# until there is a MISC target in stand.targ, being able to do
# both the 5620 and the 630 will be difficult.
LNFLAGS=	-n -I$(DMD)/include -I../h -I$(Tc)/src/misc -DDMD630
LNFILES=	jplot.ln dlarc.ln dlsegment.ln getopt.ln
LINTLIB=	$(TCSRC)/misc/llib-l630.ln


include ../../stand.targ

$(TCLIB)/$(TCTERM)/xt$(TCTERM).m: $(TCTERM)/t$(TCTERM).m
	cp $(TCTERM)/t$(TCTERM).m $@
#	$(MSTRIP) $@
	chmod $(MODELIB) $@

$(TCTERM)/t$(TCTERM).m: $(OBJECTS)
	$(MCC) $(MCFLAGS) $(OBJECTS) -o $@

$(TCTERM)/jplot.o: jplot.c
$(TCTERM)/jplot.o: ../h/tdmd.h
	$(MCC) $(MCFLAGS) -I../h -c jplot.c
	mv jplot.o $(TCTERM)/jplot.o

$(TCTERM)/dlarc.o: dlarc.c
	$(MCC) $(MCFLAGS) -c dlarc.c
	mv dlarc.o $(TCTERM)/dlarc.o

$(TCTERM)/dlsegment.o: dlsegment.c
	$(MCC) $(MCFLAGS) -c dlsegment.c
	mv dlsegment.o $(TCTERM)/dlsegment.o

$(TCTERM)/getopt.o: getopt.c
	$(MCC) $(MCFLAGS) -c getopt.c
	mv getopt.o $(TCTERM)/getopt.o


lint:	$(LNFILES) Makefile ../h/tdmd.h $(LINTLIB)
	$(LINT) $(LNFLAGS) $(LNFILES) $(LINTLIB)

.SUFFIXES:	.ln
.c.ln:
	$(LINT) $(LNFLAGS) -c $< $(LINTLIB)

$(LNFILES):	../h/tdmd.h
