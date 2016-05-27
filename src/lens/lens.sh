#       Copyright (c) 1987 AT&T   
#       All Rights Reserved       

#       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   
#       The copyright notice above does not evidence any     
#       actual or intended publication of such source code.  

# @(#)lens.sh	1.1.1.6	(3/16/88)

LENS=`basename $0`

if [ "$1" = -V ]
then
	echo "$LENS: version $verno.$subno"
	exit 0
fi

if [ -z "$DMDLIB" ]
then
    DMD=${DMD:-DeFdMd}
    if [ -z "$DMD" ]
    then
	echo '$DMD must be set' >&2
	exit 1
    fi
fi
${DMDLD:-dmdld} ${DMDLIB:-$DMD/lib}/$LENS.m $*

