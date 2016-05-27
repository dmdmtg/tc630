include ../../stand.defs

XFLAGS  = -DBSD
CFLAGS	= -I.. -I. -DX11 -DXBUG $(XFLAGS) $(EXTRACFLAGS)
SYSLIBS	= -lX11
LIB     = libj.a
LIBS	= ${LIB} ${SYSLIBS}

COMH=	jerq.h menu.h rcv.h

OFILES=	ToBitmap.o add.o bitblt.o button.o circle.o cos.o getpoint.o \
	kbdrcv.o menuhit.o mhit.o muxbuf.o rcvfill.o rev.o sizehints.o \
	snd.o util.o wait.o x11.o

${LIB}: ${OFILES}
	rm -f ${LIB}
	ar cr ${LIB} ${OFILES}
	(ranlib ${LIB}) 2>/dev/null || true

clean:
	rm -f ${OFILES} ${LIB} ${TESTS}

${OFILES} ${TESTO}: jerq.h

mhit.o t5.o: menu.h

kbdrcv.o rcvfill.o wait.o: rcv.h

