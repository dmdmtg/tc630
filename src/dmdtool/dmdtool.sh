# 
# dmdtool -- common front end for tools which load to both 5620 & 630/730
#  Determines whether on a 5620 or 630/730
#  After doing so, sets these variables
#		5620		630/730
#		====		========
#    DMDTERM	dmd		  630
#    DMDLIB	$TOOLS/lib/dmd	  $TOOLS/lib/630
#    DMDLD	$DMD/bin/32ld	  $TOOLS/bin/xdmdld or $DMD/bin/dmdld
#
#  Also sets JPATH=$DMDLIB:$JPATH in both cases.
#
#  Finally, execs $TOOLS/lib/dmdtools/$1 if it exists.  If it doesn't
#	and $DMDLIB/$1.m exists, runs $DMDLD on that instead.
#
# Version 8/26/93
#

# DMDTOOLS overrides TC630 which overrides TOOLS
TOOLS=${DMDTOOLS:-${TC630:-${TOOLS:?"must be set (or TC630 or DMDTOOLS)"}}}
case $# in
    0) echo "Usage: dmdtool tool_name [args]" >&2 ; exit 1;;
esac
PROGNAME=$1
shift

KIND=""
DMD=${DMD:-DeFdMd}
case "$TERM" in
    *630*|*730*) KIND=630 ;;
    dmd|*5620*) KIND=dmd ;;
    *) # try getting TERM another way
	if [ -x $TOOLS/bin/ttype ]
	then
	    # Need to re-open stdin in case on end of a pipeline.
	    case `$TOOLS/bin/ttype -x -Tdmd </dev/tty` in
	        *630*|*730*) KIND=630 ;;
	        dmd|*5620*) KIND=dmd ;;
	    esac
	fi
	if [ -z "$KIND" -a -x $TOOLS/bin/agent ]
	then
	    # Agent doesn't read from its stdin so it's perfectly safe
	    #  to dup it from stderr, even if stderr were open O_WRONLY.
	    # Agent does need to have stdin open so it can do a ttyname()
	    #  on it.  A ttyname on /dev/tty returns '/dev/tty' instead
	    #  of '/dev/xt*' on some machines so need to dup from stderr
	    #  instead of opening from /dev/tty.
	    # Ttype is different because it does the ttyname() on stderr.
	    #  Dmdversion uses stdin.
	    case `$TOOLS/bin/agent ROMVERSION <&2 2>/dev/null` in
	        ?\;8\;?) KIND=630 ;;
	        ?\;7\;?) KIND=dmd ;;
	    esac
	fi
	if [ -z "$KIND" -a -x $DMD/bin/dmdversion ]
	then
	    case `$DMD/bin/dmdversion -t <&2 2>/dev/null| 
		    sed -e 's/.* 8;//' -e 's/;.*//'` in
	        8) KIND=630 ;;
	        7) KIND=dmd ;;
	    esac
	fi ;;
esac

case $KIND in
    630) DMDLD=$TOOLS/bin/xdmdld
	 if [ ! -x "$DMDLD" ]
	 then
	    DMDLD=$DMD/bin/dmdld
	 fi
	 DMDLIB=$TOOLS/lib/630
	 DMDTERM=630
	 ;;
    dmd) DMDLD=$DMD/bin/32ld
         DMDLIB=$TOOLS/lib/dmd
	 DMDTERM=dmd
	 ;;
    *) echo "$PROGNAME: dmdtool cannot determine terminal type" >&2 ; exit 1;;
esac

JPATH=$DMDLIB:$JPATH

export DMDLD DMDLIB DMDTERM JPATH TOOLS

if [ -f $TOOLS/lib/dmdtools/$PROGNAME ]
then
    exec $TOOLS/lib/dmdtools/$PROGNAME ${1+"$@"}
elif [ -f $DMDLIB/$PROGNAME.m ]
then
    exec $DMDLD $DMDLIB/$PROGNAME.m ${1+"$@"}
else
    echo "dmdtool: \$TOOLS/lib/dmdtools/$PROGNAME not found" >&2
    exit 2
fi
