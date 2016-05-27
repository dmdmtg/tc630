#!/bin/sh

#		C I P R I N T
#
# Print out pic files from cip or xcip.
# Supports printers: Xerox 9700, Imagin i300 and Postscript.
# Uses old and new troff, pic, dx9700 and prt.

VERSION="x5.0"

FONTSIZE="TX97.ti10p"	# Supports font sizes 7 & 10.
PRINTER_TYPE=xr		# Printer default - override with -t option.
PO="0.4"		# Position offset - override with -o option.

VERS_FLAG=0

# Determine version of Documenter's WorkBench (DWB).
if [ "${DWB}" = "" ]
then
    DWBV=`where dwbv`
    if [ "$DWBV"  = "" ]
    then
	if
		test -d /usr/add-on/dwb2.0a
	then
		DWB="2"
	else
		DWB="1"
	fi
    else
	DWB=`dwbv | cut -f2 -d" "`
    fi
fi

args=""
files=""

until
	test ${#} -eq 0
do
	case ${1} in
	-\?)
		echo "usage: $0 [-V?] [-o offset] [-t xr|i300 ] [-f12] [printer_args] [files]"
		exit 0 
		;;
	-V|-v)
		VERS_FLAG=1
		echo "$0 version ${VERSION}"
		echo ""
		;;
	-o)
		shift
		PO="${1}"
		;;
	-o*)
		PO=`echo $1 |sed -e "s/^-o//"`
		;;
	-t|-T)
		shift
		PRINTER_TYPE="${1}"
		;;
	-t*|-T*)
		PRINTER_TYPE=`echo $1 |sed -e "s/^-.//"`
		;;
	-[f]12)
		FONTSIZE="TX97.ti12p"		# Supports font sizes 9 & 12.
		;;
	-[f]10)
		FONTSIZE="TX97.ti10p"		# Supports font sizes 7 & 10.
		;;
	-*)
		args="${args} ${1}"
		;;
	+*)
		args="${args} ${1}"
		;;
	*)
		files="${files} ${1}"
		;;
	esac
	shift
done

case ${PRINTER_TYPE} in

xr)
	# Form command line.  The grep at end removes from standard error 
	# numerous meaningless number messages from dx9700.

	case ${DWB} in
	1*)	
		PROCESS_LINE="pic -TX97 - 2>/dev/null | \
		troff -mm -${FONTSIZE} - 2>/dev/null | \
		( ndx9700 | opr -txr -r ${args} ) 2>&1 | grep -v '^([0-9]' >&2"
		;;
	2*)
		PROCESS_LINE="pic -D - 2>/dev/null | \
		troff -mm -${FONTSIZE} - 2>/dev/null | \
		( dx9700 | opr -txr -r ${args} ) 2>&1 | grep -v '^([0-9]' >&2"
		;;
	*)
		PROCESS_LINE="pic - 2>/dev/null | \
		troff -mm -${FONTSIZE} - 2>/dev/null | \
		( dx9700 | opr -txr -r ${args} ) 2>&1 | grep -v '^([0-9]' >&2"
		;;
	esac;;

i300)
	if [ "${IDEST}" = "" ]
	then
	    echo "Warning: the environment variable IDEST is not set to an i300 destination."
	    echo ""
	fi
	case ${DWB} in
	1*)
		PROCESS_LINE="pic -T300 - | troff -mm -Ti300 - | i300 ${args}"
		;;
	2*)
		PROCESS_LINE="pic -D - | troff -mm -Ti300 - | i300 ${args}"
		;;
	*)
		PROCESS_LINE="pic - | troff -mm -Ti300 - | i300 ${args}"
		;;
	esac;;

prt)
	PROCESS_LINE="pic -Tpost - | troff -mm -Tpost | prt -l troff ${args}"
	;;

*)
	echo "$0: ERROR: Printer type [-t ${PRINTER_TYPE}] is invalid" >&2
	exit 1
	;;
esac

FIRST=1		# flag for first file.

(
	echo ".lg 0"		# turn off ligation; eg. "ff"
	echo ".rm )k"		# turn off tick marks at top of page
	echo ".PH"		# turn off page headers

	for i in ${files} 
	do
		if [ $FIRST = 1 ] ;
		then
			FIRST=0
		else
			echo ".bp"	# break to next page
		fi
    		echo ".po ${PO}i"	# position over inches
		echo ".DS CB"		# center picture
		cat $i		
		echo ".DE"
	done
) |

if [ $VERS_FLAG = 1 ]
then
	echo ${PROCESS_LINE}
else
	eval ${PROCESS_LINE}
fi
