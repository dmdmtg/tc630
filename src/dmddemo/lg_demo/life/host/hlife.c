/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)hlife.c	1.1.1.1	(5/26/88)";

/*
 * host life: read and write files
 */

#include <stdio.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <termio.h>
#include <ctype.h>

int	bugout();
struct termio ttyraw, ttysave;

main(argc, argv)
char **argv;
{
	char file[128];
	register c;
	FILE *fp;

	setbuf(stdout, NULL);
	ioctl(0, TCGETA, &ttysave);
	signal(SIGHUP, bugout);
        ttyraw.c_iflag = IGNBRK;
        ttyraw.c_cflag = (ttysave.c_cflag & CBAUD) | 
                         (ttysave.c_cflag & CLOCAL) | CS8 | CREAD;
        ttyraw.c_cc[VMIN] = 1;
        ttyraw.c_cc[VTIME] = 1;
        (void)ioctl(1, TCSETAW, &ttyraw);
	for (;;) {
		switch (getchar()) {

		case 'r':
			getfile(file);
			if ((fp = fopen(file, "r")) == NULL) {
				putchar('n');
			} else {
				int result = 'Y';

				putchar('y');
				while ((c = getc(fp)) != EOF) {
					if (isdigit(c) || c==' ' || c=='\t'
					  || c=='\n' | c=='-')
						putchar(c);
					else {
						result = 'N';
						break;
					}
				}
				putchar(result);
				fclose(fp);
			}
			continue;

		case 'w':
			getfile(file);
			if ((fp = fopen(file, "w")) == NULL) {
				putchar('n');
			} else {
				putchar('y');
				while((c = getchar()) != EOF && c != 'X')
					putc(c, fp);
				fclose(fp);
			}
			continue;

		default:
			break;
		}
		break;
	}
	bugout();
}

getfile(f)
register char *f;
{
	register c;

	while ((c = getchar()) != EOF && c != '\n')
		*f++ = c;
	*f++ = '\0';
}

bugout()
{
	ioctl(0, TCSETAW, &ttysave);
	exit(0);
}
