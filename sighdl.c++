/*
Access Gateway - a mediator for the World Wide Web

(C) 1998-2006 Silas S. Brown (St John's College,
Cambridge, UK, http://www.cus.cam.ac.uk/~ssb22 )

This is not intended to be model or example code,
but it is intended to be secure enough to run on a
Web server without compromising it.

If you would like to email me a patch, please first
ensure that you have the latest release, and use
"diff -Naur old-directory new-directory", where
"old-directory" contains the unmodified release
version and "new-directory" contains the
modifications.  Please also include a brief
description of what the patch achieves, your name
(for an acknowledgement), and any copyright
restrictions (ideally the patch should be public
domain).  I may not apply every patch but I do try to
answer my email, unless I am inundated, busy, or
offline for a long time.

This program is free software; you can redistribute it
and/or modify it under the terms of the GNU General
Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your
option) any later version.

This program is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General
Public License along with this program; see the file
COPYING.  If not, write to the Free Software Foundation,
Inc., 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.

*/

#ifndef ALLINC_H
#define ALLINC_H
#include "allinc.h"
#endif
// This "allinc.h" fragment is also in "notice.txt" (copied
// to top of every file on release), just in case forget to
// put it in some file
// Also, need to put this HERE (not in include file) for
// Monash:
#ifdef __GNUC__
#if (__GNUC__ > 2)
using namespace std;
#endif
#else
// Unknown compiler - assume it's standard
using namespace std;
#endif

// end of notice.txt
#include "allinc.h"
#include "platform.h"
#ifdef USE_POSIX_FORK
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "webget.h"
#include "debug.h"

#ifndef MAX_RUNTIME_SECONDS
#define MAX_RUNTIME_SECONDS 300
#endif

#ifdef Have_Checkpoints
char checkpointBuf[1024]="No checkpoint";
#endif

volatile sig_atomic_t fatalErrorInProgress=0;
int child_pid;
void fatalError(int sig) {
if(!fatalErrorInProgress) {
// (and I hope there isn't another signal at THIS point, but the consequences wouldn't be too bad)
fatalErrorInProgress=1;
if(child_pid) kill(child_pid,SIGKILL);
removeTempFiles();
printf("Content-type: text/html\n\n<HTML><BODY>Web access gateway: Exitting on signal %d"
#ifdef Have_Checkpoints
"<BR>Last checkpoint: %s" // %s OK
#endif
"</BODY></HTML>",sig
#ifdef Have_Checkpoints
,checkpointBuf
#endif
);
exit(1);
}
}

void setUpSignalHandlers() {
// Fatal program errors
signal(SIGFPE,  fatalError);
signal(SIGILL,  fatalError);
signal(SIGSEGV, fatalError);
signal(SIGBUS,  fatalError);
signal(SIGTRAP, fatalError);
// Sysadmin interrupts
signal(SIGTERM, fatalError);
signal(SIGINT,  fatalError);
signal(SIGQUIT, fatalError);
signal(SIGHUP,  fatalError);
// Don't exit on SIGPIPE (if someone disconnects when
// there is still data to write) - might be a remote
// web server problem
signal(SIGPIPE,SIG_IGN);
// Maximum running time:
signal(SIGALRM, fatalError);
alarm(MAX_RUNTIME_SECONDS);
}
#endif
