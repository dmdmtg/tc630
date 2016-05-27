#       Copyright (c) 1987 AT&T   
#       All Rights Reserved       

#       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   
#       The copyright notice above does not evidence any     
#       actual or intended publication of such source code.  

# @(#)makefile	1.1.1.8	(11/19/87)

# Makefile for mpx demos
#
include ../../stand.defs

TC630=../../..
OBJS5620=
OBJS630=
PRODUCTS5620=dmd/bltdemo.j dmd/disc.j dmd/elevator.j dmd/moire.j dmd/munch.j \
	    dmd/spheres.j dmd/win.j dmd/one.j dmd/a.j dmd/b.j dmd/clock1.j \
	    dmd/juggle.j dmd/m.j dmd/maxwell.j dmd/nball.j dmd/rae.j \
	    dmd/clock2.j dmd/scope.j dmd/weird.j
PRODUCTS630=630/bltdemo.m 630/disc.m 630/elevator.m 630/moire.m 630/munch.m \
           630/spheres.m 630/win.m 630/one.m 630/a.m 630/b.m 630/clock1.m \
	   630/juggle.m 630/m.m 630/maxwell.m 630/nball.m 630/rae.m \
	   630/clock2.m 630/scope.m 630/weird.m
INSTALL5620=install5620
INSTALL630=installs630

IN5620FILES = $(TCLIB)/xdmddemo/dmd/demolibsa/xbltdemo \
	   $(TCLIB)/xdmddemo/dmd/demolibsa/xdisc \
	   $(TCLIB)/xdmddemo/dmd/demolibsa/xelevator \
	   $(TCLIB)/xdmddemo/dmd/demolibsa/xmoire \
	   $(TCLIB)/xdmddemo/dmd/demolibsa/xmunch \
	   $(TCLIB)/xdmddemo/dmd/demolibsa/xspheres \
	   $(TCLIB)/xdmddemo/dmd/demolibsa/xwin \
	   $(TCLIB)/xdmddemo/dmd/demolibsa/xone \
	   $(TCLIB)/xdmddemo/dmd/demolib/xa \
	   $(TCLIB)/xdmddemo/dmd/demolib/xb \
	   $(TCLIB)/xdmddemo/dmd/demolib/xclock1 \
	   $(TCLIB)/xdmddemo/dmd/demolib/xclock2 \
	   $(TCLIB)/xdmddemo/dmd/demolib/xjuggle \
	   $(TCLIB)/xdmddemo/dmd/demolib/xm \
	   $(TCLIB)/xdmddemo/dmd/demolib/xmaxwell \
	   $(TCLIB)/xdmddemo/dmd/demolib/xnball \
	   $(TCLIB)/xdmddemo/dmd/demolib/xrae \
	   $(TCLIB)/xdmddemo/dmd/demolib/xscope \
	   $(TCLIB)/xdmddemo/dmd/demolib/xweird \
	   $(TCLIB)/xdmddemo/demobin/xclock1

IN630FILES= $(TCLIB)/xdmddemo/630/xbltdemo \
	   $(TCLIB)/xdmddemo/630/xdisc \
	   $(TCLIB)/xdmddemo/630/xelevator \
	   $(TCLIB)/xdmddemo/630/xmoire \
	   $(TCLIB)/xdmddemo/630/xmunch \
	   $(TCLIB)/xdmddemo/630/xspheres \
	   $(TCLIB)/xdmddemo/630/xwin \
	   $(TCLIB)/xdmddemo/630/xone \
	   $(TCLIB)/xdmddemo/630/xa \
	   $(TCLIB)/xdmddemo/630/xb \
	   $(TCLIB)/xdmddemo/630/xclock1 \
	   $(TCLIB)/xdmddemo/630/xclock2 \
	   $(TCLIB)/xdmddemo/630/xjuggle \
	   $(TCLIB)/xdmddemo/630/xm \
	   $(TCLIB)/xdmddemo/630/xmaxwell \
	   $(TCLIB)/xdmddemo/630/xnball \
	   $(TCLIB)/xdmddemo/630/xrae \
	   $(TCLIB)/xdmddemo/630/xscope \
	   $(TCLIB)/xdmddemo/630/xweird \
	   $(TCLIB)/xdmddemo/demobin/xclock1

SOURCE=a.c b.c bltdemo.c clock1.c clock1.sh clock2.c disc.c elevator.c \
       juggle.c m.c maxwell.c moire.c munch.c nball.c one.c rae.c scope.c \
       spheres.c weird.c win.c

TOOLNAME=dmddemo/sm_demo


