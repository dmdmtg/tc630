# initialize variables and flags
BSS_FUDGE=0
TEXT_FUDGE=0
ZERO=0
EXEC_RAM=1
EXEC_ROM=2
FONTFILE=3


MLD=$DMD/bin/mc68ld

obj_num=0
obj_name=""
src_name=""
sys_init_nm=""
usr_init_nm=""
text_size=0
data_size=0
bss_size=0
peid=0
strip=1
d2flg=0
MAKE_CART=${MAKE_CART:-make}

# MAX realprealloc() address

bss_addr=8126456
td_addr=1048592
bss_accum=0
td_accum=0
sys_init_addr=0
usr_init_addr=0
# bss maximum size is 100K + 8bytes of pad
bss_limit=204800

iFLAG=""
IFLAG=""
mFLAG=""
MFLAG=""
OARG="cartridge"
CART_ID=0x0000
USAGE="usage: make_cart.735 [-im] [-o <file>] [-c <cartridge_id>] [-b <bss_address>]"

# parse command line options
set -- `getopt imo:c:b: $*`
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
	-i)	iFLAG=$i;shift;;
	-I)	IFLAG=$i;shift;;
	-m)	mFLAG=$i;shift;;
	-M)	MFLAG=$i;shift;;
	-o)	OARG=$2; shift 2;;
	-c)	CART_ID=$2; shift 2;;
	-b)	bss_addr=$2; shift 2;;
	--)	shift; break;;
	esac
done

bss_end_addr=$bss_addr
bss_start_addr=`expr $bss_end_addr - $bss_limit`
sysrtn="header.c header.o sys_init.c sys_init.o usr_init.c usr_init.o"
tmplist="/tmp/*$$* $sysrtn"
trap "exit 1" 1 2 3 15
trap "rm -f $tmplist usr_init sys_init;" 0

