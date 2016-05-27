#       Copyright (c) 1987 AT&T   
#       All Rights Reserved       

#       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   
#       The copyright notice above does not evidence any     
#       actual or intended publication of such source code.  

# @(#)clock1.sh	1.1.1.4	(11/4/87)

ME=`basename $0`
echo "type 'q' to exit"
if ismpx -
then
    ${DMDLD:-dmdld} ${DMDLIB:-${DMD:-DeFdMd}/lib/demolib}/$ME "`date`"
else
    echo "$ME only works in layers"
fi
