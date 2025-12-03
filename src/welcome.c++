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
