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
