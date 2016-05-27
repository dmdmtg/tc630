#       Copyright (c) 1987 AT&T   
#       All Rights Reserved       

#       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   
#       The copyright notice above does not evidence any     
#       actual or intended publication of such source code.  

# @(#)hp2621.sh	1.1.1.5	(11/4/87)

ME=`basename $0`
if [ $# -gt 2 ] ; then
  echo usage: $ME [-V] [startup [firsttime]]
  exit 1
fi

if [ "$1" = -V ]
then
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
${DMDLD:-dmdld} ${DMDLIB:-$DMD/lib}/$ME.m "$@"
