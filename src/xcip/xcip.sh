#
# dmd/630  cip/xcip script
#

CIP=`basename $0`
case $CIP in
    x*) JPIC=xjpic; JX=xjx;;
    *)  JPIC=jpic; JX=jx;;
esac

if ismpx -
then
    :
else
    echo "$CIP not available in standalone mode - use layers" >&2
    exit 1
fi

DMD=${DMD:-DeFdMd}
if [ "$DMD" = "" ]
then
    echo $CIP: 'environment variable $DMD must be set' >&2
    exit 1
fi

dmdobj=${DMDLIB:-$DMD/lib}/$CIP.m

# Print out version if requested.  (Note: this does not work on VAXes or
#   '386es because its byte ordering is reversed.)
if [ "$1" = "-V" ]
then
	sed -n -e "/ version /p" ${dmdobj} 2>/dev/null | \
	sed -n -e "s/.*\( version [0-9a-zA-Z]*\.[0-9a-zA-Z]*\).*/$CIP \1/p"
	echo "" | $JPIC -V
	exit 0
fi

# Test if download is attempted in the window that will receive system
# and other messages.  These messages can hang up reads & writes.
# This test does not work on old pty layers.  Just set "mesg n" to try to
# prevent messages from getting in.
if [ "$DMDAGENT" != "" -a "$DMDAGENTDIR" = "" ]
then
    mesg n
else
    MYTTY=`tty <&2|sed 's-/dev/--'`
    if [ "`who|awk '{if ($2 == \"'$MYTTY'\") print $2}'`" != "" ]
    then
    	echo "$CIP should NOT be downloaded in this window as this window" >&2
    	echo "   will be the one receiving system and other messages." >&2
    	echo "   Create another window and run $CIP in it." >&2
    	exit 1
    fi
fi


# Determine font directory.
fontpath=${DMD}/font:/usr/add-on/dmd2.0/font
if [ "${DMDLIB}" != "" ]
then
    fontpath=${DMDLIB}/../xfont:$fontpath
fi
if [ "${FONT}" != "" ]
then
	# Add user supplied font directory to FONT path.
	fontpath=${FONT}:${fontpath}
fi

for fontdir in `echo ${fontpath}|sed 's/:/ /g'`
do
	if [ -f ${fontdir}/R.10 ]
	then
		break;
	fi
done

if [ ! -f ${fontdir}/R.10 ]
then
	echo "$CIP: a DMD/MTG font directory not found - set FONT to font" >&2
	echo "		directory and export FONT." >&2
	exit 1
fi

# Finally... invoke jx to download $CIP.m!

$JX ${dmdobj} ${fontdir}