# initialize the output files sys_init.c, usr_init.c
cat > sys_init.c <<\
!
void (*init[])() = {
!

cat > usr_init.c <<\
!
void (*init[])() = {
!

cmd=$0
echo cmd=$cmd
while read src_name sys_init_nm usr_init_nm type dflag src_file
do
#dflag=0
	echo "$src_name"
	PRGNAME=`basename $src_name '\.c'`
	SRCNAME="$src_file"
	DIRNAME=`dirname  $src_name`
	OBJNAME=/tmp/`basename $src_file '\.c'`.o
echo "prg=$PRGNAME src=$SRCNAME dir=$DIRNAME"

	MTLMAP=""
	MLDFLAGS="-L$DMD/lib -a -r"
	obj_name=/tmp/$PRGNAME.C
	MLDOBJ=" -o $obj_name"

	if [ ! -r $SRCNAME ]
	then
		echo "$cmd: cannot read $SRCNAME"
		exit 2
	fi

	if [ $type = $EXEC_RAM -o $type = $FONTFILE ]
	then
		$DMD/bin/dmdcc -c $SRCNAME
		if [ $? != 0 ] 
		then
			echo "$cmd: $SRCNAME: dmdcc failed"
			exit 2
		fi
		$MLD $MLDOBJ -L$DMD/lib -a -r $DMD/lib/crtm.o $PRGNAME.o \
			-ljx -lj -lfw -lc
		if [ $? != 0 ] 
		then
			echo "$cmd: $SRCNAME: $MLD failed"
			exit 2
		fi


	elif [ $type != $EXEC_ROM ]
	then
		echo "$cmd: $type: invalid type code"
		exit 2
	else
		save=`pwd`
		cd /tmp
		dmdcc -c $SRCNAME
		cd $save
		cd $DIRNAME
		$MAKE_CART DMD=$DMD MLD=$DMD/bin/mc68ld MLDFLAGS="$MLDFLAGS" MLNDIR="" MTLMAP="$MTLMAP" MLDOBJ="$MLDOBJ $OBJNAME" $PRGNAME
		if [ $? != 0 ]
		then
			echo "$cmd: $PRGNAME: $NAME_CART command failed"
			cd $save
			exit 2
		else
			cd $save
		fi
	fi


	obj_num=`expr $obj_num + 1`


	# determine the relative address of sys_init_addr 
	# and usr_init_addr.
	if [ "$sys_init_nm" -a $sys_init_nm != "null" ]
	then
		echo "nmgrep $sys_init_nm $obj_name"
		sys_init_addr=`mc68nm $obj_name | \
				grep "^$sys_init_nm[ \t]" | \
				cut -d"|" -f2 | \
				sed s"/[ \t]*//"`
		sys_init_addr=${sys_init_addr:=ZERO}
		if [ sys_init_addr -eq 0 ]
		then
			echo "$cmd: Warning: system initialization routine $sys_init_nm"
			echo "was not found for $obj_name"
		fi
	else
		sys_init_addr=0
	fi

	if [ "$usr_init_nm" -a $usr_init_nm != "null" ]
	then
		echo "nmgrep $usr_init_nm $obj_name"
		usr_init_addr=`mc68nm $obj_name | \
				grep "^$usr_init_nm[ \t]" | \
				cut -d"|" -f2 | \
				sed s"/[ \t]*//"`
		usr_init_addr=${usr_init_addr:=ZERO}
		if [ usr_init_addr -eq 0 ]
		then
			echo "$cmd: Warning: user initialization routine $sys_init_nm"
			echo "was not found for $obj_name"
		fi
	else
		usr_init_addr=0
	fi

	# determine the absolute address of sys_init_addr and
	# usr_init_adr by adding td_addr.  If address is not 0,
	# place the absolute addresses sys_init_addr and
	# usr_init_addr in the files sys_init.c and usr_init.c
	# respectively.
	if [ $sys_init_addr -ne 0 ]
	then
		sys_init_addr=`expr $sys_init_addr + $td_addr`
		cat >> sys_init.c <<\
!
	(void (*)())$sys_init_addr,
!
	fi

	if [ $usr_init_addr -ne 0 ]
	then
		usr_init_addr=`expr $usr_init_addr + $td_addr`
		cat >> usr_init.c <<\
!
	(void (*)())$usr_init_addr,
!
	fi

	# generate the map file which directs mc68ld to link load
	# obj_name to the absolute text/data address td_addr and
	# absolute bss address bss_addr.  Since we are using the
	# preallocation scheme, we need first to determine where
	# to start the bss by subtracting the size of the bss
	# section for obj_name from bss_addr.
	echo "create map file for $obj_name"
	mc68size $obj_name > /tmp/size$$
	if [ $? != 0 ]
	then
		echo "$cmd: mc68size fails on $obj_name"
		exit 2
	fi
	read st_size p1 sd_size p2 sb_size equals total < /tmp/size$$
	st_size=`expr $st_size + $TEXT_FUDGE`
	sb_size=`expr $sb_size + $BSS_FUDGE`
	bss_addr=`expr $bss_addr - $sb_size`
	data_addr=`expr $td_addr + $st_size`
	peid=`getpeid $obj_name`
	if [ $? != 0 ]
	then
		echo "$cmd: getpeid failed on $obj_name"
		exit 2
	fi

	if [ "$dflag" -a $dflag != 0 ]
	then
		d2flg=$dflag
		mv $obj_name $PRGNAME.C
		echo "st_size=$st_size sd_size=$sd_size sb_size=$sb_size"
		echo "ba=$bss_addr da=$data_addr id=$peid"
		echo "ua=$usr_init_addr & sa=$sys_init_addr"
	fi

# set the start address of the data and bss section.

	if [ $type != $FONTFILE ]
	then

	sed -e "/__progid =/c\\
long __progid = $peid;" \
	    -e "/__data =/c\\
char \*__data = (char \*)$data_addr;" \
	    -e "/__bss  =/c\\
char \*__bss  = (char \*)$bss_addr;" \
    	    <$SRCNAME >$SRCNAME$$


		if [ $? != 0 ]
		then
			echo "$cmd: $SRCNAME: sed failed"
			rm -f $SRCNAME$$
			exit 2
		fi
		mv $SRCNAME$$ $SRCNAME
		rm -f $SRCNAME$$
	fi


	cat > /tmp/ifile$obj_num$$ <<\
!
MEMORY {
        cbss : org = $bss_start_addr, len = $bss_limit
        crom : org = 0x100000, len = 0x60000
}


SECTIONS {
        GROUP $td_addr: { 
                .text: {}
                .data: {}
        } > crom

        GROUP $bss_addr: {
                .bss    :{}
        } > cbss
}
!
	# if the -i option was specified, create a local copy of
	# the map ifile.
	if [ $iFLAG ]
	then
		cp /tmp/ifile$obj_num$$ ifile$obj_num
	fi

	MLNDIR=/tmp/ifile$obj_num$$
	MLDOBJ="-o /tmp/obj$obj_num$$"
	MLDFLAGS="-L$DMD/lib -a"

	if [ $strip ]
	then
		MLDFLAGS="$MLDFLAGS -s"
	fi

	if [ $mFLAG ]
	then
		MLDFLAGS="$MLDFLAGS -m"
		MTLMAP=" > map$obj_num"
	fi
	
	if [ $type = $EXEC_RAM -o $type = $FONTFILE ]
	then
		$DMD/bin/dmdcc -c $SRCNAME
		if [ $? != 0 ] 
		then
			echo "$cmd: $SRCNAME: dmdcc failed"
			exit 2
		fi
		$MLD $MLDOBJ $MLDFLAGS $DMD/lib/crtm.o $PRGNAME.o \
	  	     $MLNDIR -ljx -lj -lfw -lc $MTLMAP
		if [ $? != 0 ] 
		then
			echo "$cmd: $SRCNAME: $MLD failed"
			exit 2
		fi

	else
		save=`pwd`
		cd /tmp
		dmdcc -c $SRCNAME
		cd $save
		cd $DIRNAME
		$MAKE_CART DMD=$DMD MLD=$DMD/bin/mc68ld MLDFLAGS="$MLDFLAGS" MLNDIR="$MLNDIR"  MTLMAP="$MTLMAP" MLDOBJ="$MLDOBJ $OBJNAME" $PRGNAME
		if [ $? != 0 ]
		then
			echo "$cmd: $PRGNAME: $MAKE_CART command failed"
			cd $save
			exit 2
		else
			cd $save
		fi
	fi

	if [  $type != $EXEC_ROM -a "$dflag" -a $dflag = 0  ]
	then
		rm -f $DIRNAME/$PRGNAME.o $SRCNAME
	fi

	if [ "$mFLAG" -a $DIRNAME != "." ]
	then
		mv $DIRNAME/map$obj_num .
	fi


	# Get size of text, data and bss sections to update the variables
	# bss_accum, td_accum, and td_addr.
	echo "mc68size $obj_name"
	mc68size /tmp/obj$obj_num$$ > /tmp/size$$
	if [ $? != 0 ]
	then
		echo "$cmd: mc68size fails on $obj_name"
		exit 2
	fi
	read text_size p1 data_size p2 bss_size equals total < /tmp/size$$

	if [ "$dflag" -a $dflag != 0 ]
	then
	  echo "text_size=$text_size data_size=$data_size bss_size=$bss_size"
	fi

	if [ $st_size != $text_size -o $sd_size != $data_size \
		-o $sb_size != $bss_size ]
	then
		echo "$cmd: text: $st_size $text_size"
		echo "    data: $sd_size $data_size"
		echo "    bss:  $sb_size $bss_size"
		echo "    phase error - object size changed"
		exit 2
	fi

	bss_accum=`expr $bss_accum + $bss_size`
	td_accum=`expr $td_accum + $text_size + $data_size`
	td_addr=`expr $td_addr + $text_size + $data_size`
	dflag=""
done

# complete the files sys_init.c and usr_init.c.
cat >> sys_init.c <<\
!
	(void (*)())0,
};

main()
{
	long *vt;
	void (**p)() = init;
	while(*p)
		(*p++)();

	vt = *((long **)((*(long *)176) + 6));
	/*
	** Fix Rommax
	*/
	vt[-124] = (long)0x200000;
}
!

cat >> usr_init.c <<\
!
        (void (*)())0,
};


