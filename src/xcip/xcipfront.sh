#!/bin/sh
#
# cip/xcip front end shell script
#

TOOLS=${DMDTOOLS:-${TC630:-$TOOLS}}

CIP=`basename $0`
case $CIP in
    x*) JPIC=xjpic;;
    *)  JPIC=jpic;;
esac

# Parse parameters:

VERSION=""
USEX=""
until
    test ${#} -eq 0
do
    case ${1} in
    -v|-V)  # Print out version.
	VERSION="-V"
        ;;
    -t*) # Explicitly specify type.
	if [ "${1}" = "-t" ]
	then
	    shift
	    TERM_TYPE=${1}
	else
	    TERM_TYPE=`echo ${1} | sed -e "s/^-t//"`
	fi
	case ${TERM_TYPE} in
	x|sun|sparc|x11|X|X11) USEX="YES";;
	*)  USEX="NO";;
	esac
	;;
    *)  # Get rest of parameters:
	PARMS="$PARMS ${1}"
	;;
    esac
    shift
done

# Check if $JPIC exists on PATH.
for dir in `echo ${PATH}|sed 's/:/ /g'`
do
	if [ -f ${dir}/$JPIC ]
	then
		break;
	fi
done

if [ ! -f ${dir}/$JPIC ]
then
	echo "$CIP: The host program $JPIC is not present." >&2
	exit 1
fi


# See if we should use X
XCIP_OBJ=${TOOLS}/lib/x/$CIP
if [ "$USEX" = "" -a -x "$XCIP_OBJ" ]
then
    case $TERM in
    xterm|sun-cmd|x11)
        USEX="YES"
	break
	;;
    *)  if [ "$DISPLAY" != "" ]
	then
            USEX="YES"
	fi
    	;;
    esac
fi

if [ "$USEX" = "YES" ]
then

    if [ "$VERSION" != "" ]
    then
        # Printout versions of cip and jpic.
        $XCIP_OBJ -V
        echo "" | $JPIC -V
    else
        # Invoke X-window cip:
        eval $XCIP_OBJ $PARMS
    fi

    exit 0
fi

#
# Invoke dmd portion
#
exec ${TOOLS}/lib/dmdtools/dmdtool $CIP $VERSION
