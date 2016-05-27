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
# dmdpr [ -t ] [ -u [mach!]user ] [ file ... ]
#
#	dmdpr sends the stdin [of the file(s)] to the printer port of 
#	a [particular] user's 5620 DMD. -t says tee output rather
#	than just redirect.
#

Usage="$0 [ -t ] [ -u user ] [ file ... ]"

TEE=""
RAW=""
UNAME=$LOGNAME
UHOME=$HOME

set -- `getopt tru: "$@"`

if [ $? != 0 ]
then
	echo $USAGE 1>&2
	exit 747
fi
set +e
for i in $*
do
	case $i in
	-t) TEE="T" ; shift ;;
	-r) RAW="T" ; shift ;;
	-u) UNAME=$2 ; shift 2;;
	--) shift; break;;
	esac
	if [ $? != 0 ]
	then
		echo "Parameter error." 1>&2
		echo $USAGE 1>&2
		exit 747
	fi
done

REMOTE=`expr "$UNAME" : '\(.*\)!.*'`
if [ "$REMOTE" != "" ]
then
	UNAME=`expr "$UNAME" : '.*!\(.*\)'`
	uux - "$REMOTE!dmdpr -u $UNAME"
	echo remote print to $REMOTE!$UNAME
	exit
fi

UHOME=`logdir $UNAME` 

# pick one of the following:
# - it should agree in dmdps and dmdpr
PDIR="$UHOME/rje/"
#PDIR="$UHOME/uucp/"
#PDIR="/usr/spool/uucppublic/$UNAME"

if [ -s ${PDIR}.lck ]
then
	echo "Printer in use by "`cat ${PDIR}.lck` 1>&2
	exit 1;
else 
	if [ -s ${PDIR}.prtty ]
	then
		# do it! cat $files to tty specified in ${PDIR}.prtty file
		trap "/bin/rm -f ${PDIR}.lck ; trap 0; exit " 0 1 2 3 15
		echo $LOGNAME > ${PDIR}.lck
		chmod +r ${PDIR}.lck
		if [ "x$TEE" = "xT" ]
		then
			cat $* | tee `cat ${PDIR}.prtty` 
		else
			cat $* > `cat ${PDIR}.prtty`
		fi
		if [ "$RAW" != "T" ]
		then
			echo "\f" > `cat ${PDIR}.prtty`
		fi
	else
		echo "dmdps is not being run by the user $UNAME."
		exit 1;
	fi
fi
