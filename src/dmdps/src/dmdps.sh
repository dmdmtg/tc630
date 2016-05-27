# 
#									
#	Copyright (c) 1987,1988,1989,1990,1991,1992   AT&T		
#			All Rights Reserved				
#									
#	  THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T.		
#	    The copyright notice above does not evidence any		
#	   actual or intended publication of such source code.		
#									
# 
#
# dmdps.sh
#
# author: A. T. Schnable
# 
# 2/24/1988 : move pfds to directory ($TOOLS/lib/dmdps).
# 2/24/1988 : get rid of dmdp references
# 1/13/1986 : add descriptive comments.
# 1/13/1986 : now prints error msg to stderr and exits on error.
# 7/1/1986  : fully effected dmdp to dmdps name change.

# check required variables
: ${TOOLS:?}
: ${DMD:?}

# Note, this had better match the similar line in dmdpr
# this should be a writable directory to everyone you want
# to share the printer with.
PDIR="$HOME/rje/"
PFDDIR=${TC630:-$TOOLS}/lib/dmdps/pfd

CMD=`basename $0`

# the -c option must be last. sorry. getopt won't return the string in one piece.
USAGE="Usage: $CMD"' [-P | [-7 | -8] [-9600 | -4800 | -1200]] [-C] [-f bitmap_file] [-T printer_type ] [-c pipe_command]'
FILENAME="BLITMAP"
ZFLAG=""
CFLAG=""

case $CMD in 
dmdpp)
	PAR=t;;
dmdps)
	PAR=f;;
esac

set -- `getopt 781:9:4:c:f:p:T:PCz "$@"`
set +e
for i in $*
do 
	case $i in
	-7 | -8) BITS=$1; shift ;;
	-4 ) 	if [ $2 = "800" ]
		then
			SPEED="-4800"
		else
			echo "bad speed specification." 2>&1
			exit 1
		fi
		shift 2;;
	 -1 )	if [ $2 = "200" ] 
		then
			SPEED="-1200"
		else
			echo "bad speed specification." 2>&1
			exit 1
		fi
		shift 2;; 
	 -9 )	if [ $2 = "600" ] 
		then
			SPEED="-9600"
		else
			echo "bad speed specification." 2>&1
			exit 1
		fi
		shift 2;; 
	-p)	echo "The -p option has been phased out." 2>&1
		echo "Use the -T option instead" 2>&1
		exit 1;;
	-T)	PTYPE=$2 ; shift 2;;
	-P)	PAR=t ; shift 1;;
	-C)	CFLAG="-C" ; shift 1;;
	-z)	ZFLAG="-z" ; shift 1;;
	-c)	shift ; PCMD=$1; shift; for j in $*
		do
			if [ $j = -- ]
			then
				break
			else
				PCMD="$PCMD $j"
				shift 
			fi
		done;;
	-f)	FILENAME="$2" ; shift 2;;
	--)	shift ; break ;
	esac
	if [ $? != 0 ]
	then
		echo $USAGE 1>&2
		exit 767
	fi
done

case $PTYPE in
/* )
	PFDFILE=$PTYPE
	SPEED=${SPEED-"-4800"} 
	BITS=${BITS-"-8"} ;;

*)	if [ -f "$PFDDIR/$PTYPE.$BITS" ]
	then
		PFDFILE="$PFDDIR/$PTYPE.$BITS"
	else	if [ -f $PFDDIR/$PTYPE ]
		then
			PFDFILE="$PFDDIR/$PTYPE"
		else
			cat 1>&2 <<!!
The printer type $PTYPE is unknown to $0. Try one of: 

`ls $PFDDIR | egrep -v ".7$" | pr -t -4`

!!
			exit 2
		fi
	fi ;;
esac

if [ "$PAR" = 't' ]
then
	LOADM=dmdpp.m
else
	LOADM=dmdps.m
fi

# figure out which downloader to use...
case $TERM in
*630*) 	;;
*) 	TERM=`ttype -Tdmd` ;;
esac

case $TERM in
*630*) 	JPATH=$JPATH:${DMDLIB:+$DMDLIB:}${TC630:-$TOOLS}/lib/630 ;;
*) 	JPATH=$JPATH:${DMDLIB:+$DMDLIB:}${TC630:-$TOOLS}/lib/dmd ;;
esac
export JPATH

# generate full path for dmdpi to find later...
LOADM=`where -fp $JPATH x$LOADM`

if ismpx -
then
	# the PDIR.prtty file contains tty of active dmdps process.
	# we must remove it on exit - dmdpr looks for this file.
	trap '/bin/rm -f ${PDIR}.lck ${PDIR}.prtty; trap 0; exit' 0 1 2 3 15
	TTY=`tty`
	echo $TTY > ${PDIR}.prtty
	chmod +r ${PDIR}.prtty
	chmod +w $TTY

	# dmdpr will create this lock file - remove on a restart of dmdps
	# just in case dmdpr didn't get a chance to (maybe a machine crash...)
	/bin/rm -f ${PDIR}.lck

	CMD="jx $ZFLAG ${LOADM} ${CFLAG} ${SPEED} ${BITS}"

	if [ -r "${PFDFILE}" ]
	then
		CMD="$CMD -p ${PFDFILE}"
	fi
	if [ -z "${FILENAME}" ]
	then
		CMD="$CMD -f ${FILENAME}"
	fi
	if [ "${PCMD}" ]
	then
		CMD="$CMD -c \"${PCMD}\""
	fi
	
	eval "$CMD"
else
	echo `basename $0` only runs under mpx
fi
