#       Copyright (c) 1987 AT&T   
#       All Rights Reserved       

#       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   
#       The copyright notice above does not evidence any     
#       actual or intended publication of such source code.  

# @(#)life.sh	1.1.1.1	(5/26/88)

case `basename $0` in
    x*) X=x;;
    *) X=;;
esac
if ismpx -
then
    DMDLD=${DMDLD:-${DMD:-DeFdMd}/bin/dmdld}
    DMDLIB=${DMDLIB:-${DMD:-DeFdMd}/lib/demolib}
    DMDLIBBIN=${DMDLIBBIN:-${DMD:-DeFdMd}/lib/demobin}
    $DMDLD $DMDLIB/${X}life && $DMDLIBBIN/${X}hlife
else
    echo "${X}life only runs in layers"
fi
