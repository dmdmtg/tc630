# @(#)jf.sh	1.1.1.3 88/02/24 13:44:09

ME=`basename $0`
case $ME in
    x*) JX=xjx;;
    *)  JX=jx;;
esac

if [ "$1" = -V ]; then
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
$JX ${DMDLIB:-$DMD/lib}/$ME.m $*
