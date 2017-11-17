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
#include "localusr.h"
#ifdef LOCAL_DNS_SUFFIX
#ifndef LOCAL_IP_PREFIX
#define LOCAL_IP_PREFIX
#endif
#endif
#ifdef LOCAL_IP_PREFIX
#include "webget.h"
#include "literals.h"
#include "platform.h"
#include "strfunc.h"
#include "lastupd.h"
#include "action.h"
#include "options.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NO_EXCEPTION_ON_REMOTE_HOST  /* NEEDATTENTION Is is OK to put this here? - ie. will it work on all web servers? */

#ifdef STR_FUNCTIONS_NEED_IMPLEMENTING
char* strchr (const char* szSearch, int cFor);
#endif

int userIsLocal(const CGIEnvironment* theEnvironment) {
#ifdef NONLOCAL_PASSWORD
const char* passwd=theEnvironment->getName(ENV_NONLOCAL_PASSWORD,MAY_RETURN_NULL);
if(passwd && !strcmp(passwd,NONLOCAL_PASSWORD)) return 1;
#endif
// non-local users still allowed to post their own text
// (e.g. Chinese email), since that doesn't involve
// getting remote pages:
if(theEnvironment->getName(USER_TEXT_ENV,MAY_RETURN_NULL)) return 1;
#ifdef LOCAL_DNS_SUFFIX
const char* x=getenv("REMOTE_HOST");
if(!x)
#ifdef NO_EXCEPTION_ON_REMOTE_HOST
return(0);
#else
throw "userIsLocal(): Unable to get REMOTE_HOST from environment";
#endif
#else
const char* x=getenv("REMOTE_ADDR");
if(!x) x=getenv("REMOTE_HOST");
if(!x) throw "userIsLocal(): Unable to get REMOTE_ADDR or REMOTE_HOST from environment";
#endif	
const char* prefixComparison[]=
#ifdef LOCAL_DNS_SUFFIX
LOCAL_DNS_SUFFIX;
#else
LOCAL_IP_PREFIX;
#endif
for(int lp=0; prefixComparison[lp]; lp++) {
#ifdef LOCAL_DNS_SUFFIX
int tmp=strlen(prefixComparison[lp]),t2=strlen(x); // Bit of optimisation
if(t2>=tmp && !strncmp(x+(t2-tmp),prefixComparison[lp],tmp))
#else
if(!strncmp(x,prefixComparison[lp],strlen(prefixComparison[lp])))
#endif
return(1);
} return(0);
}

#ifdef PROXY_FOR_NONLOCALS
const char* nonLocalProxyOverride=NULL;
#endif

int dealWithUnlocalUser(CGIEnvironment* theEnvironment) {
#ifdef PROXY_FOR_NONLOCALS
nonLocalProxyOverride=PROXY_FOR_NONLOCALS;
#endif
// First see if can re-direct to another alias (in case
// of proxy problems e.g. ucam.org not being recognised
// as local to JANET).  Not for POST requests as not all
// browsers re-submit them (and re-writing them as GET
// may be awkward especially if they're multipart).
#ifdef REDIRECT_NONLOCAL_IF_HTTP_HOST_NOT_MATCH_IP
const char* host=getenv("HTTP_HOST");
if(host && stricmp(host,IP_ADDRESS)
&& !(theEnvironment->getName(FORM_NEEDS_POSTING,MAY_RETURN_NULL))) {
// There is a mismatch; output a re-direction
#ifndef BRIEF_RESPONSE
// Oh dear, this is hacky
puts("HTTP/1.0 200 OK");
#endif
InString s;
theEnvironment->urlDumpOptionsForRedirect(s);
printf(MakeActionURL("Location: ","?%s\n"),s.getString()); // %s OK here
#ifndef BRIEF_RESPONSE
puts(""); // NEEDATTENTION Need something better than
// that - no point putting "rubbish" after the response
#endif
}
#ifdef PROXY_FOR_NONLOCALS
else
#endif
#endif // REDIRECT_NONLOCAL_IF_HTTP_HOST_NOT_MATCH_IP
#ifdef PROXY_FOR_NONLOCALS
return 1; // OK to use the gateway
// (we'll use that proxy)
#else
// Output a message
#ifdef DEVELOPMENT_COPY
if(theEnvironment->getName(ENV_LANGUAGE_TABLE,MAY_RETURN_NULL) &&
!stricmp(theEnvironment->getName(ENV_LANGUAGE_TABLE,MAY_NOT_RETURN_NULL),"Japanese"))
theEnvironment->h_fputs(JPN_NONLOCAL);
#endif
theEnvironment->outHTMLParagraph(MESSAGE_FOR_NON_LOCAL_USERS);
const char* x=getenv("REMOTE_HOST"); if(x) {
theEnvironment->h_fputs("Host: "); theEnvironment->h_fputs(x); theEnvironment->h_puts("<BR>");
} x=getenv("REMOTE_ADDR"); if(x) {
theEnvironment->h_fputs("IP address: "); theEnvironment->h_fputs(x); theEnvironment->h_puts("<BR>");
}
#ifdef ADMIN_EMAIL
theEnvironment->h_puts("<P>" L_OPTIONS ":<BR>[<A HREF=\"mailto:" ADMIN_EMAIL "\">" L_EMAIL_ADMIN " (" ADMIN_EMAIL ")</A>]<BR>");
#endif
theEnvironment->h_fputs("[<A HREF=\"" NON_LOCALS_CAN_GO_TO "?");
theEnvironment->urlDumpOptionsForRedirect();
theEnvironment->h_printf("\">%s</A>] %s " NON_LOCALS_ORGANISATION "<BR>",L_REDIRECT_REQUEST,L_TO); // %s OK here
x=theEnvironment->getName(URL_ENV,MAY_RETURN_NULL);
if(x) {
if(!strncmp(x,"//",2)) x=x+2; // A bit nasty
theEnvironment->h_fputs("[<A HREF=\"");
if(needProtocolFor(x)) theEnvironment->h_fputs("http://");
theEnvironment->h_fputs(x);
theEnvironment->h_printf("\">%s</A>]%s<BR>",L_LEAVE_GATE_INLINK,L_LEAVE_GATE_AFTERLINK); // %s OK here
}
#ifdef NONLOCAL_PASSWORD
// *****************************
// Do stuff with ENV_NONLOCAL_PASSWORD (a box?)
#endif
outputLastUpdate(theEnvironment);
#endif // 'else' clause of PROXY_FOR_NONLOCALS
return 0;
}

#endif
