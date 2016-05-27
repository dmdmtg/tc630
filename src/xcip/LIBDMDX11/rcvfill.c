#include "jerq.h"
#include "rcv.h"
#if defined(BSD) || defined(SYSV)
#include <fcntl.h>
#endif

rcvfill ()
{
	static char rbuf[1024];
	register i;

	i = min (1024, Jrcvbuf.size - Jrcvbuf.cnt);
	i = read(0, rbuf, i);
	if (i <= 0) {
#if defined(BSD) || defined(SYSV)
		fcntl(1, F_SETFL, 0);
#endif
		exit(0);
	}
	rcvbfill(rbuf, i);
}
