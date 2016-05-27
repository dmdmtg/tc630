#
#	'Makefile' for CENTIPEDE by Pat Autilio	11/3/83
#
# JERQ=/usr/jerq
# JERQINCLUDE=$(JERQ)/include
# JERQBIN=$(JERQ)/bin
#
# 'CC' works for the 'blit' but will no doubt be different for the
#	5620.
# CC=$(JERQBIN)/mcc -I$(JERQINCLUDE) -Dmc68000

DMDBIN=$(DMD)/bin
DMDLIB=$(DMD)/lib
DMDSRC=$(DMD)/src
DMDINCLUDE=$(DMD)/include
DMDCC=$(DMDBIN)/dmdcc
DMDFLAGS=-I$(DMDINCLUDE)
DMDLD=$(DMDBIN)/dmdld
CC=$(DMDCC) $(DMDFLAGS)

MCC=$(DMDCC)
MCFLAGS=-g


CFLAGS=
CFILES = 	digits.c globals.c player.c\
		chk_coll.c chk_mecoll.c error.c exit_game.c\
		init_game.c init_round.c killplayer.c personchoose.c\
		playdisp.c put_chunk.c init_scr.c c_rand.c scorpchoose.c\
		shotchoose.c slugchoose.c spiderchoose.c start_stuff.c\
		wormchoose.c
OFILES = 	digits.o globals.o player.o\
		chk_coll.o chk_mecoll.o error.o exit_game.o\
		init_game.o init_round.o killplayer.o personchoose.o\
		playdisp.o put_chunk.o init_scr.o c_rand.o scorpchoose.o\
		shotchoose.o slugchoose.o spiderchoose.o start_stuff.o\
		wormchoose.o

OBJSCART = 	digits.o globals.o Cplayer.o\
		chk_coll.o chk_mecoll.o error.o exit_game.o\
		init_game.o init_round.o killplayer.o personchoose.o\
		playdisp.o put_chunk.o init_scr.o c_rand.o scorpchoose.o\
		shotchoose.o slugchoose.o spiderchoose.o start_stuff.o\
		wormchoose.o
#
# 'make' is all you have to make for this to work.
#
all:	cent.m

cent.m:	$(OFILES)
	dmdcc -o cent.m $(OFILES)

cart_Cplayer:	$(OBJSCART)
	$(MLD) $(MLDFLAGS) $(MLNDIR) $(DMD)/lib/crtm.o $(OBJSCART) \
	$(MLDOBJ) -ljx -lj -lfw -lc $(MTLMAP)


digits.o:	playdefs.h
	$(MCC) $(MCFLAGS) -c digits.c

globals.o:	playdefs.h
	$(MCC) $(MCFLAGS) -c globals.c

player.o:	playdefs.h
	$(MCC) $(MCFLAGS) -c player.c

Cplayer.o:	Cplayer.c
Cplayer.o:	playdefs.h
	$(MCC) $(MCFLAGS) -c Cplayer.c

chk_coll.o:	playdefs.h
	$(MCC) $(MCFLAGS) -c chk_coll.c

chk_mecoll.o:	playdefs.h
	$(MCC) $(MCFLAGS) -c chk_mecoll.c

error.o:	playdefs.h
	$(MCC) $(MCFLAGS) -c error.c

exit_game.o:	playdefs.h
	$(MCC) $(MCFLAGS) -c exit_game.c

init_game.o:	playdefs.h
	$(MCC) $(MCFLAGS) -c init_game.c

init_round.o:	playdefs.h
	$(MCC) $(MCFLAGS) -c init_round.c

killplayer.o:	playdefs.h
	$(MCC) $(MCFLAGS) -c killplayer.c

personchoose.o:	playdefs.h
	$(MCC) $(MCFLAGS) -c personchoose.c

playdisp.o:	playdefs.h
	$(MCC) $(MCFLAGS) -c playdisp.c

put_chunk.o :	playdefs.h
	$(MCC) $(MCFLAGS) -c put_chunk.c

init_scr.o :	playdefs.h
	$(MCC) $(MCFLAGS) -c init_scr.c

c_rand.o:	c_rand.c
	$(MCC) $(MCFLAGS) -c c_rand.c
scorpchoose.o :	playdefs.h
	$(MCC) $(MCFLAGS) -c scorpchoose.c

shotchoose.o :	playdefs.h
	$(MCC) $(MCFLAGS) -c shotchoose.c

slugchoose.o :	playdefs.h
	$(MCC) $(MCFLAGS) -c slugchoose.c

spiderchoose.o:	playdefs.h
	$(MCC) $(MCFLAGS) -c spiderchoose.c

start_stuff.o :	playdefs.h
	$(MCC) $(MCFLAGS) -c start_stuff.c

wormchoose.o:	playdefs.h
	$(MCC) $(MCFLAGS) -c wormchoose.c


clean:
	-rm -f *.o
#
# 'make cpiofile' packs the source up for sending it somewhere.
#
cpiofile:
	ls cent.README Makefile playdefs.h *.c | cpio -oc > cent.cpio
#
# End of 'Makefile'
#
