# Make a C array from the text of an mc68 executable
#
# $1 is path to mc68dis
# $2 is path to mc68 executable
# $3 is name of array
# An integer with the size of the array Size$3 is also created
#
# Dave Dykstra, 8/4/93
#
$1 $2|(echo "unsigned short $3[] = {";\
    sed -n '/.*:/{s/.*:  /0x/;s/ /,0x/g;s/,0x,.*/,/p;}';\
    echo "};"; \
    echo "int Size$3 = sizeof($3);" )
