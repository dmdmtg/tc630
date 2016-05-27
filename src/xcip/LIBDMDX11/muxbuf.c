#include "jerq.h"
#ifdef SUNTOOLS
static char *buffile = "/tmp/muxbuf";
#define DOSEL 1
#ifdef DOSEL
#include <suntool/selection_svc.h>
#include <suntool/selection_attributes.h>
#endif
#include <sys/file.h>
#endif /*SUNTOOLS*/

typedef struct String{
	char *s;	/* pointer to string */
	short n;	/* number used, no terminal null */
	short size;	/* size of allocated area */
} String;

#ifdef DOSEL
static char *selbufp;
static int context = 0;

selread(buffer)
Seln_request *buffer;
{
	char *reply;

	if (*buffer->requester.context == 0) {
		*buffer ->requester.context = 1;
		if (buffer == NULL) reply = "";
		else reply = buffer->data + sizeof(Seln_attribute);
	} else reply = buffer->data;
	
	while (*reply) *selbufp++ = *reply++;
	*selbufp = 0;
	return(SELN_SUCCESS);
}
#endif /* DOSEL */

getmuxbuf (pmb)
String *pmb;
{
	char *ans;
	int n = 0;

#ifdef X11
	ans = XFetchBytes(dpy, &n);
#endif /*X11*/
#ifdef SUNTOOLS
#define BSIZE 4096
	char buffer[BSIZE];
	int fd;
#ifdef DOSEL
	Seln_client client;
	Seln_holder holder;

	holder = seln_inquire(SELN_PRIMARY);
	if (holder.state != SELN_NONE) {
		selbufp = buffer;
		*buffer = 0;
		context = 0;
		seln_query (&holder, selread, &context, SELN_REQ_CONTENTS_ASCII, 0, 0);
		n = strlen(buffer);
	}
#endif /* DOSEL */
	if (n == 0) {
		fd = open(buffile, O_RDONLY);
		if (fd < 0)
			return;
		n = read(fd, buffer, BSIZE);
		close(fd);
	}
	if (n < 0)
		return;
	ans = buffer;
#endif /*SUNTOOLS*/
	if (pmb->size < (n+1)) {
		pmb->size = n+1;
		gcalloc(pmb->size, &(pmb->s));
	}
	pmb->n = n;
	if (n)
		strncpy(pmb->s, ans, n);
	pmb->s[n] = 0;
#ifdef X11
	free(ans);
#endif /*X11*/
}

setmuxbuf(pmb)
String *pmb;
{
#ifdef X11
	XStoreBytes(dpy, pmb->s, pmb->n);
#endif/* X11*/
#ifdef SUNTOOLS
	int fd = open(buffile, O_WRONLY|O_CREAT|O_TRUNC, 0666);
	if (fd < 0)
		return;
	fchmod(fd, 0666);	/* Umask may change mode from open */
	write(fd, pmb->s, pmb->n);
#ifdef DOSEL
	seln_hold_file(SELN_PRIMARY,buffile);
#endif /* DOSEL */
	close(fd);
#endif /*SUNTOOLS*/
}