include ../../stand.targ

install5620: $(IN5620FILES)

installs630: $(IN630FILES)

$(TCLIB)/xdmddemo/demobin/xclock1: clock1.sh
	sed "s+DeFdMd+$(DEFDMD)+g" clock1.sh >$(TCLIB)/xdmddemo/demobin/xclock1
	chmod $(MODEBIN) $(TCLIB)/xdmddemo/demobin/xclock1

$(TCLIB)/xdmddemo/dmd/demolibsa/xbltdemo:   dmd/bltdemo.j
	cp dmd/bltdemo.j $(TCLIB)/xdmddemo/dmd/demolibsa/xbltdemo
	chmod $(MODELIB) $(TCLIB)/xdmddemo/dmd/demolibsa/xbltdemo

$(TCLIB)/xdmddemo/dmd/demolibsa/xdisc:   dmd/disc.j
	cp dmd/disc.j $(TCLIB)/xdmddemo/dmd/demolibsa/xdisc
	chmod $(MODELIB) $(TCLIB)/xdmddemo/dmd/demolibsa/xdisc

$(TCLIB)/xdmddemo/dmd/demolibsa/xelevator:   dmd/elevator.j
	cp dmd/elevator.j $(TCLIB)/xdmddemo/dmd/demolibsa/xelevator
	chmod $(MODELIB) $(TCLIB)/xdmddemo/dmd/demolibsa/xelevator

$(TCLIB)/xdmddemo/dmd/demolibsa/xmoire:   dmd/moire.j
	cp dmd/moire.j $(TCLIB)/xdmddemo/dmd/demolibsa/xmoire
	chmod $(MODELIB) $(TCLIB)/xdmddemo/dmd/demolibsa/xmoire

$(TCLIB)/xdmddemo/dmd/demolibsa/xmunch:   dmd/munch.j
	cp dmd/munch.j $(TCLIB)/xdmddemo/dmd/demolibsa/xmunch
	chmod $(MODELIB) $(TCLIB)/xdmddemo/dmd/demolibsa/xmunch

$(TCLIB)/xdmddemo/dmd/demolibsa/xspheres:   dmd/spheres.j
	cp dmd/spheres.j $(TCLIB)/xdmddemo/dmd/demolibsa/xspheres
	chmod $(MODELIB) $(TCLIB)/xdmddemo/dmd/demolibsa/xspheres

$(TCLIB)/xdmddemo/dmd/demolibsa/xwin:   dmd/win.j
	cp dmd/win.j $(TCLIB)/xdmddemo/dmd/demolibsa/xwin
	chmod $(MODELIB) $(TCLIB)/xdmddemo/dmd/demolibsa/xwin

$(TCLIB)/xdmddemo/dmd/demolibsa/xone:   dmd/one.j
	cp dmd/one.j $(TCLIB)/xdmddemo/dmd/demolibsa/xone
	chmod $(MODELIB) $(TCLIB)/xdmddemo/dmd/demolibsa/xone

$(TCLIB)/xdmddemo/dmd/demolib/xa:     dmd/a.j
	cp dmd/a.j $(TCLIB)/xdmddemo/dmd/demolib/xa
	chmod $(MODELIB) $(TCLIB)/xdmddemo/dmd/demolib/xa

$(TCLIB)/xdmddemo/dmd/demolib/xb:     dmd/b.j
	cp dmd/b.j $(TCLIB)/xdmddemo/dmd/demolib/xb
	chmod $(MODELIB) $(TCLIB)/xdmddemo/dmd/demolib/xb

$(TCLIB)/xdmddemo/dmd/demolib/xclock1:     dmd/clock1.j
	cp dmd/clock1.j $(TCLIB)/xdmddemo/dmd/demolib/xclock1
	chmod $(MODELIB) $(TCLIB)/xdmddemo/dmd/demolib/xclock1

$(TCLIB)/xdmddemo/dmd/demolib/xclock2:     dmd/clock2.j
	cp dmd/clock2.j $(TCLIB)/xdmddemo/dmd/demolib/xclock2
	chmod $(MODELIB) $(TCLIB)/xdmddemo/dmd/demolib/xclock2

$(TCLIB)/xdmddemo/dmd/demolib/xjuggle:     dmd/juggle.j
	cp dmd/juggle.j $(TCLIB)/xdmddemo/dmd/demolib/xjuggle
	chmod $(MODELIB) $(TCLIB)/xdmddemo/dmd/demolib/xjuggle