main()
{
    void (**p)() = init;
    int *bss_end;
    int *bssp;
    long *sys;
    char *(*realprealloc)();
    
    sys = *((long **)((*(long *)176) + 6));
    realprealloc = (char *(*)())sys[609];
    bss_end = (int *)($bss_end_addr);
    bssp = (int *)realprealloc(((char *)bss_end-$bss_accum),${bss_accum}L,0L);
    for( ; bssp <= bss_end; bssp++)
	*bssp = 0;
    while(*p)
        (*p++)();
}
!

# Record the current td_addr as the start of the text section
# for sys_init.c.
sys_init_addr=$td_addr

# Compile sys_init.c
dmdcc -c sys_init.c

# Get size of text data and bss for sys_init.o
mc68size sys_init.o > /tmp/size$$
read text_size p1 data_size p2 bss_size equals total < /tmp/size$$

bss_addr=`expr $bss_addr - $bss_size`
bss_accum=`expr $bss_accum + $bss_size`
td_accum=`expr $td_accum + $text_size + $data_size`

# generate the map file which directs mc68ld to link load
# sys_init.o to the absolute text/data address td_addr and
# absolute bss address bss_addr.
echo "create map file for sys_init.o"
cat > /tmp/sys_ifile$$ <<\
!
MEMORY {
        cbss : org = $bss_start_addr, len = $bss_limit
        crom : org = 0x100000, len = 0x60000
}


SECTIONS {
        GROUP $td_addr: {
                .text: {}
                .data: {}
        } > crom

        GROUP $bss_addr: {
                .bss    :{}
        } > cbss
}
!

