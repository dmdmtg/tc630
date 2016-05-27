#       Copyright (c) 1987 AT&T   
#       All Rights Reserved       

#       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   
#       The copyright notice above does not evidence any     
#       actual or intended publication of such source code.  

# @(#)xdmddemo.sh	1.1.1.6	(3/17/88)


ME=`basename $0`

if [ -n "$DMDLIB" ]
then
    # run from under dmdtool
    DMDLIB=`dirname $DMDLIB`/$ME

    DMDLIBBIN=$DMDLIB/demobin

    if [ "$DMDTERM" = dmd ]
    then
	if ismpx -
	then
	    DMDLIB=$DMDLIB/dmd/demolib
	else
	    DMDLIB=$DMDLIB/dmd/demolibsa
	fi
    else
	DMDLIB=$DMDLIB/630
    fi
else
    # not run from under dmdtool
    DMD=${DMD:-DeFdMd}
    if [ -z "$DMD" ]
    then
	echo '$DMD must be set' >&2
	exit 1
    fi
    DMDLIBBIN=$DMD/lib/demobin
    DMDLIB=$DMD/lib/demolib
    DMDLD=${DMDLD:-dmdld}
    export DMDLD
fi

export DMDLIB DMDLIBBIN

if [ "$1" = "" ]
then
	echo Available demos are:
	cd $DMDLIB
	ls | pr -t -6 -l1
else
	if test -s $DMDLIBBIN/$1
	then
		$DMDLIBBIN/$1
	else
		if test -s $DMDLIB/$1
		then
			$DMDLD $DMDLIB/$1
		
		else
			if test $1 = -V
			then
				echo $ME: version $verno.$subno
				exit 0
			fi
			echo Demo \`$1\' does not exist
			echo Type \"$ME\" for a list of demos
		fi
	fi
fi

