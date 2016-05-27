#       Copyright (c) 1987 AT&T   
#       All Rights Reserved       

#       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   
#       The copyright notice above does not evidence any     
#       actual or intended publication of such source code.  

# @(#)twid.sh	1.1.1.9	(7/23/92)

ME=`basename $0`
case $ME in
    x*) JX=xjx;;
    *)  JX=jx;;
esac

if [ "$1" = -V ]; then
	echo "$ME: version $verno.$subno"
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
$JX ${DMDLIB:-$DMD/lib}/$ME.m $*
