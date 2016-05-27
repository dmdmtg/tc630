#       Copyright (c) 1987 AT&T   
#       All Rights Reserved       

#       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   
#       The copyright notice above does not evidence any     
#       actual or intended publication of such source code.  

# @(#)makefile	1.1.2.1	(5/26/88)

include ../../../../stand.defs

PRODUCTSHOST=hlife
PRODUCTS5620=$(PRODUCTSHOST)
PRODUCTS630=$(PRODUCTSHOST)

INSTALLHOST=$(TCLIB)/xdmddemo/demobin/xhlife
INSTALL5620=$(TCLIB)/xdmddemo/demobin/xlife $(INSTALLHOST)
INSTALL630=$(TCLIB)/xdmddemo/demobin/xlife $(INSTALLHOST)

TC630=../../../../..
TOOLNAME=dmddemo/lg_demo/life/host

SOURCE= hlife.c life.sh makefile

include ../../../../stand.targ

$(TCLIB)/xdmddemo/demobin/xhlife: 	hlife
	cp hlife $@
	chmod $(MODEBIN) $@

$(TCLIB)/xdmddemo/demobin/xlife:	life.sh
	sed "s+DeFdMd+$(DEFDMD)+g" life.sh >$@
	chmod $(MODEBIN) $@

hlife:	hlife.c
	$(CC) hlife.c -o hlife $(CFLAGS)

