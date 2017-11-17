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
#include "welcome.h"
#include "lastupd.h"
#include "webget.h"
#include "action.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>

int isFromOutside() {
const char* x=getenv("HTTP_REFERER");
if(x && strlen(x)) {
if(strstr(x,CGI_LOCATION)) return 0; // probably
#ifdef Is_Monash
if(strstr(x,"www.csse.monash.edu.au")) return 0;
if(strstr(x,"ssb22/chinese.html")) return 0;
#endif
// (can't check IP_ADDRESS because it might be IP or
// DNS or DNS aliases or ...)
else return 1; // definitely outside
} else return 0; // in case doesn't send referer
}

void printUnwelcomeMessage(CGIEnvironment* e) {
e->h_puts(L_DEEP_LINKING);
}

//void printRobotBarrier(CGIEnvironment* e) {
void printWelcomeMessage(CGIEnvironment* e) {
if(!preprocessOnly(e)) {
//e->h_puts(L_REFERER_PROBLEM);
//e->h_puts(MakeRelativeActionURL("<FORM ACTION=\"","\" METHOD=\"GET\">"));
//e->formDumpCurrentOptions("");
//e->h_puts("<CENTER><LABEL FOR=\"sub\"><INPUT TYPE=\"Submit\" ID=\"sub\" VALUE=\"" L_REFERER_CONTINUE "\"></LABEL></CENTER></FORM>");

//e->h_puts(L_WELCOME);
//outputLastUpdate(e);
// Don't do that - see comments in main.c++

e->h_fputs("<SCRIPT LANGUAGE=\"JavaScript\"><!--\nwindow.status=\"");
e->h_fputs(L_WELCOME);
e->h_puts("\";\n// --></SCRIPT>");
}
}