$(TCLIB)/xdmddemo/dmd/demolib/xm:     dmd/m.j
	cp dmd/m.j $(TCLIB)/xdmddemo/dmd/demolib/xm
	chmod $(MODELIB) $(TCLIB)/xdmddemo/dmd/demolib/xm

$(TCLIB)/xdmddemo/dmd/demolib/xmaxwell:     dmd/maxwell.j
	cp dmd/maxwell.j $(TCLIB)/xdmddemo/dmd/demolib/xmaxwell
	chmod $(MODELIB) $(TCLIB)/xdmddemo/dmd/demolib/xmaxwell

$(TCLIB)/xdmddemo/dmd/demolib/xnball:     dmd/nball.j
	cp dmd/nball.j $(TCLIB)/xdmddemo/dmd/demolib/xnball
	chmod $(MODELIB) $(TCLIB)/xdmddemo/dmd/demolib/xnball

$(TCLIB)/xdmddemo/dmd/demolib/xrae:     dmd/rae.j
	cp dmd/rae.j $(TCLIB)/xdmddemo/dmd/demolib/xrae
	chmod $(MODELIB) $(TCLIB)/xdmddemo/dmd/demolib/xrae

$(TCLIB)/xdmddemo/dmd/demolib/xscope:     dmd/scope.j
	cp dmd/scope.j $(TCLIB)/xdmddemo/dmd/demolib/xscope
	chmod $(MODELIB) $(TCLIB)/xdmddemo/dmd/demolib/xscope

$(TCLIB)/xdmddemo/dmd/demolib/xweird:     dmd/weird.j
	cp dmd/weird.j $(TCLIB)/xdmddemo/dmd/demolib/xweird
	chmod $(MODELIB) $(TCLIB)/xdmddemo/dmd/demolib/xweird


$(TCLIB)/xdmddemo/630/xbltdemo:     630/bltdemo.m
	cp 630/bltdemo.m $(TCLIB)/xdmddemo/630/xbltdemo
	$(MSTRIP) -m $(TCLIB)/xdmddemo/630/xbltdemo
	chmod $(MODELIB) $(TCLIB)/xdmddemo/630/xbltdemo

$(TCLIB)/xdmddemo/630/xdisc:     630/disc.m
	cp 630/disc.m $(TCLIB)/xdmddemo/630/xdisc
	$(MSTRIP) -m $(TCLIB)/xdmddemo/630/xdisc
	chmod $(MODELIB) $(TCLIB)/xdmddemo/630/xdisc

$(TCLIB)/xdmddemo/630/xelevator:     630/elevator.m
	cp 630/elevator.m $(TCLIB)/xdmddemo/630/xelevator
	$(MSTRIP) -m $(TCLIB)/xdmddemo/630/xelevator
	chmod $(MODELIB) $(TCLIB)/xdmddemo/630/xelevator

$(TCLIB)/xdmddemo/630/xmoire:     630/moire.m
	cp 630/moire.m $(TCLIB)/xdmddemo/630/xmoire
	$(MSTRIP) -m $(TCLIB)/xdmddemo/630/xmoire
	chmod $(MODELIB) $(TCLIB)/xdmddemo/630/xmoire

$(TCLIB)/xdmddemo/630/xmunch:     630/munch.m
	cp 630/munch.m $(TCLIB)/xdmddemo/630/xmunch
	$(MSTRIP) -m $(TCLIB)/xdmddemo/630/xmunch
	chmod $(MODELIB) $(TCLIB)/xdmddemo/630/xmunch

$(TCLIB)/xdmddemo/630/xspheres:     630/spheres.m
	cp 630/spheres.m $(TCLIB)/xdmddemo/630/xspheres
	$(MSTRIP) -m $(TCLIB)/xdmddemo/630/xspheres
	chmod $(MODELIB) $(TCLIB)/xdmddemo/630/xspheres

$(TCLIB)/xdmddemo/630/xwin:     630/win.m
	cp 630/win.m $(TCLIB)/xdmddemo/630/xwin
	$(MSTRIP) -m $(TCLIB)/xdmddemo/630/xwin
	chmod $(MODELIB) $(TCLIB)/xdmddemo/630/xwin

$(TCLIB)/xdmddemo/630/xone:     630/one.m
	cp 630/one.m $(TCLIB)/xdmddemo/630/xone
	$(MSTRIP) -m $(TCLIB)/xdmddemo/630/xone
	chmod $(MODELIB) $(TCLIB)/xdmddemo/630/xone

$(TCLIB)/xdmddemo/630/xa:     630/a.m
	cp 630/a.m $(TCLIB)/xdmddemo/630/xa
	$(MSTRIP) -m $(TCLIB)/xdmddemo/630/xa
	chmod $(MODELIB) $(TCLIB)/xdmddemo/630/xa

