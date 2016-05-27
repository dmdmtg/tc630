/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)strcat2.c	1.1.1.2 88/02/10 17:14:04";

/* procedures */


/*
 * strcat2 shifts str2 to the right for the length of str1.  It then
 * inserts str1 into the hole thereby created in str2.
 */
char *
strcat2 (str1, str2)	/* returns str2 with str1 prepended */
    char *str1, *str2;
{
	register char c, *srcp, *dstp;
	register short s1len, s2len;

	/*
	 * shift str2 to the right to make room for str1.
	 * Copy backwards (to avoid overwriting) from the terminator
	 * of str2 to where the terminator of the final string will reside.
	 */
	srcp = str2 + (s2len = strlen (str2));
	dstp = srcp + (s1len = strlen (str1));

	while (s2len-- >= 0)
	    *(dstp--) = *(srcp--);

	/*
	 * now copy str1 into the hole created by the shift.
	 * After shifting, srcp has been decremented to one less
	 * than the beginning of str2, so subsequent use must
	 * do a pre-increment.
	 */
	while (s1len-- > 0)
	    *(++srcp) = *(str1++);

	return str2;
}
