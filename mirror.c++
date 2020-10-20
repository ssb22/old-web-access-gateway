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
#include "mirror.h"
#ifdef MIRRORS
#include "cgilib.h"
#include "strfunc.h"
#include "literals.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#ifdef USE_GNU_SOCKETS
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

// Todo: remove Justvanilla from this lot

// MASTER MIRROR DATA for servers that are CJK-enabled
// and automatically updated
// (NB Some non-CJK enabled servers can re-direct to a
// CJK-enabled server, but not the other way around)
// (+ servers trying to minimise their traffic should never
// be re-directed to)
#if (defined(Is_Monash) || defined(Is_Cam) || defined(Is_Japan2001) || defined(Is_Tardis) || defined(Is_Flatline) || defined(Is_ACCU) || defined(Is_ACCU_Russia) || defined(Is_Socratec) || defined(Is_Justvanilla) || defined(Is_Enfour))

#define Mirrordata_Already_Defined

// Need to sort out before access goes down:
// Tardis and ACCU are using ssb22's image server; also japanese.html (which is pointing to monash!) (nobody's using www.jicc.demon.co.uk/files/)

// Which domains should use the USA servers?
// .com/.net/.org is awkward so we'll leave those alone
#define USA_Domains ".edu",".webtv.net",".gov",".mil",".home.com",".us",".ca"

// Domains around the Pacific (that I'm *guessing* have more connectivity to Australia than they do to the USA)
#define Pacific_Domains ".au",".jp",".sg",".hk"

static const char* mirrorData[]={
// If we're not in Cambridge, give the Cambridge mirror
#ifndef Is_Cam
// "Cambridge University","http://ssb22.joh.cam.ac.uk/cgi-bin/access",".cam.ac.uk",0,
// COMMENTED OUT DUE TO SERVER PROBLEMS (to be fixed?)
//"Cambridge University","http://www-jcsu.jesus.cam.ac.uk/access/cgi-bin/access.cgi",".cam.ac.uk",0,
#endif
// If we're not in Edinburgh, give the Edinburgh mirror
#ifndef Is_Tardis
// COMMENTED OUT DUE TO SERVER DOWN
// "Edinburgh University","http://www.tardis.ed.ac.uk/cgi/access/access",".ed.ac.uk",".gla.ac.uk",0,
#endif
// If we're not in Imperial, give the Imperial mirror
#ifndef Is_Japan2001
// COMMENTED OUT DUE TO SERVER DOWN
// "Imperial College London","http://japan2001.doc.ic.ac.uk/~silas/",".ic.ac.uk",0,
#endif
// If we're not in UK, give Imperial as the UK mirror
// (UK = Cam, Ed, Imperial, Flatline, ACCU, Socratec)
#if (!(defined(Is_Japan2001) || defined(Is_Cam) || defined(Is_Tardis) || defined(Is_Flatline) || defined(Is_ACCU) || defined(Is_Socratec) || defined(Is_Justvanilla)))
// COMMENTED OUT DUE TO SERVER DOWN - USING CAM FOR NOW
//"Imperial College London","http://japan2001.doc.ic.ac.uk/~silas/",".uk",".ie",".ja.net",0,
// CAN'T USE CAM BECAUSE cache.bt.net:3128 IS DOWN - USING
// KANGZHUANG FOR NOW (very temporary!)
// (Note: access.ucam.org and JCN also pointing to this as
// gateway for nonlocals)
"kangzhuang.ucam.org","http://kangzhuang.ucam.org/cgi-bin/access/access.cgi",".uk",".ie",".ja.net",0,
#endif

// COMMENTED OUT - this mirror doesn't stay up-to-date
// If we are non-CJK capable then we can re-direct to
// the ACCU Russian mirror (which is non-CJK capable)
// if we're not that mirror.
//#if (defined(Is_Flatline) || defined(Is_ACCU) || defined(Is_Socratec))
// "ACCU's Russian mirror (Informika)","http://www.accu.informika.ru/cgi-bin/access/access",".ru",0,
//#endif

// If we're not Enfour, give Enfour as the USA mirror
#ifndef Is_Enfour
//"Enfour Systems (USA)","http://www.enfour.com/cgi-bin/access.cgi",USA_Domains,0,
#endif
// (Add USA_Domains to Monash if this goes)

// If we're not Monash, give Monash as the mirror for
// the Pacific region
#ifndef Is_Monash
"Monash University (Australia)","http://www.csse.monash.edu.au/cgi-bin/cgiwrap/~jwb/access",Pacific_Domains,USA_Domains,0,
#endif
0};
#endif // defined(lots of things)

