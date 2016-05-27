/*     Copyright (c) 1991 AT&T */
/*     All Rights Reserved     */

/*     THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T     */
/*     The copyright notice above does NOT evidence any        */
/*     actual or intended publication of such source code.     */
/*     Department 45262 reserves the rights for distribution.  */
/*     Filename: makelock.c.                                        */

#ident "@(#)makelock.c   version  25.1.2.4  RDS UNIX source.  Last delta: 7/22/91 10:46:05"

/*
 * makelock
 *
 * create a lock file
 * Includes SVR4 password standards.
 *  These are:
 *	o It must contain at least 6 characters.
 *	o It cannot be a circular shift of the login ID.
 *	o It must contain at least two alphabetid characters
 *	  and one numeric or special character.
 *
 *	This code borrows heavily from taken from tlock.c
 *	  S.D.Ericson 7/91
 *
 */

#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <sys/termio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include "stdpass.h"

#ifndef FILENAME_MAX
#define	FILENAME_MAX	1024
#endif

#define TRUE 1
#define FALSE 0
#define PWTIMEOUT	30	/* confirmation timeout is 30 secs */
#define MAXPASS		20	/* maximum passwd allowed */
#define FMODE		00400	/* .dmd.lock mode,  read-only by owner */

char    usertty[256];
char	buffer[MAXPASS];

	
main(argc,argv)
int argc;
char **argv;
{
	void   restore();
	extern char	*getenv();
	struct stat	fbuf;
	struct passwd 	*pwd, *getpwnam();
 	int	ret;
	char 	ttycmd[100],cmd[75], pass2[MAXPASS];
	char	good=FALSE;
	char	password[MAXPASS];
	char    filename[FILENAME_MAX], newname[FILENAME_MAX];
	char 	*home, *logname;
	char 	*getlogin(), *pw, *crypt(), *lgetpass();
	FILE	*stream, *fp;
	putenv("IFS= \n\t");

	/* get user tty definition for reset on makelock exit */
	if ((fp = popen("/bin/stty -g","r")) == NULL) {
		fprintf(stderr,"makelock: cannot popen stty -g");
		pclose(fp);
		exit(2);
	}				/* end popen if */
	else
	{
		if (fscanf(fp,"%s",usertty) == EOF) {
		      fprintf(stderr,"makelock: cannot read popen stty -g");
		      exit(2);
		}
		pclose(fp); 
	}
	

/*
	logname = getlogin();
*/
	logname = cuserid(NULL);

	if ((pwd=getpwnam(logname)) == NULL) {
		fprintf(stderr, 
		 "%s: cannot get entry from password file\n",argv[0]);
		exit(1);
	}

	if (isatty(0) == 0) {
		fprintf(stderr,"%s: stdin not a tty\n",argv[0]);
		exit(1);
	}
	if (fstat(0, &fbuf) < 0) {
		fprintf(stderr,"%s: cannot stat tty device (stdin)\n",argv[0]);
		exit(1);
	}
	if (fbuf.st_uid != pwd->pw_uid) {/* did not log on as this login */
		fprintf(stderr,
		 "\nmakelock: should not be executed while inside \"su\"\n\n");
		exit(1);
	}

	home = pwd->pw_dir;
	if ((home == NULL) || (*home == '\0')) {
		fprintf(stderr,"%s: cannot get login directory\n",argv[0]);
		exit(2);
	}

	strcpy(filename,home);
	strcat(filename,"/.dmd.lock");
	strcpy(newname,filename);
	strcat(newname,".new");		/* temp place for new lock file */
	
	sprintf(cmd,"/bin/stty -echo");

	sigset (SIGINT,restore);     /* Interrupt */
	sigset (SIGQUIT,restore);     /* QUIT */
	sigset (SIGALRM,restore);     /* ALARM */
	if ((ret=system(cmd)) < 0) {
		printf("makelock: Cannot do: stty -echo");
		exit(1);
	}

	while (good == FALSE) {
		alarm((unsigned)PWTIMEOUT);	/*set password timeout*/

		strcpy(password,lgetpass("Enter password: "));

		good=FALSE;
		switch ( stdpass(pwd->pw_name, password) ) 
		{
		case PWDOK:
			good=TRUE;
			break;
		case PWDSHORT:		/* check password length */
			fprintf(stderr,"Password is too short, it must ");
			fprintf(stderr,"contain at least 6 characters.\n");
			break;
                case PWDCIRC:	/* Password is circular shift of login */
			fprintf(stderr,"Password cannot be a circular ");
			fprintf(stderr,"shift of your login ID.\n");
			break;
                case PWDNOMIX:
		/* passwords must contain at least two alpha characters */
		/* and one numeric or special character                 */
			fprintf(stderr,"Password must have at least two ");
			fprintf(stderr,"alphabetic\n");
			fprintf(stderr,"characters and one numeric or ");
			fprintf(stderr,"special character.\n");
			break;
		}
		if ( good == FALSE )
		        continue;	/* bad password - don't confirm */
					
		alarm((unsigned)PWTIMEOUT);	/*set password timeout*/
		strcpy(pass2,lgetpass("Re-enter password: "));

		if (strcmp(password,pass2)) {
			fprintf(stderr, "They don't match!  ");
			fprintf(stderr, "Try again.\n");
			good=FALSE;
		} 
	} 

	alarm( (unsigned) 0);   /* turn off time-out - have good password */
	
	stream = fopen(newname,"w");
	if (stream == NULL) {
		fprintf(stderr,"%s: can\'t create new lock file %s\n",
			argv[0],filename);
	/* Added the following two lines to re-set user tty if the filesystem */
	/* has run out of inodes.  Out of blocks condition gets re-set below. */
		sprintf(ttycmd,"/bin/stty %s",usertty);
		system(ttycmd);                /* re-set user tty settings */
		exit(1);
	} else {
		pw = crypt(password,"V0");
		fprintf(stream,"%s\n",pw);
		(void) unlink(filename);
		if ((fclose(stream) == 0) &&
			(chmod(newname,FMODE) == 0) &&
#ifdef SVR4
			(rename(newname,filename) == 0))
#else
			(link(newname,filename) == 0) &&
			(unlink(newname) == 0))
#endif
		{
			fprintf(stdout, "Your password is stored in %s\n",
					filename);
			fprintf(stdout, "dmdlock -u will use this file ");
			fprintf(stdout,"to set the password.\n"); 
		}
		else 
		{
			perror(argv[0]);
			unlink(newname);	/* make sure temp is gone */
		}
	}

	/* If filesystem is out of blocks it is caught here. */
	sprintf(ttycmd,"/bin/stty %s",usertty);
	system(ttycmd);                         /* re-set user tty settings */
	return(0);				/* keeps lint quiet */
}



/*
 * local getpass which doesn't read from /dev/tty 
 */
char *
lgetpass(prompt)
char	*prompt;
{
	register int	i;

	if (prompt)
		fprintf(stdout,"%s", prompt);

	fflush(stdout);
	if (i=read(0,buffer,MAXPASS-1)) 
		i--;			/* eat the \n */
	else			
		i = 0;			/* read returned error (-1) or EOF */

	buffer[i]='\0';
	putchar('\n');
	return(buffer);
}


void
restore(sig)
int sig;
{
	char 	ttycmd[100];
	switch (sig) 
	{
	      case SIGALRM:
		fprintf(stderr,"\nPassword entry timed out\n");
		break;
	}
	sprintf(ttycmd,"/bin/stty %s",usertty);
	system(ttycmd);                         /* re-set user tty settings */
	exit(sig);
}
