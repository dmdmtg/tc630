#       Copyright (c) 1987 AT&T
#         All Rights Reserved

#       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#       The copyright notice above does not evidence any
#       actual or intended publication of such source code.

# @(#)makefile	1.1.1.16	(6/20/90)

include ../stand.defs

CFLAGS=-I$(DMDSYS)/include -I../inc $(EXTRACFLAGS)
TC630=../..
TOOLNAME=sysmon
OBJSHOST=sysmon.o ver.o
OBJS5620=$(OBJSHOST)
OBJS630=$(OBJSHOST)

PRODUCTSHOST=sysmon
PRODUCTS5620=$(PRODUCTSHOST) $(TCTERM)/sysmon.m 1.xsysmon
PRODUCTS630=$(PRODUCTS5620)
INSTALLHOST=$(TCLIB)/dmdtools/xsysmon
INSTALL5620=$(INSTALLHOST) $(TCBIN)/xsysmon \
	$(TCLIB)/$(TCTERM)/xsysmon.m $(TCMAN)/1.xsysmon
INSTALL630=$(INSTALL5620)
SOURCE=README jerqmon.c sysmon.c xsysmon.1 ver.c


include ../stand.targ

$(TCLIB)/dmdtools/xsysmon:	sysmon
	cp sysmon $(TCLIB)/dmdtools/xsysmon
	@echo NOTE: Change the ownership of $(TCLIB)/dmdtools/xsysmon to root
	@echo NOTE: Change the mode of $(TCLIB)/dmdtools/xsysmon to 4$(MODEBIN)

$(TCLIB)/630/xsysmon.m:	630/sysmon.m
	cp 630/sysmon.m $(TCLIB)/630/xsysmon.m
	$(MSTRIP) -m $(TCLIB)/630/xsysmon.m
	chmod $(MODELIB) $(TCLIB)/630/xsysmon.m

$(TCLIB)/dmd/xsysmon.m:	dmd/sysmon.m
	cp dmd/sysmon.m $(TCLIB)/dmd/xsysmon.m
	chmod $(MODELIB) $(TCLIB)/dmd/xsysmon.m

$(TCBIN)/xsysmon:	../startup.sh
	cp ../startup.sh $(TCBIN)/xsysmon
	chmod $(MODEBIN) $(TCBIN)/xsysmon

$(TCMAN)/1.xsysmon:	1.xsysmon
	cp 1.xsysmon $(TCMAN)/1.xsysmon
	chmod $(MODELIB) $(TCMAN)/1.xsysmon

sysmon: sysmon.c ver.o
	@if [ -n "$(DEFDMD)" ];then D=-DDEFDMD=\"$(DEFDMD)\";else D=;fi;\
	if grep p_region /usr/include/sys/proc.h > /dev/null || { [ -x /bin/uts ] && uts; }; then \
		echo paging system; \
		set -x; $(CC) -DPAGING -o sysmon sysmon.c ver.o $(CFLAGS) $$D; \
	else \
		echo swapping system; \
		set -x; $(CC) -o sysmon sysmon.c ver.o $(CFLAGS) $$D; \
	fi



$(TCTERM)/sysmon.m:	jerqmon.c
	$(MCC) $(MCFLAGS) jerqmon.c -o $(TCTERM)/sysmon.m

1.xsysmon: xsysmon.1
	nroff -man -T37 $(MANFL) xsysmon.1 > 1.xsysmon

sysmon.o : sysmon.c ../inc/quiet.h