# link load sys_init.o
echo "mc68ld sys_init.o"
mc68ld -a -s -m -o sys_init sys_init.o /tmp/sys_ifile$$ > sys.map
rm /tmp/sys_ifile$$

# Calculate new td_addr
td_addr=`expr $td_addr + $text_size + $data_size`


# Record the current td_addr as the start of the text section
# for usr_init.c.
usr_init_addr=$td_addr

# Compile usr_init.c
dmdcc -c usr_init.c

# Get size of text data and bss for usr_init.o
mc68size usr_init.o > /tmp/size$$
read text_size p1 data_size p2 bss_size equals total < /tmp/size$$

bss_addr=`expr $bss_addr - $bss_size`
bss_accum=`expr $bss_accum + $bss_size`
td_accum=`expr $td_accum + $text_size + $data_size`

# generate the map file which directs mc68ld to link load
# usr_init.o to the absolute text/data address td_addr and
# absolute bss address bss_addr.
echo "create map file for usr_init.o"
cat > /tmp/usr_ifile$$ <<\
!
MEMORY {
        cbss : org = $bss_start_addr, len = $bss_limit
        crom : org = 0x100000, len = 0x60000
}


SECTIONS {
        GROUP $td_addr: {
                .text: {}
                .data: {}
        } > crom

        GROUP $bss_addr: {
                .bss    :{}
        } > cbss
}
!

# link load usr_init.o
echo "mc68ld usr_init.o"
mc68ld -a -s -m -o usr_init usr_init.o /tmp/usr_ifile$$ > usr.map
rm /tmp/usr_ifile$$

# Calculate new td_addr
td_addr=`expr $td_addr + $text_size + $data_size`

# Create header.c
cat > header.c <<\
!
struct C_hdr {
        unsigned int uid;       /* user id */
        unsigned int sid;       /* system id */
        unsigned long sizbss;   /* .bss section of whole cartridge */
        void (*init1)();        /* system initialization routine */
        void (*init2)();        /* user initialization routine */
};

struct C_hdr My_hdr = {
        $CART_ID,               /* anything but 0x007b and 0xaaaa */
        0x6300,                 /* first byte must be 0x63 */
        0L,             	/* zero since we are using prealloc */
        (void (*)())$sys_init_addr,         /* not needed */
        (void (*)())$usr_init_addr,         /* user initialization routine */
};
!

# Compile header.c
echo "dmdcc -c header.c"
dmdcc -c header.c

# Create combined ifile
cat > comb_ifile <<\
!
MEMORY {
        cbss : org = $bss_start_addr, len = $bss_limit
        crom : org = 0x100000, len = 0x60000
}


SECTIONS {
	GROUP 0x100000 : {
		.head: {header.o(.data)}
!

count=1
while [ count -le $obj_num ]
do
	cat >> comb_ifile <<\
!
		.obj$count: {/tmp/obj$count$$(.text, .data)}
!
	count=`expr $count + 1`
done

cat >> comb_ifile <<\
!
		.sys: {sys_init(.text, .data)}
		.usr: {usr_init(.text, .data)}
	} > crom

	GROUP : {
!

count=1
while [ count -le $obj_num ]
do
        cat >> comb_ifile <<\
!
                .obj$count: {/tmp/obj$count$$(.bss)}
!
	count=`expr $count + 1`
done

cat >> comb_ifile <<\
!
	} > cbss
}
!

# create a list of objects to be link loaded
obj_list=header.o

count=1
while [ $count -le $obj_num ]
do
	obj_list="$obj_list /tmp/obj$count$$"
	count=`expr $count + 1`
done

obj_list="$obj_list sys_init usr_init"

# link load all objects
mc68ld -a -s -m -o $OARG $obj_list comb_ifile > $OARG.map

		if [  $d2flg = 0  ]
		then
			rm -f comb_ifile sys.map usr.map $OARG.map
		fi

# Modified 12/5/90 by Mark Habenicht to work for 730+ cartridge.
# Make ROM images
# Check how many ROMS will be needed
# 1310718 is 0x100000 + 256K bytes -2 bytes for checksum.  Use 2 128K byte ROMS
# 1572862 is 0x100000 + 512K bytes -2 bytes for checksum.  Use 4 128K byte ROMS
if [ $td_addr -le 1310718 ]
then
	ROMLIMIT=140000
elif [ $td_addr -le 1572862 ]
then
	ROMLIMIT=180000
else
	echo "size of text and data has exceeded maximum cartridge size: 512K bytes"
	exit 2 
fi

echo "Making ROM images"
aconv -ofs -s 100000 -m $ROMLIMIT -p 128 $OARG

