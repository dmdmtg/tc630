#       Copyright (c) 1987 AT&T   
#       All Rights Reserved       

#       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   
#       The copyright notice above does not evidence any     
#       actual or intended publication of such source code.  

# @(#)Makefile	1.1.1.11	(6/20/90)

include ../../stand.defs

PRODUCTSHOST=	tdmd
PRODUCTS5620=	$(PRODUCTSHOST)
PRODUCTS630=	$(PRODUCTSHOST)

INSTALLHOST=	$(TCLIB)/dmdtools/xtdmd
INSTALL5620=	$(INSTALLHOST) $(TCBIN)/xtdmd
INSTALL630=	$(INSTALLHOST) $(TCBIN)/xtdmd

TOOLNAME=	tdmd/host
TC630=		../../..

SOURCE=		Makefile driver.c arc.c circle.c close.c defs.c \
		delay.c erase.c fill.c jplot.h label.c line.c linemod.c \
		move.c open.c point.c space.c subr.c ver.c

OBJSHOST=	driver.o \
		arc.o circle.o close.o defs.o delay.o \
		erase.o fill.o label.o line.o linemod.o \
		move.o open.o point.o space.o subr.o ver.o
OBJS5620=	$(OBJSHOST)
OBJS630=	$(OBJSHOST)

DbFile=		tdmd.debug
DEBUG=		-DDEBUG
CFLAGS=		$(DEBUG) -I$(DMDSYS)/include -I../h -I../../inc \
			-DDbFile=\"$(DbFile)\" $(EXTRACFLAGS)
LINT=		lint
LNFLAGS=	$(CFLAGS)
LNFILES=	driver.ln \
		arc.ln circle.ln close.ln defs.ln delay.ln \
		erase.ln fill.ln label.ln line.ln linemod.ln \
		move.ln open.ln point.ln space.ln subr.ln



include ../../stand.targ

$(TCBIN)/xtdmd:	../../startup.sh
	cp ../../startup.sh $@
	chmod $(MODEBIN) $@

$(TCLIB)/dmdtools/xtdmd:	tdmd
	cp tdmd $@
	chmod $(MODEBIN) $@

tdmd: $(OBJSHOST)
	@HOSTAGENT=""; set -x; \
	for F in $(LIBWINPATHS); do \
		if [ -f "$$F" ]; then HOSTAGENT=$$F; break; fi; done; \
	$(CC) $(OBJSHOST) -o $@ $$HOSTAGENT $(CFLAGS)

arc.o: jplot.h ../h/tdmd.h
arc.o: arc.c
	$(CC) $(CFLAGS) -c arc.c

circle.o: jplot.h
circle.o: circle.c
	$(CC) $(CFLAGS) -c circle.c

close.o: jplot.h
close.o: close.c
	$(CC) $(CFLAGS) -c close.c

defs.o: jplot.h
defs.o: defs.c
	$(CC) $(CFLAGS) -c defs.c

delay.o: jplot.h ../h/tdmd.h
delay.o: delay.c
	$(CC) $(CFLAGS) -c delay.c

driver.o: jplot.h
driver.o: driver.c
	$(CC) $(CFLAGS) -c driver.c

erase.o: jplot.h ../h/tdmd.h
erase.o: erase.c
	$(CC) $(CFLAGS) -c erase.c

fill.o: jplot.h ../h/tdmd.h
fill.o: fill.c
	$(CC) $(CFLAGS) -c fill.c

label.o: jplot.h
label.o: label.c
	$(CC) $(CFLAGS) -c label.c

line.o: jplot.h ../h/tdmd.h
line.o: line.c
	$(CC) $(CFLAGS) -c line.c

linemod.o: jplot.h ../h/tdmd.h
linemod.o: linemod.c
	$(CC) $(CFLAGS) -c linemod.c

move.o: jplot.h ../h/tdmd.h
move.o: move.c
	$(CC) $(CFLAGS) -c move.c

open.o:	jplot.h
open.o: open.c
	@if [ -n "$(DEFDMD)" ];then D=-DDEFDMD=\"$(DEFDMD)\";else D=;fi;set -x;\
		$(CC) $(CFLAGS) $$D -c open.c

point.o: jplot.h
point.o: point.c
	$(CC) $(CFLAGS) -c point.c

space.o: jplot.h
space.o: space.c
	$(CC) $(CFLAGS) -c space.c

subr.o: jplot.h ../h/tdmd.h
subr.o: subr.c
	$(CC) $(CFLAGS) -c subr.c

lint:	$(LNFILES)
	$(LINT) $(LNFLAGS) $(LNFILES)

.SUFFIXES:	.ln
.c.ln:
	$(LINT) $(LNFLAGS) -c $<
