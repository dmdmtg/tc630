.ND "May 25, 1988"
.TL "311401-2299, 311401-0199" "38794-23, 25952"
A 5620 Emulator Library for Version 11 X Windows and Suntools
.AU "D. A. Kapilow" DAK MH 11228 3596 2B-424 alice!dak
.AU "J. I. Helfman" JH MH 11229 5087 2C-535 alice!jon
.TM 11228-880525-06TMS 11229-880525-04TMS
.fp 5 CW
.MT
.H 1 "Introduction"
An AT&T 5620 terminal emulator library
has been implemented to simplify porting the Ninth Edition Unix\(dd
5620 tools to version 11 of the X Window System* (X11)
.FS *
X Window System is a trademark of M.I.T.
.FE
and Sun Microsystem's \f5suntools\fR window environment.
.FS \(dd
Unix is a registered trademark of AT&T.
.FE
The library has been successfully used to port several
tools to the Sun Workstation\(dg.
.FS \(dg
Sun Workstation is a registered trademark of Sun Microsystems, Inc.
.FE
The same tool source code can be compiled for either X11 or
\f5suntools\fR running under Sun's Berkeley (BSD)
derived Unix system (SunOS), or X11 under Ninth Edition Unix.
.P
The emulator library and many of the tools should easily port to other
hardware running Unix and X11 because of the device independence of 
both the operating system and windowing software.
The library should work unmodified on other BSD
Unix systems that support X11.
While designed for 5620 applications, it should also be possible
to port code written for other members of the AT&T DMD
terminal family, such as the 630.
.P
Other 5620 emulator libraries have been written\*(Rf
.RS
M. E. Meth and C. D. Blewett, \fIA Mux Emulator Running under
X-Windows\fR, TM 11229-870827-06TMS
.RF
\*(Rf.
.RS
M. J. Hawley and S. J. Leffler, \fIWindows for UNIX at Lucasfilm\fR,
Proceedings Summer USENIX Meeting, 1985, pp. 393-406
.RF
This work is a refinement and extension of a previous library\*(Rf
designed for X11 on a Ninth Edition Unix system.
.RS
D. A. Kapilow, \fIA Port of the Ninth Edition Unix Kernel,
X Window System, and 5620 Tools to the Sun Workstation\fR,
TM 11228-871119-17TMS
.RF
.H 1 "Host-Terminal Model"
In the 5620 environment applications are broken into two
separate processes that run on different processors.
The \fIterminal\fR process is down-loaded to the 5620 and
has access to the display, keyboard, and mouse.
A cooperating \fIhost\fR process runs on a Unix computer
allowing access to Unix resources.
On Ninth Edition Unix systems,
\f5mux\fR\*(Rf manages the communications to allow multiple
host-terminal process pairs to run concurrently over a single
serial communications channel.
.RS
Unix Time-Sharing System, \fIProgrammer's Manual\fR, Ninth Edition,
Volume 1, AT&T Bell Laboratories, 1986
.RF
This is illustrated in Figure 1 below.
.DS CB
.FG "AT&T 5620 Terminal Application Model" 3 2
.PS
scale=81
box invis ht 240 wid 304 with .sw at 0,0
line <-> from 160,120 to 160,80 
box ht 64 wid 80 with .nw at 216,80 
box ht 64 wid 80 with .nw at 112,80 
box ht 64 wid 80 with .nw at 8,80 
box ht 64 wid 80 with .nw at 64,224 
"\f4\s10\&2\f1\s0" at 256,33
line <-> from 208,160 to 208,136 
box ht 16 wid 216 with .nw at 48,136 
"\f4\s10\&AT&T 5620 Terminal\f1\s0" at 48,233 ljust
box ht 128 wid 232 with .nw at 40,240 dotted
"\f4\s10\&Mux Terminal Operating System\f1\s0" at 156,129
line <-> from 104,160 to 104,136 
"\f4\s10\&2\f1\s0" at 208,177
"\f4\s10\&UNIX System\f1\s0" at 8,9 ljust
box ht 88 wid 304 with .nw at 0,88 dotted
"\f4\s10\&1\f1\s0" at 48,33
"\f4\s10\&1\f1\s0" at 104,177
"\f4\s10\&Host\f1\s0" at 256,65
"\f4\s10\&Host\f1\s0" at 48,65
"\f4\s10\&Terminal\f1\s0" at 208,209
"\f4\s10\&Terminal\f1\s0" at 104,209
"\f4\s10\&Process\f1\s0" at 208,193
"\f4\s10\&Process\f1\s0" at 104,193
box ht 64 wid 80 with .nw at 168,224 
line <-> from 192,48 to 216,48 
"\f4\s10\&Process\f1\s0" at 256,49
"\f4\s10\&Process\f1\s0" at 48,49
line <-> from 88,48 to 112,48 
"\f4\s10\&Process\f1\s0" at 152,33
"\f4\s10\&Host\f1\s0" at 152,49
"\f4\s10\&Mux\f1\s0" at 152,65
.PE
.DE
.P
The two process implementation of applications is maintained in the
emulator library.
The \fIhost\fR and \fIterminal\fR processes run as separate
Unix processes on the same processor and
communicate with a bidirectional connection
appropriate for the operating system: a pipe in Ninth
Edition Unix and a Unix domain socket in BSD Unix.
The \fIterminal\fR process may have direct access to the
screen as in \f5suntools\fR, or may use another communications channel
to a display server as in X11.
The X11 case is illustrated in Figure 2 below.
.DS CB
.FG "Emulator Library Application Model (X11)" 4 2
.PS
scale=81
box invis ht 184 wid 432 with .sw at 0,0
box ht 184 wid 336 with .nw at 0,184 dotted
line <- from 208,48 to 280,48 
"\f4\s10\&Host\f1\s0" at 56,145
"\f4\s10\&Host\f1\s0" at 56,65
line <- from 280,96 to 280,48 
"\f4\s10\&Keyboard\f1\s0" at 392,129
"\f4\s10\&Display\f1\s0" at 392,145
"\f4\s10\&Mouse\f1\s0" at 392,113
box ht 64 wid 80 with .nw at 240,160 
box ht 64 wid 80 with .nw at 352,160 
line <-> from 320,128 to 352,128 
"\f4\s10\&X11\f1\s0" at 280,145
"\f4\s10\&Server\f1\s0" at 280,129
"\f4\s10\&Process\f1\s0" at 280,113
"\f4\s10\&1\f1\s0" at 56,113
"\f4\s10\&1\f1\s0" at 168,113
"\f4\s10\&2\f1\s0" at 56,33
"\f4\s10\&\f1\s0" at 56,145
box ht 64 wid 80 with .nw at 128,160 
"\f4\s10\&Process\f1\s0" at 168,129
box ht 64 wid 80 with .nw at 16,160 
line <-> from 96,128 to 128,128 
"\f4\s10\&Process\f1\s0" at 56,129
"\f4\s10\&Terminal\f1\s0" at 168,145
"\f4\s10\&\f1\s0" at 56,33
"\f4\s10\&2\f1\s0" at 168,33
"\f4\s10\&Terminal\f1\s0" at 168,65
"\f4\s10\&\f1\s0" at 56,65
box ht 64 wid 80 with .nw at 16,80 
box ht 64 wid 80 with .nw at 128,80 
"\f4\s10\&Process\f1\s0" at 56,49
"\f4\s10\&Process\f1\s0" at 168,49
line <-> from 96,48 to 128,48 
line <-> from 208,128 to 240,128 
"\f4\s10\&UNIX System\f1\s0" at 24,169 ljust
.PE
.DE
.P
In many tools implemented as two processes in the 5620
environment, the \fIhost\fR process is used only for
file service and data compression between the terminal and
host computer.
Tools using the emulator library
can directly access the file system at high bandwidth so
many of the simpler tools can be implemented with a single process.
.H 1 "Porting Guide"
.P
This section describes how to port a tool to the
emulator library environment.
It assumes familiarity with the interface provided by
\f5mux\fR to applications that run on the
AT&T 5620 terminal.
Section 3.1 and 3.2 give detailed descriptions of
the source code changes that must be made
in the \fIhost\fR and \fIterminal\fR processes.
Section 3.3 summarizes the porting procedure and section 3.4
contains a sample program.
.H 2 "Host Process Modifications"
Although the \fIhost\fR process does not directly interact with
the graphics system, a few source code changes are required for
proper operation in the emulator environment.
The \fIhost\fR process is responsible for starting
the \fIterminal\fR process and establishing the communications
channel between the processes.
A different mechanism is required to achieve this in
the emulator because both processes run on the same processor.
.P
In the 5620 environment, the \fIhost\fR process usually down-loads
the \fIterminal\fR process by calling \f532ld\fR.
The following \fIhost\fR process code fragment from the debugger
\f5pi\fR, is a simplified version of the code to down-load
\f5pads\fR, \f5pi\fR's \fIterminal\fR process, to the 5620.
.DS I N 10
\f5int fd;

system("/usr/jerq/bin/32ld /usr/jerq/mbin/pads.m");
fd = open("/dev/tty", 2);\fR
.DE
.P
The file descriptor, \f5fd\fR, is used in the \fIhost\fR
process to exchange information with the \fIterminal\fR process.
.P
In the emulator library, both processes run on the same processor
so a combination of \f5fork\fR and \f5exec\fR is used.
The emulator implementation assumes that file descriptors
in the \fIterminal\fR process are arranged so
\f5write\fRs to file descriptor 1 send data to the \fIhost\fR process
and \f5read\fRs from file descriptor 0 receive data from the
\fIhost\fR process.
Since a Unix process started with \f5exec\fR inherits open file
descriptors, the \fIhost\fR process can properly arrange the
file descriptors for the \fIterminal\fR process when it is started.
.P
Code to start the \fIterminal\fR process
for use with the emulator library is shown below:
.DS I N 10
	\f5int fds[2], pid;

	/* Establish a bidirectional connection */
#ifdef BSD
	socketpair(AF_UNIX, SOCK_STREAM, 0, fds);
#endif BSD
#ifdef V9
	pipe(fds);
#endif V9
	if( (pid=fork())==0 ){
		dup2(fds[0], 0);		/* Move fds[0] to 0, 1 */
		dup2(fds[0], 1);
		close(fds[0]);		/* Close duped fd */
		close(fds[1]);		/* Close host end fd */
		execlp("pads", "pads", (char *)0);
		_exit(1);
	}
	close(fds[0]);			/* Close terminal end fd */\fR
.DE
.P
Here the file descriptor, \f5fds[1]\fR, is used in the \fIhost\fR process
to communicate with the \fIterminal\fR process.
The calls to \f5dup2\fR move the communications channel to the
proper file descriptors in the \fIterminal\fR process.
.H 2 "Terminal Process Modifications"
The \fIterminal\fR process directly interacts with the window system.
The emulator library converts 5620 style graphics and I/O requests
into the primitives of the underlying window system.
In the implementation of the library, compromises had to be made
between compatibility with the 5620 code and efficiency of operation
in the new environments.
Efficiency was given preference over source code changes provided
the modifications could be easily made.
.H 3 "Preprocessor and Compiling"
Before the emulator library can be used, the file
\f5jerq.h\fR must be included in \fIterminal\fR process source files.
This file redefines many of the 5620 data structures
to ones appropriate for the window system.
All other references to 5620 related \f5#include\fR files, other
than \f5menu.h\fR, should be removed from the source.
.P
Preprocessor flags are used to compile applications for
the different environments.
The flags \f5BSD\fR and \f5V9\fR determine the version
of the Unix operating system while
\f5X11\fR and \f5SUNTOOLS\fR specify the window system.
.P
On a Sun running Sun's operating system (SunOS), the following line
can be used to create a binary for X11.
.DS I N 10
\f5cc -DBSD -DX11 -I/usr/jtools/include testfile.c \e
/usr/jtools/lib/libj.a -lX11\fR
.DE
.P
The corresponding line for \f5suntools\fR is:
.DS I N 10
\f5cc -DBSD -DSUNTOOLS -I/usr/jtools/include testfile.c \e
/usr/jtools/lib/libsj.a -lsunwindow -lpixrect\fR
.DE
.P
The libraries have been successfully tested with SunOS releases
3.0 to 3.5, and the Beta release of 4.0.
.H 3 "Initialization"
A call to the function
.DS I N 10
\f5initdisplay(argc, argv)
int argc;
char **argv;\fR
.DE
.P
should be placed in the beginning of the \fIterminal\fR process.
This call hides the details of creating a window on the screen
and initializing data structures.
\f5Argc\fR, and \f5argv\fR should be the command line arguments
passed to \f5main\fR.
\f5Initdisplay\fR searches these arguments for window system related
parameters such as the X11 geometry specification.
.P
The operations performed by \f5initdisplay\fR depend heavily on the
window system.
Under \f5suntools\fR, \f5initdisplay\fR overlays a preexisting
window on the screen obtained from the \f5WINDOW_GFX\fR
environment variable.
Thus the geometry of the tool is determined by the covered window.
If a tool using the emulator library requires a new window to run in,
\f5shelltool\fR can be called with the tool to be run as an argument.
In the \f5suntools\fR environment all tools using the emulator library
are configured with a backing store.
.P
In X11, \f5initdisplay\fR establishes a connection
to the X11 server and creates a new window.
If a geometry specification is present the window is mapped on the
screen in the appropriate position.
Otherwise \f5initdisplay\fR calls the window manager to allow the user to
determine the tool's size and position on the screen.
The X11 backing store is not used in the current
implementation because of poor performance.
.P
In X11, a tool can give size hints to the window
manager using two different functions.
A call to
.DS I N 10
\f5setsizehints(width, height, flags)
int width, height, flags;\fR
.DE
.P
can be made before \f5initdisplay\fR is called.
\f5Width\fR and \f5height\fR are the dimensions in pixels
of the default window size. 
Most X11 window managers allow the user to override default size hints
by using a different mouse button to sweep the window.
If \f5flags\fR is non zero, the X11 minimum
size hints are also set to \f5width\fR and \f5height\fR,
which in most window managers prohibits the user from making
a smaller window.
.P
Alternatively, if an application supplies the routine
.DS I N 10
\f5jerqsizehints()\fR
.DE
.P
it will be called from \f5initdisplay\fR
after the connection to the X server has been established
but before the window is created.
This allows the size hints to be computed from
the screen's physical parameters such as the pixel resolution
and dimensions.
An application provided \f5jerqsizehints\fR routine should call
\f5setsizehints\fR to set the size hints.
.P
If neither \f5jerqsizehints\fR nor \f5setsizehints\fR is called,
the size hints are set to values appropriate for a 80 column by
24 line window in the default font.
Both \f5jerqsizehints\fR and \f5setsizehints\fR are ignored in
\f5suntools\fR as the window size is predetermined by
the window that is overlaid.
.P
A call to
.DS I N 10
\f5request(resource)
int resource;\fR
.DE
.P
should be made before \f5initdisplay\fR is called to inform
the emulator library which I/O resources will be required.
\f5Resource\fR may be one or more of the bit flags
\f5ALARM\fR, \f5CPU\fR, \f5KBD\fR,
\f5MOUSE\fR, \f5RCV\fR, and \f5SEND\fR.
\f5Request\fR has several differences from the 5620 version:
.AL 1
.LI
All resources except \f5RCV\fR are implicitly requested.
.LI
\f5Request\fR must be called before \f5initdisplay\fR and can
only be called \fIonce\fR.
.LE
.P
By default \f5initdisplay\fR sets up the mouse tracking so new
mouse motion events occur only while one or more of the mouse buttons
are pressed.
This differs from the 5620 where the global
\f5mouse\fR structure is updated regardless of the state of
the mouse buttons.
Tools that need to track the mouse when the mouse buttons
are up should call the function
.DS I N 10
\f5mousemotion();\fR
.DE
.P
before the call to \f5initdisplay\fR is made.
.P
It is an error to call any of the emulator library functions
other than \f5mousemotion\fR, \f5request\fR, 
and \f5setsizehints\fR before the call to
\f5initdisplay\fR has returned.
.H 3 "Global Data Structures"
Several of the global data structures are modified
in the emulator library.
There is a newly defined structure called \f5Jproc\fR that
corresponds to the \f5Proc\fR structure in \f5mux\fR.
In the emulator library \f5Jproc\fR has only two fields:
an integer \f5state\fR that uses bit flags to store resource
information and a \f5cursor\fR pointer used in the implementation
of the function \f5cursswitch\fR.
The \f5Jproc\fR structure of an application can be accessed
through the global pointer \f5P\fR.
\f5P->state\fR may be checked to see which resources are available.
The bits and their meanings are described below:
.VL 12
.LI \f5ALARM\fR
Set when a timer started with \f5alarm\fR expires.
This bit must be cleared by the application.
.LI \f5KBD\fR
Set when keyboard input is available and
cleared automatically by \f5kbdchar\fR when the keyboard input
buffer is empty.
.LI \f5RCV\fR
Set when input is available from the \fIhost\fR process and
cleared automatically by \f5rcvchar\fR when the receive
buffer is empty.
.LI \f5RESHAPED\fR
Set when a window has been reshaped by the user
and must be redrawn.
This bit must be cleared by the application.
An application can determine the size of the new window by
examining the global \f5Rectangle Drect\fR.
In X11, this bit is also set if a damaged region of the
window is exposed.
.LE
.P
The bits \f5MOUSE\fR, \f5SEND\fR, and \f5CPU\fR are for
compatibility with the \f5wait\fR and \f5own\fR functions and
are never set in \f5P->state\fR.
.P
As on the 5620, there is a global \f5mouse\fR
structure that stores the mouse's position and button states.
Unlike the 5620, this structure is not updated
asynchronously by the operating system.
Mouse position and button updates are hidden in I/O requests.
.P
The global \f5Rectangle\fRs \f5Drect\fR
and \f5display.rect\fR store the dimensions of a tool's window.
In the emulator library the upper left hand corner
of the display area of a process is the coordinate system origin.
Thus the \f5origin\fRs of \f5Drect\fR and \f5display.rect\fR
are always set to the point 0,0.
On the 5620, these \f5Rectangle\fRs are in screen coordinates.
.P
There are also several new global data structures that allow
applications to directly call routines in the underlying
window system.
These can be used to do operations that are supported in the
window systems but not in the emulator library,
such as drawing lines of different width.
Code calling these low level functions should be enclosed
in preprocessor conditional defines.
.P
If compiled for X11, the following globals can be accessed
.DS I N 10
\f5GC		gc;		/* X11 graphics context */
Display	*dpy;		/* X11 display */
int		fgpix, bgpix;
Colormap	colormap;
XColor	fgcolor, bgcolor;\fR
.DE
.P
while compiling for \f5suntools\fR allows access to
.DS I N 10
\f5Pixwin	*displaypw;\fR
.DE
.P
For example, to draw a dashed line that is 3 pixels wide in
X11, the following code can be used:
.DS I N 10
\f5#ifdef X11
  XSetLineAttributes(dpy, gc, 3, LineOnOffDash, CapNotLast, JoinMiter);
  segment(&display,Drect.origin,Drect.corner,F_XOR);
#endif X11\fR
.DE
.P
The window system manuals should be consulted for
more information on using these data structures\*(Rf
.RS
Jim Gettys, Ron Newman, Robert Scheifler,
\fIXlib - C Language X Interface Protocol Version 11\fR,
X Release X.V11R1, 1987.
.RF
\*(Rf.
.RS
\fISunView Programmers Guide\fR,
Sun Microsystems, Inc.,
Part No:800-134502, 1986
.RF
.H 3 "Graphics"
For efficiency reasons cursors, \f5Texture\fRs, and \f5Bitmap\fRs
are stored in a format that is compatible with the underlying window system.
Three new routines must be used to initialize these
structures from bit patterns.
The conversions can only be made after \f5initdisplay\fR
has returned.
.P
A \f5Texture\fR on the 5620 is an array of
16 shorts representing a 16x16 bit pattern.
Initialization is done by filling the array.
For example, the following code defines a grey \f5Texture\fR.
.DS I N 10
\f5Texture grey={
	0xDDDD, 0x7777, 0xDDDD, 0x7777, 0xDDDD, 0x7777, 0xDDDD, 0x7777,
	0xDDDD, 0x7777, 0xDDDD, 0x7777, 0xDDDD, 0x7777, 0xDDDD, 0x7777,
};\fR
.DE
.P
In the emulator library \f5Texture\fRs are more complex objects.
For example, a \f5Texture\fR in the X11 implementation is a handle
to a remote \f5Pixmap\fR data structure maintained by the server.
To create a \f5Texture\fR it is necessary call the function
\f5ToTexture\fR.
The code corresponding to the initialization of \f5grey\fR is show below.
.DS I N 10
\f5short grey_bits[]={
	0xDDDD, 0x7777, 0xDDDD, 0x7777, 0xDDDD, 0x7777, 0xDDDD, 0x7777,
	0xDDDD, 0x7777, 0xDDDD, 0x7777, 0xDDDD, 0x7777, 0xDDDD, 0x7777,
};
Texture grey;

grey = ToTexture(grey_bits);\fR
.DE
.P
There is a similar function, called \f5ToTexture32\fR to convert
32x32 bit \f5Texture32\fR objects.
.P
On the 5620, cursors are \f5Texture\fRs.
In the emulator library a new object called \f5Cursor\fR is defined.
This was necessary because both \f5suntools\fR and X11 represent
cursors as special objects.
A bit pattern is converted to a \f5Cursor\fR in the emulator library
with the function \f5ToCursor\fR.
For example, the cursor \f5crossHairs\fR is initialized on the 5620
with the following code.
.DS I N 10
\f5Texture crossHairs = {
  0x100,0x100,0x100,0x100,0x100,0x100,0x0,0xFC7E,
  0x0,0x100,0x100,0x100,0x100,0x100,0x100,0x0,
};\fR
.DE
.P
The corresponding code in the emulator is:
.DS I N 10
\f5short crossHairs_bits[] = {
  0x100,0x100,0x100,0x100,0x100,0x100,0x0,0xFC7E,
  0x0,0x100,0x100,0x100,0x100,0x100,0x100,0x0,
};
Cursor crossHairs;

crossHairs = ToCursor(crossHairs_bits, crossHairs_bits, 7, 7);\fR
.DE
.P
The first argument to \f5ToCursor\fR is a 16x16 bit pattern
defining the cursor.
On the 5620 and in \f5suntools\fR, these bits are exclusive-ored on the screen.
In X11, these bits are set to the foreground color.
The second argument is another 16x16 bit pattern that is ignored in
\f5suntools\fR.
These bits are used as a mask in X11.
Bits set in both the first and second bit patterns are set to the
foreground color.
Bits set in the second bit pattern, but not the first, are set to the
background color.
Bits not set in the second bit pattern are not changed.
If the same bit pattern is given for both arguments, as is done above,
the cursor will not be visible in foreground colored regions of the screen.
The last two arguments are the x and y offset of the cursor's hot spot
relative to the upper left hand corner of the cursor and can be used
to set the cursor's active point.
This is useful if the cursor is not symmetrical.
.P
Initialized \f5Bitmap\fRs also require code changes.
For example, the following piece of 5620 code creates a 16x16
\f5Bitmap\fR from the bits defined in \f5starbits\fR.
.DS I N 10
\f5short starbits[]={
	0x07E0,0x0,0x1F08,0x0,0x0,0x0,0x7FFE,0x0,
	0x3FC2,0x0,0x0,0x0,0xFFFF,0x0,0x7FC1,0x0,
	0x0,0x0,0xFFFF,0x0,0x1F01,0x0,0x0,0x0,
	0x7FFE,0x0,0x0,0x0,0x1008,0x0,0x07E0,0x0,
};
Bitmap stardwg={(Word *)starbits, 1, 0, 0, 16, 16};\fR
.DE
.P
\f5Bitmap\fR scan lines are word aligned (4 byte) on the 5620,
so the odd shorts must be present in \f5starbits\fR
even though they are not used.
The 1 in the initialization of \f5stardwg\fR determines the number
of words in a scan line and the last four numbers define the
\f5Bitmap\fR's \f5Rectangle\fR.
.P
Assuming the same definition of \f5startbits\fR, the corresponding piece
of code for use in the emulator is:
.DS I N 10
\f5Bitmap stardwg;

stardwg=ToBitmap((char *)starbits, 4, 0, 0, 16, 16);\fR
.DE
.P
The arguments to the function \f5ToBitmap\fR correspond to the
initialized data of a \f5Bitmap\fR on the 5620 except
the length of a scan line is in bytes instead of words.
Word alignment of the bit patterns is not necessary in the emulator
library.
.P
Some 5620 programs use the function \f5addr\fR to directly access
the pixels in a \f5Bitmap\fR.
\f5Addr\fR returns the address of the \f5Word\fR containing the bit
of a given point in a \f5Bitmap\fR.
This function is not implemented in the emulator library as the
address may not be accessible.
For example, in X11 \f5Bitmap\fRs are maintained by the server.
Code calling \f5addr\fR will have to be modified to use function
calls to access pixels.
The function \f5point\fR can be used to set individual pixels,
and a new function
.DS I N 10
\f5int getpoint(b, p)
Bitmap b;
Point p;\fR
.DE
can be used to read back the current value of a pixel.
.P
The constants that define the dimensions of the screen in pixels,
\f5XMAX\fR and \f5YMAX\fR, are undefined in the emulator library.
Applications using these constants
should substitute \f5Drect.corner.x\fR and \f5Drect.corner.y\fR instead.
In the emulator library these values contain the width and
height of the window that the application controls.
Since \f5Drect\fR does not contain valid data until
after \f5initdisplay\fR has returned, initialized data
derived from \f5XMAX\fR and \f5YMAX\fR should be changed
to assignment statements that are called after \f5initdisplay\fR.
.H 3 "Fonts"
The emulator library uses the native fonts of the underlying window system.
The \f5Font\fR structure is redefined by the preprocessor
to correspond to a handle appropriate for the window system:
a \f5XFontStruct\fR in X11 and a \f5Pixfont\fR pointer in
\f5suntools\fR.
.P
Code accessing fields in the \f5Font\fR structure
will have to be modified.
Three macros, \f5fontheight\fR, \f5fontwidth\fR,
and \f5fontnchars\fR take a \f5Font\fR pointer
as an argument and return the height of a character in pixels,
width of a character in pixels, and number of characters in the
font respectively.
The macros only work correctly on fixed-width fonts.
.P
To open a \f5Font\fR
.DS I N 10
\f5Font getfont(fontname)
char *fontname;\fR
.DE
.P
is used.
Appropriate \f5fontname\fRs can be determined with
\f5xlsfonts\fR for X11 and by examining the directory
\f5/usr/lib/fonts/fixedwidthfonts\fR for \f5suntools\fR.
.H 3 "I/O"
The handling of input in the emulator library is hidden
in the routine
.DS I N 10
\f5int wait(resource)
int resource;\fR
.DE
.P
where \f5resource\fR is a bit vector indicating which resources
to wait for.
Like the 5620 version, it returns a bit vector indicating
which resources are available.
The emulator library uses the \f5wait\fR routine to hide many
window system dependencies, such as reading and parsing of
mouse and keyboard events,
flushing the X11 output buffer,
and restoring damaged areas of the screen from the
backing-store in \f5suntools\fR.
Keyboard events are queued in a buffer while mouse events are
used to update the global \f5mouse\fR structure.
.P
\f5Wait\fRing for the \f5KBD\fR, \f5RCV\fR, or \f5ALARM\fR
resources has the same behavior in the emulator library
and 5620 environments.
The \f5CPU\fR and \f5MOUSE\fR resources behave differently.
On the 5620, \f5wait(CPU)\fR gives up the processor so other
processes on the terminal can run.
In the emulator library, the \fIterminal\fR process is a
Unix process, so scheduling is determined by the Unix kernel.
A \f5wait(CPU)\fR call in the emulator determines if
any new input has arrived and processes the results appropriately
without blocking.
\f5Wait(CPU)\fR is the only way to update the keyboard queue, receive
queue, and \f5mouse\fR structure without the possibility of blocking.
.P
\f5Wait\fRing for the \f5MOUSE\fR on the 5620
blocks the process until it has control of the mouse.
In the emulator library it blocks until any new window event is
received.
Typical events are mouse button hits, button releases,
keyboard input, and motion of the mouse.
.P
The function
.DS I N 10
\f5nap(t)
int t;\fR
.DE
.P
busy loops for \f5t\fR ticks of a 60 hertz
clock on the 5620.
The pseudo-code below shows how it is typically used to avoid screen
flicker while tracking the mouse.
.DS I N 10
\f5for(; button3(); nap(2)) {
	/* If mouse moved, redraw object */
}\fR
.DE
.P
\f5Nap\fR is equivalent to \f5wait(MOUSE)\fR in the emulator library
so applications using it for timing should call \f5sleep\fR instead.
.P
In the emulator library, care must be taken to avoid infinite loops
that wait for mouse changes without calling \f5wait\fR or \f5nap\fR.
For example, the following piece of code works correctly on
the 5620 since the \f5mouse\fR structure is updated asynchronously
but loops indefinitely in the emulator library:
.DS I N 10
\f5while(button2());\fR
.DE
.P
It can be corrected with the addition of a call to \f5wait\fR.
.DS I N 10
\f5while(button2()) wait(MOUSE);\fR
.DE
.P
The function \f5own\fR can be used to determine which resources
are available.
Unlike the 5620, \f5own()&MOUSE\fR is always true.
.P
The functions \f5sendchar\fR and \f5sendnchars\fR are used to
send data to the \fIhost\fR process.
In the emulator library, these procedures are implemented with
non-blocking \f5write\fRs to avoid the possibility of the
\fIterminal\fR and \fIhost\fR processes becoming deadlocked.
An application may redefine these routines to override
the default implementation.
For example, in the Ninth Edition Unix implementation of some terminal
emulators it is desirable to push line disciplines that require formatted
messages on the communications channel between the
\fIterminal\fR process and \fIhost\fR shell.
These line disciplines allow \f5ioctl\fR calls by Unix programs
to be caught and interpreted in the \fIterminal\fR process.
.P
Similarly, the handling of input from the \fIhost\fR
process can be specialized.
When \f5wait\fR detects input from
the \fIhost\fR process on file descriptor 0, the function
.DS I N 10
\f5rcvfill()\fR
.DE
.P
is called.
The default \f5rcvfill\fR routine \f5read\fRs the data
and places it in the \f5RCV\fR queue with no intermediate processing.
An application can provide its own \f5rcvfill\fR routine
if the data must be decoded before it is queued.
An application supplied \f5rcvfill\fR routine should \f5read\fR
the data available from the \fIhost\fR process, decode
the message, and place the results in the receive queue
with the routine
.DS I N 10
\f5rcvbfill(buf, cnt)
char *buf;
int cnt;\fR
.DE
.P
\f5Rcvbfill\fR copies \f5cnt\fR bytes of data starting at \f5buf\fR
into the receive queue and sets the \f5RCV\fR flag in \f5P->state\fR.
The routine \f5rcvchar\fR retrieves the data
from the receive queue.
.P
Programs accessing files can use the standard I/O library.
Care should be taken not to block while \f5read\fRing device
file descriptors or the emulator library may not work correctly.
For example, if the X11 server blocks while attempting to
send events to a client for more than several seconds,
it will close down the connection.
.H 3 "Timers"
The 5620 timing functions, \f5sleep\fR, \f5alarm\fR and
\f5realtime\fR are supported in the emulator library.
\f5Sleep\fR and \f5alarm\fR take a single integer argument
containing the interval in 60 hertz ticks.
The amount of time before \f5sleep\fR returns or the
\f5alarm\fR goes off depends on the Unix scheduler and the
frequency of the Unix clock.
It is rounded up to the best approximation.
The \f5sleep\fR and \f5alarm\fR timers are independent of one
another and may run concurrently.
\f5Sleep\fR will continue to \f5read\fR and queue input while
the process is delayed.
.H 3 "Exchange Buffer"
On the 5620, two independent tools may exchange
strings by placing them in a global buffer maintained by \f5mux\fR.
In the emulator library, two new functions have been added
to provide a similar facility.
.DS I N 10
\f5typedef struct String {
	char *s;	/* pointer to string */
	short n;	/* number used, no terminal null */
	short size;	/* size of allocated area */
} String;

getmuxbuf (str)
String *str;

setmuxbuf(str)
String *str;\fR
.DE
.P
The \f5String\fR structure is used in calls to set and retrieve
the global buffer.
It is the same structure used in the implementation of the global
buffer in \f5mux\fR.
\f5S\fR points to a string obtained with \f5gcalloc\fR.
\f5Size\fR is the length of the allocated string and \f5n\fR is
the number of bytes that contain valid data.
.P
\f5Setmuxbuf\fR places \f5str->n\fR bytes of the string
\f5str->s\fR in the global buffer.
\f5Getmuxbuf\fR retrieves a string from the global buffer.
If the area pointed to by \f5str->s\fR is not large enough,
the region will be freed with \f5gcfree\fR and a new string
of appropriate size allocated with \f5gcalloc\fR.
The number of valid characters in the returned string is placed
in \f5str->n\fR.
.P
In X11, the global buffer is implemented with the X cut and
paste buffers making it possible for tools to exchange strings
with other X11 tools.
The \f5suntools\fR implementation uses a temporary file
that is incompatible with the tools distributed by Sun.
The emulator library does not use the \f5Sunview\fR Notifier
making compatibility with the \f5suntools\fR exchange buffer
extremely difficult.
.H 3 "Efficiency Considerations"
Code making repetitive graphics calls
may run inefficiently in the emulator library.
This is particularly noticeable when complex objects
are constructed by writing individual pixels with
the function \f5point\fR.
To improve performance, three new functions
may be used when repetitive calls to \f5point\fR
operate on a \f5Bitmap\fR with the same drawing \f5Code\fR.
.DS I N 10
\f5initpoints(b, f)
Bitmap *b;
Code f;

points(p)
Point p;

endpoints()\fR
.DE
.P
The function \f5points\fR, should be substituted for each call to
\f5point\fR in the repetitive sequence.
The sequence is then enclosed between calls to
\f5initpoints\fR and \f5endpoints\fR.
.P
In X11, \f5Points\fR is implemented with the X11 \f5XDrawPoints\fR operator
which significantly decreases the traffic between the client
and server when compared with multiple \f5XDrawPoint\fR calls.
In \f5suntools\fR, \f5initpoints\fR locks the window with a semaphore.
This lock avoids two system calls per subsequent graphics call
and significantly speeds operation.
Enclosing any long sequence of consecutive graphics
operations between an \f5initpoints\fR and \f5endpoints\fR
usually leads to performance improvements in \f5suntools\fR.
.P
As mentioned in the \fIGlobal Data Structures\fR section
it is possible to call the primitives of
the underlying window system.
For many complex operations that are not supported by
the emulator library such as filling polygons, this can
lead to significant performance improvements.
.H 3 "Name Space Conflicts"
Several of the 5620 primitives have the same names as system calls
or library subroutines in the Unix operating system.
To allow applications the possibility of accessing the Unix
routines, the file \f5jerq.h\fR uses the preprocessor to
alter the subroutine names used in the emulator library.
The redefined routines are listed below:
.DS I N 10
\f5#define nap(x)	jnap(x)
#define wait(x)	jwait(x)
#define sleep(x)	Jsleep(x)
#define alarm(x)	Jalarm(x)\fR
.DE
.P
An application desiring access to the Unix routines
of the same name should use the preprocessor \f5#undef\fR
directive to remove the mappings.
.H 3 "Debugging"
The different environments provided by the emulator library
and 5620 lead to several common program bugs
related to the handling of input.
On the 5620 the operating system asynchronously
updates input related data structures such as the global
\f5mouse\fR structure and the keyboard and receive queues.
In the emulator, updates to input data structures are
hidden in input routines such as \f5wait\fR, \f5nap\fR and
\f5sleep\fR.
Programs that poll input data structures without intervening
calls to the input routines will work correctly on
the 5620 but loop indefinitely in the emulator.
.P
The emulator library also hides other window system
operations in the input routines, such as
restoring damaged screen areas from the backing-store,
on the assumption they will be frequently called.
If an application does not call the input routines,
a damaged window will not be repaired.
.P
The buffering of graphics calls in X11 may also
lead to debugging peculiarities.
Graphics requests are not received by the server until the
X11 output buffer is flushed.
By default, this does not occur until either a X11 call requires
a response from the server, or an emulator library
input routine such as \f5wait\fR or \f5nap\fR is called.
The buffering of X11 graphics requests can be bypassed by
setting the global integer variable, \f5_XDebug\fR, to a non-zero
value before the call to \f5initdisplay\fR is made.
While applications may run up to an order of magnitude slower
without buffering, the output from graphics calls can be seen
immediately when single stepped under the control of
a debugger.
.H 3 "Startup"
Since the window systems use different methods
to create new windows it is convenient to use a single
shell file to invoke the tools.
The following file can be used to start the tool
\f5cip\fR for both X11 and \f5suntools\fR:
.DS I N 10
\f5#!/bin/sh
# If not running under suntools, assume X11
DIR=/usr/jtools
if [ ! "$WINDOW_ME" ]
then
    exec $DIR/x3bin/cip $*
else
    exec shelltool -Wl cip -Ws 591 802 -Wp 0 80 $DIR/s3bin/cip $*
fi\fR
.DE
.P
This file assumes that X11 binaries are in \f5/usr/jtools/x3bin\fR
and \f5suntools\fR binaries are in \f5/usr/jtools/s3bin\fR.
If the \f5WINDOW_ME\fR environment variable is set the file assumes
\f5suntools\fR is running and \f5shelltool\fR is called with
the \f5suntools cip\fR binary as an argument.
Otherwise it assumes that X11 is running and invokes
the X11 binary.
With an additional test it is possible to use the
same shell file to select appropriate binaries depending on the
machine's architecture.
.H 3 "Running Emulator Code on the 5620"
Most 5620 graphics code that is modified to run with
the emulator library can also run
on the 5620 provided the preprocessor
maps the newly defined structures and functions back
into the original 5620 primitives.
The following defines cover a major percentage of the
source code changes while the other emulator routines
that are not 5620 primitives
can be implemented in short subroutines.
.DS I N 10
\f5#define Cursor			Texture
#define ToCursor(a,b,c,d)	(*(Texture *)(a))
#define ToTexture(t)		(*(Texture *)(t))
#define ToTexture32(t)		(*(Texture32 *)(t))
#define fontheight(f)		((f)->height)
#define fontwidth(f)		((f)->info->width)
#define fontnchars(f)		((f)->n)
#define initdisplay(c,v)\fR
.DE
.H 2 "Porting Overview"
The following list summarizes how to port an application.
It can be used as a step by step guide for most simple 5620 tools.
.AL 1
.LI
If the application requires both a \fIhost\fR and
\fIterminal\fR process,
the \fIhost\fR process must be connected to the \fIterminal\fR
process with a bidirectional socket or pipe.
If the only purpose of the \fIhost\fR process is file access and
data compression, it can probably be eliminated.
.LI
Include the file \f5jerq.h\fR at the top of the \fIterminal\fR
source file.
If there are references to other 5620 related \f5#include\fR
files other than \f5menu.h\fR, remove them.
.LI
Add the subroutine call \f5initdisplay(argc, argv)\fR early in
the program.
This must be called before any graphics operations are performed
or any conversions of the \f5Cursors\fR, \f5Texture\fRs and initialized
\f5Bitmap\fRs are made.
If \f5main\fR does not have \f5argc\fR and \f5argv\fR arguments, add them.
.LI
Move the \f5request\fR call before the call to \f5initdisplay\fR.
.LI
\f5Cursors\fR, \f5Texture\fRs and initialized \f5Bitmap\fRs must be
converted to the appropriate structures using the functions
\f5ToCursor\fR, \f5ToTexture\fR, and \f5ToBitmap\fR respectively.
Make new declarations for the object and the bits that make
up the contents of the object.
.LI
If the constants \f5XMAX\fR and \f5YMAX\fR are used by the program,
\f5Drect.corner.x\fR and \f5Drect.corner.y\fR should
be substituted respectively.
.LI
If the members of \f5Font\fR structures are examined, it is most likely
to determine the width, height, or number of characters in the font.
The macros \f5fontwidth(fp)\fR, \f5fontheight(fp)\fR,
and \f5fontnchars(fp)\fR should be substituted respectively.
.LI
If \f5addr\fR is called, an appropriate combination of
\f5point\fR and \f5getpoint\fR should be substituted.
.LI
Compile the program with appropriate flags set for the desired run-time
environment and link it with the appropriate libraries.
.LE
.H 2 "Sample Program"
Below is a complete program that uses the emulator library.
It demonstrates some simple graphics functions, mouse tracking,
and the handling of keyboard input.
.DS I N 10
\f5#include "jerq.h"

main (argc, argv)
int argc;
char **argv;
{
   int r;

   request(KBD|MOUSE);
   initdisplay(argc, argv);
   for(;;){
      r = wait(KBD|MOUSE);
      if(button1()){
         rectf(&display, Drect, F_CLR);
         rectf(&display, Rect(Drect.origin.x, Drect.origin.y,
               mouse.xy.x, Drect.corner.y), F_XOR);
      } else if(button23())
         break;
      if(r&KBD && kbdchar() == 'y')
         string(&defont, "Hello", &display, Drect.origin, F_XOR);
   }
}\fR
.DE
.P
The calls to \f5request\fR and \f5initdisplay\fR initialize
the window.
When button 1 is down, the window is cleared and the section
of the window to the left of the mouse is inverted.
The program exits when mouse button 2 or 3 is pressed.
If the character y is typed on the keyboard, the string
"Hello" is toggled in the top left corner of the window.
.H 1 "Comments"
.H 2 "Implementation"
Most of the 5620 graphics primitives were easily mapped into the
primitives of X11 and \f5suntools\fR.
Support of the off screen bit-maps required a little tweaking.
Since X11 and \f5suntools\fR don't allow off screen bit-maps
to have origin offsets,
a flag was added to the \f5Bitmap\fR structure to distinguish
screen from memory bit-maps.
When the flag indicates a memory bit-map, functions in
the emulator library transform the bit-map coordinates appropriately.
In the \f5suntools\fR implementation, this flag is also used to
call different low-level graphics primitives.
Screen bit-maps use Sun's \f5pixwin\fR interface
while memory bit-maps use the \f5pixrect\fR interface.
.P
The most difficult part of the emulator library was
efficient construction of the I/O handlers hidden in the
\f5wait\fR routine.
The notification-based I/O mechanisms provided by the X toolkit
and \f5SunView\fR do not map cleanly into the
I/O primitives used by the 5620 tools.
In the \f5suntools\fR implementation the emulator library
handles window events from the operating system
and does not make any use of the \f5SunView\fR notifier.
Similarly, the X11 implementation directly services the X events.
While decoding the window events is tedious in both environments,
the code is more efficient and compact than it
would have been had the higher level window interfaces been used.
.P
Creation of a window in both environments requires a substantial
amount of code.
The \f5initdisplay\fR routine required 126 lines of C source 
for X11 and 60 lines for \f5suntools\fR.
In X11, approximately one third of the initialization code
manages interactions between the tool and the window manager.
These routines may be useful to application programmers 
who need to create a window in the different window
environments but do not want to use the emulator library.
.H 2 "Window Systems"
Experience with the three window systems has led
to some observations.
.P
The X11 protocol provided a rich interface to
implement the emulation.
Having the graphics server separate from the applications
has both pros and cons.
On the positive side binaries are small and all tools can
be executed on remote processors.
However, use of a communications channel for graphics has
substantial overhead.
While this is minimized by buffering output,
delays are particularly noticeable for functions that require
responses from the server.
Reading the value of a pixel back from the screen
requires at least two context switches and four system calls.
.P
In it's current state on the Sun, tools using the emulator
library with X11 perform sluggishly when compared with \f5suntools\fR.
As the X11 server code matures and becomes better tuned
to the hardware, performance should improve.
.P
Unlike X11, it was difficult to directly
handle I/O in the \f5suntools\fR environment.
While the low level handling of I/O is remarkably similar to
the event mechanism of X11, it is poorly documented.
Many of the window system features, such as the maintenance
of the border, window layout menus, and cut and paste buffers
depend on the \f5SunView\fR Notifier.
Providing the standard \f5suntools\fR user interface for
operations such as labeling, reshaping, hiding, and moving windows
requires an additional Unix process for every tool using
the emulator library.
.P
The \f5suntools\fR environment suffers from excessive generality.
For example, the cut and paste buffers can be implemented
with temporary files in a few of lines of code.
Sun instead opted for a much more general mechanism based on
a dynamically binding interprocess communication mechanism.
It depends heavily on the Notifier and requires many pages
of code to implement.
It appears to only be used in the implementation of the cut
and paste buffers.
.P
This philosophy has led to another \f5suntools\fR problem:
huge binaries.
The smallest graphics program generated, even without pulling
in the Notifier code, requires over 230 kilobytes of text space.
Shared libraries in the next release of Sun's operating system
software will alleviate the problem,
but this wouldn't be required if the libraries
were more prudently designed.
.P
The 5620 environment suffers from different problems.
The low-bandwidth serial connection has caused many of the
applications to be more complicated than necessary.
Care must be taken by tool designers to insure that the data
traffic between the \fIterminal\fR and \fIhost\fR processes
does not overload the connection.
A fair amount of the effort in designing a tool for the 5620
is in properly segmenting the work to be done on the different
processors.
.P
A properly segmented tool does have its benefits.
The separation of the display processor from the Unix engine
avoids sluggish mouse behavior when the Unix processor
is busy.
Even when run on the same processor, separation of the tool
into two processes is desirable since the \fIterminal\fR
process can respond to user input while the \fIhost\fR process
is busy.
From a user's perspective, it is reassuring for menus to
continue to pop up while a complicated operation is in progress.
.H 1 "Conclusions"
Using the emulator library, it is possible to port
applications written for the 5620 to both X11 and \f5suntools\fR.
Ported tools perform well in both X11 and \f5suntools\fR
with only minor source code changes.
Tools intended for the 5620 may now be
used on a wide variety of hardware. 
.P
The library can also be used as an educational tool
for comparing the different window systems.
An application programmer familiar with one environment
can quickly find corresponding functionality in the other
two environments.
.SG
.rs
.sp -1v
MH-11228/11229-DAK/JIH-dak/jih
.NS 3
References (1-6)
.NE
