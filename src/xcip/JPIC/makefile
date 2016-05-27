#       Copyright (c) 1987 AT&T   
#       All Rights Reserved       

#       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   
#       The copyright notice above does not evidence any     
#       actual or intended publication of such source code.  

# @(#)makefile	1.1.1.2	(8/24/92)

include ../../stand.defs

LEX_CMD=lex
YACC_CMD=yacc -d
MV=mv
TC630=../../..
TOOLNAME=xcip/JPIC
OBJSHOST=main.o print.o misc.o symtab.o blockgen.o \
	boxgen.o circgen.o arcgen.o linegen.o movegen.o \
	troffgen.o textgen.o pic2.o pltroff.o \
	picy.o picl.o
SOURCE=arcgen.c blockgen.c boxgen.c circgen.c driver.c \
	epic.c jp.c linegen.c main.c misc.c \
	movegen.c pic1.c pic2.c picl.c picy.c \
	pltroff.c print.c symtab.c textgen.c troffgen.c \
	pic.h y.tab.h picl.l picy.y 

PRODUCTSHOST=xjpic
PRODUCTS5620=$(PRODUCTSHOST)
PRODUCTS630=$(PRODUCTSHOST)
INSTALLHOST=$(TCBIN)/xjpic
INSTALL5620=$(INSTALLHOST)
INSTALL630=$(INSTALLHOST)
CLEANHOST=$(OBJSHOST) picl.c picy.c y.tab.h
CLEAN5620=$(CLEANHOST)
CLEAN630=$(CLEANHOST)

include ../../stand.targ

$(TCBIN)/xjpic: xjpic 
	cp xjpic $(TCBIN)/xjpic
	chmod $(MODEBIN) $(TCBIN)/xjpic

xjpic: picy.o picl.o $(OBJSHOST)
	$(CC) -o xjpic $(OBJSHOST) -lm $(CFLAGS)

########
########

arcgen.o:	arcgen.c
arcgen.o:	pic.h
arcgen.o:	y.tab.h
	$(CC)  $(CFLAGS)  -c arcgen.c

blockgen.o:	blockgen.c
blockgen.o:	pic.h
blockgen.o:	y.tab.h
	$(CC)  $(CFLAGS)  -c blockgen.c

boxgen.o:	boxgen.c
boxgen.o:	pic.h
boxgen.o:	y.tab.h
	$(CC)  $(CFLAGS)  -c boxgen.c

circgen.o:	circgen.c
circgen.o:	pic.h
circgen.o:	y.tab.h
	$(CC)  $(CFLAGS)  -c circgen.c

linegen.o:	linegen.c
linegen.o:	pic.h
linegen.o:	y.tab.h
	$(CC)  $(CFLAGS)  -c linegen.c

main.o:	main.c
main.o:	pic.h
main.o:	../version.h
main.o:	y.tab.h
	$(CC)  $(CFLAGS)  -c main.c

misc.o:	misc.c
misc.o:	pic.h
misc.o:	y.tab.h
	$(CC)  $(CFLAGS)  -c misc.c

movegen.o:	movegen.c
movegen.o:	pic.h
movegen.o:	y.tab.h
	$(CC)  $(CFLAGS)  -c movegen.c

pic2.o:	pic.h
pic2.o:	pic2.c
pic2.o:	y.tab.h
	$(CC)  $(CFLAGS)  -c pic2.c

picl.c:	picl.l
	$(LEX_CMD) -t picl.l > picl.c

picl.o:	pic.h
picl.o:	picl.c
picl.o:	y.tab.h
	$(CC)  $(CFLAGS)  -c picl.c

picy.c:	picy.y
	$(YACC_CMD) picy.y;\
	$(MV) y.tab.c picy.c

picy.o:	picy.c
	$(CC)  $(CFLAGS)  -c picy.c

pltroff.o:	pic.h
pltroff.o:	pltroff.c
pltroff.o:	y.tab.h
	$(CC)  $(CFLAGS)  -c pltroff.c

print.o:	pic.h
print.o:	print.c
print.o:	y.tab.h
	$(CC)  $(CFLAGS)  -c print.c

symtab.o:	pic.h
symtab.o:	symtab.c
symtab.o:	y.tab.h
	$(CC)  $(CFLAGS)  -c symtab.c

textgen.o:	pic.h
textgen.o:	textgen.c
textgen.o:	y.tab.h
	$(CC)  $(CFLAGS)  -c textgen.c

troffgen.o:	pic.h
troffgen.o:	troffgen.c
troffgen.o:	y.tab.h
	$(CC)  $(CFLAGS)  -c troffgen.c


