/*
Access Gateway - a mediator for the World Wide Web

(C) 1998-2006 Silas S. Brown http://ssb22.user.srcf.net

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

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