#ifdef USE_GNU_SOCKETS
const char* nslookup(const char* IP) {
char ipAddress[4]; // NEEDATTENTION This is ipv4 specific; won't work on ipv6
for(int i=0; i<4; i++) {
if(!isdigit(*IP)) return NULL; // invalid IP
ipAddress[i]=atoi(IP);
if(i<3) {
// (Condition because we want to check IP is non-zero as a test that the proxy is sending a correctly-formed IP)
IP=strchr(IP,'.'); if(IP) IP++; else break;
}
}
if(IP) {
struct hostent* hostinfo=gethostbyaddr(ipAddress,4,AF_INET);
if(hostinfo && hostinfo->h_name && *hostinfo->h_name)
return strnew(hostinfo->h_name);
}
return NULL;
}
#endif

void checkForMirrors(CGIEnvironment* e) {
// Note: Do not force the user to use the mirror.  Besides
// anything else, the mirror might be down.
const char* remoteHost=NULL;
#ifdef USE_GNU_SOCKETS
// Try proxy's "X-Forwarded-For" first (because of e.g. JANET cache).  But that will give an IP address, so we have to look it up.
// eg. HTTP_X_FORWARDED_FOR=131.111.142.162, 131.111.8.68
// NEEDATTENTION This is all very well, but we then need to tell the proxy not to cache the thing
bool needToDelete=false;
const char* fwdFor=getenv("HTTP_X_FORWARDED_FOR");
while(fwdFor && *fwdFor) {
if(isdigit(*fwdFor)) remoteHost=nslookup(fwdFor);
if(remoteHost) { needToDelete=true; break; }
else {
// Proxy might be sending the actual domain name, or
// it might be "unknown".  If "unknown" then we might
// be able to look up the next one on the list.
// NB we might get servers like "unknown.example.net"
if(strnicmp(fwdFor,"unknown",7)
|| (fwdFor[7] && fwdFor[7]!=',')) {
// NEEDATTENTION "magic number" (length of "unknown") used three times
remoteHost=fwdFor; break;
}
}
// Go over the next comma
fwdFor=strchr(fwdFor,',');
if(fwdFor) { do fwdFor++; while(isspace(*fwdFor)); }
}
#endif
if(!remoteHost) {
remoteHost=getenv("REMOTE_HOST");
#ifdef USE_GNU_SOCKETS
if(!remoteHost || !(*remoteHost)) {
// The Web server might not be performing lookups
// (If it is then this should not be too much extra
// expense, since there should be a DNS cache)
remoteHost=nslookup(getenv("REMOTE_ADDR"));
if(remoteHost) needToDelete=true;
}
#endif
}
if(!remoteHost || !(*remoteHost)) return;

#ifndef Mirrordata_Already_Defined
const char* mirrorData[]=MIRRORS;
#endif
int i=0; while(mirrorData[i]) {
int start=i; i+=2; // skip over name and URL
for(; mirrorData[i]; i++) {
int tmp=strlen(mirrorData[i]),t2=strlen(remoteHost); // Bit of optimisation
if(t2>=tmp && !strncmp(remoteHost+(t2-tmp),mirrorData[i],tmp)) {
// Found a match
e->h_fputs("<BR>");
e->h_fputs(L_MIRROR_START);
e->h_fputs("<A HREF=\"");
e->h_fputs(mirrorData[start+1]); // URL
e->h_putchar('?');
e->urlDumpOptionsForRedirect();
e->h_fputs("\">");
e->h_fputs(L_MIRROR_LINK);
e->h_fputs(mirrorData[start]); // Name
e->h_fputs("</A> ");
e->h_puts(L_MIRROR_END);
#ifdef USE_GNU_SOCKETS
if(needToDelete) delete[] remoteHost;
#endif
return;
}
}
i++; // The next record (or another 0)
}
#ifdef USE_GNU_SOCKETS
if(needToDelete) delete[] remoteHost;
#endif
}
#endif
