#       Copyright (c) 1987 AT&T   
#       All Rights Reserved       

#       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   
#       The copyright notice above does not evidence any      
#       actual or intended publication of such source code.  

# @(#)tek4014.sh	1.1.1.4	(10/23/87)

ME=`basename $0`
USAGE="Usage: $ME -cdeglruV"
# parse command line options
set -- `getopt cdeglruV $*`
# if the return value of getopt is not zero then
# echo a usage message.
if [ $? != 0 ]
then
        echo $USAGE
        exit 2
fi
for i in $*
do
        case $i in
        -c|-d|-e|-g|-l|-r|-u)     ;;
        -V)     echo "$ME: version $verno.$subno";exit 0;;
        --)     break;;
        esac
done

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
