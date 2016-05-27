KEYFILE=$HOME/.dmd.lock
DMDLOCK=`basename $0`

if [ "$1" = "-p" ] 
then
	if [ -z "$DMDLIB" ]
	then
	    DMD=${DMD:-DeFdMd}
	    if [ -z "$DMD" ]
	    then
		echo '$DMD must be set' >&2
		exit 1
	    fi
	    $DMD/lib/makelock
	    exit $?
	else
	    # running under dmdtool
	    $DMDLIB/../$DMDLOCK/makelock
	    exit $?
	fi
fi

KEY=""
if [ -r $KEYFILE ]
then
	KEY=`cat $KEYFILE`
elif [ -f /etc/private -o -f /etc/shadow ]
then
    :
else
    KEY=`sed -n "s/^${LOGNAME}:\([^,:]*\)[,:].*/\1/p" /etc/passwd`
fi
if [ -z "${KEY}" ]
then
    # password file not found
    opts=
    for arg
    do
	case "${arg}" in
	-u)
		echo "$DMDLOCK: error: ${KEYFILE} not readable" >&2
		echo "Run $DMDLOCK -p to create a password file and try again" >&2
		exit 1
		;;
	*)
		opts="${opts} ${arg}"
		;;
	esac
    done
else
	# password ok, just pass all arguments
	opts="$* -U${KEY}"
fi

# add login ID for password standards
export LOGNAME
LOGNAME=${LOGNAME:-`logname`}
opts="$opts -I${LOGNAME}"

if [ -z "$DMDLD" -o -z "$DMDLIB" ]
then
    # not running under dmdtool
    DMD=${DMD:-DeFdMd}
    if [ -z "$DMD" ]
    then
	echo '$DMD must be set' >&2
	exit 1
    fi
    DMDLD=dmdld
    DMDLIB=$DMD/lib
fi

exec $DMDLD $DMDLIB/$DMDLOCK.m ${opts}
