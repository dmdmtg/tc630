
outfile=cart
cid=0630
applfile=
fontfile=
romafile=
bssaddress=8126456	# 0x7bfff8

set -- `getopt o:c:a:f:r:b: $*`
if [ $? != 0 ]
then
	echo "usage: [-o outfile] [-a ramappl_file] [-r romappl_file] [-f font_file] [-b bss_address]"
	exit 1
fi

for i in $*
do
	case $i in
	-o) outfile=$2; shift 2;;
	-c) cid=$2; shift 2;;
	-a) applfile=$2; shift 2;;
	-f) fontfile=$2; shift 2;;
	-r) romafile=$2; shift 2;;
	-b) bssaddress=$2; shift 2;;
	--) shift; break;;
	esac
done

tmplist=$$cartlist
trap "exit 1" 1 2 3 15
trap "rm -f $tmplist;" 0


if [ -n "$fontfile" ]
then
	if [ ! -f $fontfile ]
	then
		echo "735cart: $fontfile not found"
		exit 1
	fi
	echo "processing fonts"
	cartfont < $fontfile >> $$cartlist
	if [ $? != 0 ]
	then
		echo "735cart: error"
		exit 1
	fi
fi

if [ -n "$applfile" ]
then
	if [ ! -f $applfile ]
	then
		echo "735cart: $ramapplfile not found"
		exit 1
	fi
	while read line
	do
		echo "processing: $line"
		eval execram $line >> $$cartlist
		if [ $? != 0 ]
		then
			echo "735cart: error"
			exit 1
		fi
	done < $applfile
fi

if [ -n "$romafile" ]
then
	if [ ! -f $romafile ]
	then
		echo "735cart: $romapplfile not found"
		exit 1
	fi
	while read line
	do
		echo "processing: $line"
		eval execrom $line >> $$cartlist
		if [ $? != 0 ]
		then
			echo "735cart: error"
			exit 1
		fi
	done < $romafile
fi

if [ ! -s $$cartlist ]
then
	echo "735cart: no files to be put into the cartridge"
	exit 1
fi

make_cart.735 -o $outfile -c $cid -b $bssaddress < $$cartlist