$(TCLIB)/xdmddemo/630/xb:     630/b.m
	cp 630/b.m $(TCLIB)/xdmddemo/630/xb
	$(MSTRIP) -m $(TCLIB)/xdmddemo/630/xb
	chmod $(MODELIB) $(TCLIB)/xdmddemo/630/xb


$(TCLIB)/xdmddemo/630/xclock1:     630/clock1.m
	cp 630/clock1.m $(TCLIB)/xdmddemo/630/xclock1
	$(MSTRIP) -m $(TCLIB)/xdmddemo/630/xclock1
	chmod $(MODELIB) $(TCLIB)/xdmddemo/630/xclock1

$(TCLIB)/xdmddemo/630/xclock2:     630/clock2.m
	cp 630/clock2.m $(TCLIB)/xdmddemo/630/xclock2
	$(MSTRIP) -m $(TCLIB)/xdmddemo/630/xclock2
	chmod $(MODELIB) $(TCLIB)/xdmddemo/630/xclock2

$(TCLIB)/xdmddemo/630/xjuggle:     630/juggle.m
	cp 630/juggle.m $(TCLIB)/xdmddemo/630/xjuggle
	$(MSTRIP) -m $(TCLIB)/xdmddemo/630/xjuggle
	chmod $(MODELIB) $(TCLIB)/xdmddemo/630/xjuggle

$(TCLIB)/xdmddemo/630/xm:     630/m.m
	cp 630/m.m $(TCLIB)/xdmddemo/630/xm
	$(MSTRIP) -m $(TCLIB)/xdmddemo/630/xm
	chmod $(MODELIB) $(TCLIB)/xdmddemo/630/xm

$(TCLIB)/xdmddemo/630/xmaxwell:     630/maxwell.m
	cp 630/maxwell.m $(TCLIB)/xdmddemo/630/xmaxwell
	$(MSTRIP) -m $(TCLIB)/xdmddemo/630/xmaxwell
	chmod $(MODELIB) $(TCLIB)/xdmddemo/630/xmaxwell

$(TCLIB)/xdmddemo/630/xnball:     630/nball.m
	cp 630/nball.m $(TCLIB)/xdmddemo/630/xnball
	$(MSTRIP) -m $(TCLIB)/xdmddemo/630/xnball
	chmod $(MODELIB) $(TCLIB)/xdmddemo/630/xnball

$(TCLIB)/xdmddemo/630/xrae:     630/rae.m
	cp 630/rae.m $(TCLIB)/xdmddemo/630/xrae
	$(MSTRIP) -m $(TCLIB)/xdmddemo/630/xrae
	chmod $(MODELIB) $(TCLIB)/xdmddemo/630/xrae

$(TCLIB)/xdmddemo/630/xscope:     630/scope.m
	cp 630/scope.m $(TCLIB)/xdmddemo/630/xscope
	$(MSTRIP) -m $(TCLIB)/xdmddemo/630/xscope
	chmod $(MODELIB) $(TCLIB)/xdmddemo/630/xscope

$(TCLIB)/xdmddemo/630/xweird:     630/weird.m
	cp 630/weird.m $(TCLIB)/xdmddemo/630/xweird
	$(MSTRIP) -m $(TCLIB)/xdmddemo/630/xweird
	chmod $(MODELIB) $(TCLIB)/xdmddemo/630/xweird


630/bltdemo.m:     bltdemo.c
	$(MCC) $(MCFLAGS) bltdemo.c -o bltdemo.m
	mv bltdemo.m 630/bltdemo.m

630/disc.m:     disc.c
	$(MCC) $(MCFLAGS) disc.c -o disc.m
	mv disc.m 630/disc.m

630/elevator.m:     elevator.c
	$(MCC) $(MCFLAGS) elevator.c -o elevator.m
	mv elevator.m 630/elevator.m

630/moire.m:     moire.c
	$(MCC) $(MCFLAGS) -Z 5000 moire.c -o moire.m
	mv moire.m 630/moire.m

630/munch.m:     munch.c
	$(MCC) $(MCFLAGS) munch.c -o munch.m
	mv munch.m 630/munch.m

630/spheres.m:     spheres.c
	$(MCC) $(MCFLAGS) spheres.c -o spheres.m
	mv spheres.m 630/spheres.m

630/win.m:     win.c
	$(MCC) $(MCFLAGS) win.c -o win.m
	mv win.m 630/win.m

630/one.m:     one.c
	$(MCC) $(MCFLAGS) one.c -o one.m
	mv one.m 630/one.m

