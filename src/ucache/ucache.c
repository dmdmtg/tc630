/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)xucache.c	1.1.1.2	(5/10/87)";

#include <dmd.h>
#include <menu.h>
#include <object.h>


struct Tmenu obmenu;
unsigned long trashdata[] = {
	0x49800000,
	0x01500000,
	0x10B80000,
	0x845C0000,
	0x00280000,
	0x4FF40000,
	0x080A0000,
	0x0D5E0000,
	0x05500000,
	0x05500000,
	0x05500000,
	0x0D500000,
	0x1F500000,
	0x3FF00000
};
Bitmap trashcan = {
	(Word *) trashdata,
	2,
	0, 0, 14, 14,
	0
};

unsigned long lockdata[] = {
	0x00000000,
	0x1E000000,
	0x3F000000,
	0x73800000,
	0x61800000,
	0x61800000,
	0xFFC00000,
	0xFFC00000,
	0xC0C00000,
	0xCCC00000,
	0xCCC00000,
	0xCCC00000,
	0xC0C00000,
	0x7F800000,
	0x3F000000,
	0x00000000,
};
Bitmap lock = {
	(Word *) lockdata,
	2,
	0, 0, 16, 16,
	0
};



char *Pgm_name;

main (argc, argv)
int argc;
char **argv;
{
	Titem1 *genesis();
	void unmount();

	Pgm_name = &argv[0][strlen(argv[0])];
	while(--Pgm_name > &argv[0][0]) {
	    if (*Pgm_name == '/') {
		Pgm_name++;
		break;
	    }
	}
	obmenu.item = (Titem *)0;
	obmenu.generator = (Titem *(*)())genesis;
	obmenu.menumap = TM_TEXT|TM_UFIELD|TM_NEXT|TM_ICON|TM_DFN;

	cmdcache (*Pgm_name == 'x' ? "xucache" : "ucache",
				&obmenu, &trashcan, 0l, unmount);
}


void
unmount (val)
int val;
{
	register Obj *obj;
	extern Obj *findobj();

	if (obj = findobj ("i", val)) {
		apdecache (obj);
		obmenu.prevhit = 0;	/* don't want user to pop back into something 
					** that can be removed. The user can release
					** the mouse by mistake.
					*/
	}
}

Titem1 *
genesis (i, m)
register int i;
register Tmenu *m;
{
	register Titem1 *item = &useritems;
	register Obj *obj;
	register Appl *info;
	extern Obj *findobj();

	if (!(obj=findobj("i", i)))
		item->text = (char *)0;
	else {
		info = obj->info.cappl;
		item->text = info->caption ? info->caption : obj->name; 
		item->ufield.uval = i;
		item->ufield.grey = (obj->user || obj->type&PERMANENT) ? 1 : 0;
		item->icon = (obj->type & PERMANENT) ? &lock : (Bitmap *)0;
		item->next = (Tmenu *)0;
	}
	return (item);
}
