# ***************************************************************
# *	Copyright (c) 1987 AT&T 				*
# *     All Rights Reserved				        *
# *	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*
# *	The copyright notice above does not			*
# *	evidence any actual or intended				*
# *	publication of such source code.			*
# ***************************************************************
# @(#)makefile	1.1.1.32	(6/6/88)

########
# All descendent makefiles support the following standard targets
#
#	build		builds all the products specified by PRODUCTS.
#			This is the default for all makefiles.
#	install		installs products into bin, man, and lib directories
#	clean		removes all temporary files (ex. installable object)
#	clobber		"cleans", and then removes $(CLOBBER)
#
# Also, there are "host" equivalents (e.g. buildhost) for each of the above four
#  targets which apply to only the host-compiled binaries.  These are useful for
#  those who already have the downloadable files and shell scripts from another
#  host computer and only want to recompile the host sides (perhaps because they
#  don't have the 5620 or 630 software development packages on the new host).
########

# Be sure to see the comments in stand.defs

include stand.defs

TC630=..

TOOLPRODUCTS = \
	32ld \
	dmdtool \
	dmddemo \
	dmdld \
	dmdps \
	hp2621 \
	jf \
	jx \
	lens \
	proof \
	tdmd \
	tek4014 \
	twid \
	ucache \
	xcip \
	630cart \
	zap630

#######
# Default pieces to build for a remote exptools make
#   -- host sides belonging to the xdmdtools package
#######

HOST_SIDES=	dmddemo \
		dmdld \
		jx \
		proof \
		tdmd \
		zap630

# extra files/directories to put into package to send out with remote make
EXTRA_STUFF=	makefile inc startup.sh stand.defs stand.targ


########
#
#	Rule definitions
#
########

all:	copyright
all:	install

copyright:
	@echo "\n\n**********************************************"
	@echo "*         Copyright (c) 1987 AT&T            *"
	@echo "*           All Rights Reserved              *"
	@echo "**********************************************\n\n"

build install clean clobber buildhost installhost cleanhost clobberhost:
	$(MAKE) $(MACROS) ACTION=$@ $(TOOLPRODUCTS)

build install buildhost installhost: mkdirs

mkdirs:
	mkdir $(TC630)/bin $(TC630)/lib $(TC630)/man 2>/dev/null || true
	mkdir $(TC630)/lib/630 $(TC630)/lib/dmd 2>/dev/null || true
	mkdir $(TC630)/lib/dmdtools 2>/dev/null || true
	mkdir $(TC630)/lib/dmdps $(TC630)/lib/dmdps/pfd 2>/dev/null || true
	mkdir $(TC630)/lib/xdmddemo 2>/dev/null || true
	mkdir $(TC630)/lib/xdmddemo/demobin 2>/dev/null || true
	mkdir $(TC630)/lib/xdmddemo/630 2>/dev/null || true
	mkdir $(TC630)/lib/xdmddemo/dmd 2>/dev/null || true
	mkdir $(TC630)/lib/xdmddemo/dmd/demolib 2>/dev/null || true
	mkdir $(TC630)/lib/xdmddemo/dmd/demolibsa 2>/dev/null || true
	mkdir $(TC630)/lib/xdmdlock 2>/dev/null || true

clobber clobberhost: clobberdirs

clobberdirs:
# Don't want to rm -rf lib because there might be binaries we want
#  to keep under lib.  Go ahead and try to rmdir it though so that
#  if the directory is empty after we're done it will disappear.
	if [ "$(TC630)" = .. ]; then \
	    cd $(TC630);rm -rf bin man lib/630 lib/dmd lib/dmdtools lib/dmdps \
		lib/xdmddemo lib/xdmdlock; \
	    rmdir lib 2>/dev/null || true; \
	fi

########
#
#       Targets for making descendant directories
#
########

$(TOOLPRODUCTS):	neverExisting
	@echo "\nmaking $(ACTION) for $@"; \
	cd $@; $(MAKE) $(MACROS) $(ACTION)

# This step forces all TOOLPRODUCTS to be made, despite their date
neverExisting:

########
#
#       Targets for exptools assists
#
########

#
# Rule for making cpio file to send away to rmake.
#
rmake: rmake.cpio.Z

rmake.cpio.Z : rmake.cpio
	rm -f rmake.cpio.Z
	compress rmake.cpio

rmake.cpio: neverExisting
	@set -x; for f in $(HOST_SIDES); do \
		( \
		cd $$f; \
		pwd; \
		$(MAKE) $(MACROS) clobber \
		) \
	done
	find $(EXTRA_STUFF) $(HOST_SIDES) -type f ! -name "*.1" -print | \
				cpio -ocv > $@

clobber: rmrmake

rmrmake:
	rm -f rmake.cpio*

#
# Rmake target for making only portions of the EXPTOOLS distribution and
#  sending back the results.
#
# HOST_SIDES may be supplied on the command line
# For example,
#    rmake -pxdmdtools -P$HOME/.secret/.xdmdtools -srmake.cpio.Z \
#		-ths.cpio -mi386 HOST_SIDES="\"jx proof\""
#
#
# When hs.cpio is returned by the EXPTOOLS remote "representative
# machines", its name will be "hs.cpio.<machine>".
#
hs.cpio:
	@set -x; TC630=`pwd`; \
	    $(MAKE) TC630=$$TC630 mkdirs; \
	    for f in $(HOST_SIDES); do \
	    ( \
		cd $$f; \
		pwd; \
		$(MAKE) TC630=$$TC630 EXTRAMACROS='TC630=$$(TC630)' \
				DMDSYS=$(TOOLS)/lib/agent installhost; \
	    ) \
	done
	find bin lib -print | cpio -ocv > $@
	rm -rf bin lib man