630/a.m:    a.c
	$(MCC) $(MCFLAGS) a.c -o a.m
	mv a.m 630/a.m

630/b.m:    b.c
	$(MCC) $(MCFLAGS) b.c -o b.m
	mv b.m 630/b.m

630/clock1.m:    clock1.c
	$(MCC) $(MCFLAGS) clock1.c -o clock1.m
	mv clock1.m 630/clock1.m

630/clock2.m:    clock2.c
	$(MCC) $(MCFLAGS) clock2.c -o clock2.m
	mv clock2.m 630/clock2.m

630/juggle.m:    juggle.c
	$(MCC) $(MCFLAGS) juggle.c -o juggle.m
	mv juggle.m 630/juggle.m

630/m.m:    m.c
	$(MCC) $(MCFLAGS) m.c -o m.m
	mv m.m 630/m.m

630/maxwell.m:    maxwell.c
	$(MCC) $(MCFLAGS) maxwell.c -o maxwell.m
	mv maxwell.m 630/maxwell.m

630/nball.m:    nball.c
	$(MCC) $(MCFLAGS) nball.c -o nball.m
	mv nball.m 630/nball.m

630/rae.m:    rae.c
	$(MCC) $(MCFLAGS) rae.c -o rae.m
	mv rae.m 630/rae.m

630/scope.m:    scope.c
	$(MCC) $(MCFLAGS) scope.c -o scope.m
	mv scope.m 630/scope.m

630/weird.m:    weird.c
	$(MCC) $(MCFLAGS) weird.c -o weird.m
	mv weird.m 630/weird.m


dmd/bltdemo.j:     bltdemo.c
	$(MCC) $(MCFLAGS) -J bltdemo.c -o bltdemo.j
	mv bltdemo.j dmd/bltdemo.j

dmd/disc.j:     disc.c
	$(MCC) $(MCFLAGS) -J disc.c -o disc.j
	mv disc.j dmd/disc.j

dmd/elevator.j:     elevator.c
	$(MCC) $(MCFLAGS) -J elevator.c -o elevator.j
	mv elevator.j dmd/elevator.j

dmd/moire.j:     moire.c
	$(MCC) $(MCFLAGS) -J moire.c -o moire.j
	mv moire.j dmd/moire.j

dmd/munch.j:     munch.c
	$(MCC) $(MCFLAGS) -J munch.c -o munch.j
	mv munch.j dmd/munch.j

dmd/spheres.j:     spheres.c
	$(MCC) $(MCFLAGS) -J spheres.c -o spheres.j
	mv spheres.j dmd/spheres.j

dmd/win.j:     win.c
	$(MCC) $(MCFLAGS) -J win.c -o win.j
	mv win.j dmd/win.j

dmd/one.j:     one.c
	$(MCC) $(MCFLAGS) -J one.c -o one.j
	mv one.j dmd/one.j

dmd/a.j:    a.c
	$(MCC) $(MCFLAGS) a.c -o a.j
	mv a.j dmd/a.j

dmd/b.j:    b.c
	$(MCC) $(MCFLAGS) b.c -o b.j
	mv b.j dmd/b.j

dmd/clock1.j:    clock1.c
	$(MCC) $(MCFLAGS) clock1.c -o clock1.j
	mv clock1.j dmd/clock1.j

dmd/clock2.j:    clock2.c
	$(MCC) $(MCFLAGS) clock2.c -o clock2.j
	mv clock2.j dmd/clock2.j

dmd/juggle.j:    juggle.c
	$(MCC) $(MCFLAGS) juggle.c -o juggle.j
	mv juggle.j dmd/juggle.j

dmd/m.j:    m.c
	$(MCC) $(MCFLAGS) m.c -o m.j
	mv m.j dmd/m.j

dmd/maxwell.j:    maxwell.c
	$(MCC) $(MCFLAGS) maxwell.c -o maxwell.j
	mv maxwell.j dmd/maxwell.j

dmd/nball.j:    nball.c
	$(MCC) $(MCFLAGS) nball.c -o nball.j
	mv nball.j dmd/nball.j

dmd/rae.j:    rae.c
	$(MCC) $(MCFLAGS) rae.c -o rae.j
	mv rae.j dmd/rae.j

dmd/scope.j:    scope.c
	$(MCC) $(MCFLAGS) scope.c -o scope.j
	mv scope.j dmd/scope.j

dmd/weird.j:    weird.c
	$(MCC) $(MCFLAGS) weird.c -o weird.j
	mv weird.j dmd/weird.j


