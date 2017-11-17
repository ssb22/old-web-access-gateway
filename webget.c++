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
// Code is a bit messy due to not too good exception handling
// in some compilers

// URLs %.800s etc needs sorting out

#include "allinc.h"
#include "webget.h"
#include "cgilib.h"
#include "string.h"
#include "platform.h"
#include "encoding.h"
#include "httphdr.h"
#include "literals.h"
#include "subhtml.h" // For encodingsInCurrentUse
#include "tagstuff.h"
#include "!debug.h"
#include "extensions.h"
#include "lastupd.h"
#include "makeform.h"
#include "mylynx.h"
#include "mirror.h"
#include "debug.h"
#include "rating.h"
#include "proxy.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define BUFLEN 256

#ifdef USE_POSIX_FORK
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

void callLynx(const char* lynxCommand,const char* inputFile,const char* outputFile,char*const* lynx_argv) {
int pid=fork();
if(!pid) { // Child
#ifdef TMP_PREFIX
chdir(TMP_PREFIX);
#endif
int fdr=(inputFile?open(inputFile,O_RDONLY):0),
fdw=open(outputFile,O_WRONLY|O_CREAT|O_TRUNC,00644),
devnull=open(
#ifdef DEBUGGING_DEVNULL
"/tmp/devnull",
O_WRONLY|O_CREAT|O_TRUNC,00644
#else
"/dev/null",
O_WRONLY
#endif
);
// 0=stdin, 1=stdout, 2=stderr
if(fdr) dup2(fdr,0); dup2(fdw,1); dup2(devnull,2);
execv(lynxCommand,lynx_argv);
// Should never get here:
throw "callLynx: execv failure";
} else {
if(pid<0) throw "callLynx: fork failure"; else {
int returnCode;
extern int child_pid;
child_pid=pid;
wait(&returnCode);
child_pid=0;
}
}
}
#endif /* USE_POSIX_FORK */

#ifdef STR_FUNCTIONS_NEED_IMPLEMENTING
char* strchr (const char* szSearch, int cFor);
#endif
char* stristr (const char* s1,const char* s2);
int strnicmp(const char* s1,const char* s2,int n);

char* sortDirectories(const char* relativeHTML,const char* currentPage);

enum WebGetLimits {
MAX_NESTING=10, // Maximum number of redirections
UNIQUEID_MAXLEN=80, // Let's be generous, won't regret it
UNIQUEID_PLUS_MORE=UNIQUEID_MAXLEN+30
};

void outputCheckOption(const char* option,const char* description,CGIEnvironment* theEnvironment);

static int count=0;
#ifndef TMP_PREFIX
#define TMP_PREFIX ""
#endif

bool wasPlainText=false; // NEEDATTENTION Don't really want this global (see main.c++)

// URL can be NULL, in which case got from environment
FILE* getWebPage(const char* url,CGIEnvironment* postDataOrNull,MayReturnNull mayReturnNull,InString &lastPageReturned,CGIEnvironment* encodingEnvironment,int dontTouchRefresh,int isTopLevel,int nestLevel,int returnAnyContentType) {
checkPoint();
if(nestLevel>=MAX_NESTING) throw "Redirection nesting too deep, possibly infinitely recursive";
// Names of temporary files:
char uniqueID[UNIQUEID_MAXLEN]; sprintf(uniqueID,"%d$%d",getpid(),count++);
char lynxInFile[UNIQUEID_PLUS_MORE]; sprintf(lynxInFile,TMP_PREFIX "%si.$$$",uniqueID); // %s OK
char redirFile[UNIQUEID_PLUS_MORE]; sprintf(redirFile,TMP_PREFIX "%so.$$$",uniqueID); // %s OK
// Is there a URL to get?
const char* idx=NULL;
int textSubmittedByUser=0,hasMimeHeader=0;
InString newUrl;
#ifdef LYNX_SSL
int isSSL=0;
#endif
if(!url) {
if(!postDataOrNull) throw "getWebPage: Null everything!";
url=postDataOrNull->getName(USER_TEXT_ENV,MAY_RETURN_NULL);
if(url) {
textSubmittedByUser=1;
FILE* f=fopen(redirFile,"w");
fputs("HTTP\nContent-type: text/plain\n\n",f);
fwrite(url,sizeof(char),strlen(url),f);
fclose(f);
url="";
} else {
url=postDataOrNull->getName(URL_ENV,MAY_NOT_RETURN_NULL);
idx=postDataOrNull->getName(ISINDEX_ENV,MAY_RETURN_NULL);
}
}
if (!textSubmittedByUser) {
#ifdef USAGE_LOGFILE
// Log the request
{ FILE* fp=fopen(USAGE_LOGFILE,"at"); if(fp) {
time_t t=time(NULL);
fputs(getenv("REMOTE_HOST")?getenv("REMOTE_HOST"):getenv("REMOTE_ADDR"),fp); fputs(" got ",fp); fputs(url,fp); // NEEDATTENTION Doesn't log idx
fprintf(fp," on %s",ctime(&t)); // %s OK
fclose(fp);
}}
#endif
// Is there authentication?
const char* userName=NULL,*password=NULL;
if(postDataOrNull) {
userName=postDataOrNull->getName(USER_ENV,MAY_RETURN_NULL);
password=postDataOrNull->getName(PWD_ENV,MAY_RETURN_NULL);
}
int isAuthentication=(userName && password);
// Check the URL is sane
if(strchr(url,':') && (strchr(url,':')<strchr(url,'.') && strchr(url,':')>strchr(url,'/'))) {
if(mayReturnNull==MAY_RETURN_NULL) return(NULL);
else throw "getWebFile: Malformed URL";
}
if(!strncmp(url,"//",2)) url=url+2; // A bit nasty

namedCheckPoint(debugCheckPoint2);
// Get the URL into newUrl
// (add any necessary index queries)
if(strncmp(url,"http://",7) && strncmp(url,"ftp://",6) && strncmp(url,"https://",8)) newUrl.addString("http://");
newUrl.addString(url);
{
// Add a trailing / if it's just a domain (needed by
// internal web getting code).
// 2001-06-11: Also, yahoo puts out addresses of the
// form www.yahoo.com?stuff.  This is completely wrong
// (there should be at least one intervening /) - it
// even breaks Squid.  But I suppose we'd better hack
// around it.
const char* start=newUrl.getString();
const char* slashSlash=strstr(start,"//")+2;
// (it WILL be non-0 as we've just added it above)
const char* question=strchr(slashSlash,'?');
const char* nextSlash=strchr(slashSlash,'/');
if(question && (!nextSlash || question<nextSlash)) {
// Hack for yahoo (and servers that behave like it)
// (doesn't have to be particularly efficient for now
// - very rarely used)
InString replacement;
for(; start<question; start++) replacement.addCharacter(*start);
replacement.addCharacter('/');
replacement.addString(start);
newUrl.clear(); newUrl.addString(replacement.getString());
} else if(!nextSlash) newUrl.addCharacter('/');
}
// Add any index queries
if(idx) { newUrl.addCharacter('?'); newUrl.addString(idx); }

namedCheckPoint(debugCheckPoint2_1);
// hasMimeHeader is non-0 for HTTP (and HTTPS) URLs
hasMimeHeader=(proxyFtp()?1:strnicmp(url,"ftp://",6));
#ifdef LYNX_SSL
isSSL=!strnicmp(url,"https://",8);

// hack for testing SSL when you're offline without an
// SSL server
// if(isSSL) { newUrl.clear(); newUrl.addString("http://localhost/"); }

#endif

if(!strchr(url,'.') && strchr(url,(char)0xA9)) {
// Workaround for some versions of Mozilla (as
// distributed with Debian Linux) that put 0xA9
// instead of '.' when you type the URL in manually
// (probably because they default to an Armenian
// character set)
newUrl.replace((char)0xA9,'.');
}

// If we're making a CGI query, write out the form data
// for Lynx.  Alternatively, add it to the URL, if we can
// use our own code (ie. no authentication).  Or add it
// to the request body.
#ifdef USE_GNU_SOCKETS
// Can we use our own code instead of calling Lynx?
// Must be HTTP (so hasMimeHeader), and no authentication
// But if we're using a proxy and the proxy can do FTP,
// then FTP is OK too (but in this case hasMimeHeader
// will be #defined to 1, since FTP requests will always
// have a MIME header added by the proxy)
int canUseOwnCode=(hasMimeHeader && !isAuthentication
#ifndef PROXY_AUTH_HEADER
&& !proxyauth()
#endif
#ifdef LYNX_SSL
&& !isSSL
#endif
);
InString requestBody;
#else
#define canUseOwnCode 0
#endif
namedCheckPoint(debugCheckPoint2_2);
int isDataToPost=0;
int postInBody=(encodingEnvironment->getName(FORM_NEEDS_POSTING,MAY_RETURN_NULL)!=NULL);
if (postDataOrNull) {
#ifdef USE_GNU_SOCKETS
if(canUseOwnCode) {
checkPoint();
if(postInBody) isDataToPost=postDataOrNull->queryDumpExcludePrefix(SSB22_PREFIX,requestBody,0);
else isDataToPost=postDataOrNull->queryDumpExcludePrefix(SSB22_PREFIX,newUrl,1);
} else {
#endif
checkPoint();
FILE* fp=fopen(lynxInFile,"wt");
if(!fp) throw "getWebPage: Failed to open file for Lynx input";
isDataToPost=postDataOrNull->lynxDumpExcludePrefix(SSB22_PREFIX,fp);
// (This also un-escapes any options that happen to use SSB22_PREFIX)
fclose(fp);
#ifdef USE_GNU_SOCKETS
}
#endif
}

namedCheckPoint(debugCheckPoint3);
newUrl.removeIsspace();
url=newUrl.getString(); // hack

#ifdef WGET_COMMAND
// #define canUseWget (!isDataToPost)
#define canUseWget (!hasMimeHeader)
// Only using wget for FTP URLs, because with HTTP URLs
// it follows redirections without telling us where it's
// going (so relative URLs from directories don't work)
#else
#define canUseWget (0)
#endif

// Sort out authentication parameter(s) (if needed)
InString authtemp,authtemp2;
if(isAuthentication) {
#ifdef WGET_COMMAND
if(canUseWget) {
authtemp.addString("--http-user=");
#ifdef USE_POSIX_FORK
authtemp.addString(userName);
#else
authtemp.addStringWithCommandPrecautions(userName);
#endif
authtemp2.addString("--http-passwd=");
#ifdef USE_POSIX_FORK
authtemp2.addString(password);
#else
authtemp2.addStringWithCommandPrecautions(password);
#endif
} else {
#endif
authtemp.addString("-auth=");
#ifdef USE_POSIX_FORK
authtemp.addString(userName);
authtemp.addString(":");
authtemp.addString(password);
#else
authtemp.addStringWithCommandPrecautions(userName);
authtemp.addStringWithCommandPrecautions(":");
authtemp.addStringWithCommandPrecautions(password);
#endif
#ifdef WGET_COMMAND
}
#endif
} // else leave authtemp empty

// Construct the user agent (and other headers)
#if (defined(LYNX_USER_AGENT) || defined(USE_GNU_SOCKETS))
InString userAgent;
userAgent.addString(/* LYNX_USER_AGENT */ "Mozilla/5.0 compatible Access Gateway");
// This is for the benefit of broken servers that throw
// you out if you don't have a "Version 4 browser".  NB
// it is not violating Netscape's copyright, because it
// does not actually pretend to be Mozilla.
// 2003-02-02: "Upgraded" from 4.7 to 5.0 due to www.altova.com's check
if(!(encodingEnvironment->getName(ENV_PREFIX "B",MAY_RETURN_NULL))) encodingEnvironment->theCookies.appendToUserAgent(userAgent,url);
// We'll also append a copy of the URL as "Referer:"
// because some web servers automatically redirect to the
// main page unless a referer from their own site is given
if(!stristr(url,"//216.33.")) {
userAgent.addString(CRLF "Referer: ");
userAgent.addString(url);
}
// 2000-11-11: Interesting feature of Hotmail: If you
// click "sign up", it takes you to a redirector CGI on
// another server that uses the "Referer" field to work
// out where to redirect you to.  So our sending a copy of
// the URL as "Referer" breaks it.

// (referrer override when o/p links to diff server??)

// (if *have* got diff URL in real referer, use it)
// temporary hack: see !stristr above (horrible!)
// **********************************************

// Add an appropriate Accept-Language header; if none then
// propagate the user's Accept-Language header.
// If we're calling Lynx and it sends its own Accept-Language
// header then that's harmless (it should send ours first).
#ifdef ENCODINGS_AVAILABLE
const char* langRequest=getIsoLanguageNameOrNull(encodingEnvironment);
if(!langRequest) langRequest=getenv("HTTP_ACCEPT_LANGUAGE");
#else
const char* langRequest=getenv("HTTP_ACCEPT_LANGUAGE");
#endif // ENCODINGS_AVAILABLE
if(langRequest) {
userAgent.addString(CRLF "Accept-Language: ");
userAgent.addString(langRequest);
}
// 2001-03-16: Also need to propagate Cache-control and
// Pragma
const char* cache=getenv("HTTP_CACHE_CONTROL");
if(cache) { userAgent.addString(CRLF "Cache-control: "); userAgent.addString(cache); }
cache=getenv("HTTP_PRAGMA");
if(cache) { userAgent.addString(CRLF "Pragma: "); userAgent.addString(cache); }
#ifdef DEBUGGING_USER_AGENT
encodingEnvironment->h_printf("Debugger: userAgent=%s\n",userAgent.getString()); // %s OK
#endif
#endif // LYNX_USER_AGENT || USE_GNU_SOCKETS

// Decide which method to use for contacting the server
#ifdef USE_GNU_SOCKETS
if(canUseOwnCode) {
try {
socketWebGet(url,userAgent.getString(),redirFile,requestBody.getString());
} catch (...) {
if(mayReturnNull==MAY_RETURN_NULL) return NULL;
else throw;
}
} else {
#endif // USE_GNU_SOCKETS
// We have to call Lynx
// Construct the Lynx command line:
#define MAXARGS 19
const char* lynx_argv[MAXARGS]; int lPtr=0;
#ifdef WGET_COMMAND
if (canUseWget) {
lynx_argv[lPtr++]=WGET_COMMAND;
lynx_argv[lPtr++]="-q"; // quiet
lynx_argv[lPtr++]="-O"; lynx_argv[lPtr++]="-"; // output to stdout
lynx_argv[lPtr++]="--ignore-length";
} else
#endif
lynx_argv[lPtr++]=LYNX_COMMAND;
#ifdef LYNX_USER_AGENT
InString tempUA;
// NB Don't limit the scope of these InStrings, because
// they need to exist at the system call
tempUA.addString(canUseWget?"--user-agent=":"-useragent=");
#ifdef USE_POSIX_FORK
tempUA.addString(userAgent.getString());
#else
tempUA.addStringWithCommandPrecautions(userAgent.getString());
#endif // USE_POSIX_FORK
lynx_argv[lPtr++]=tempUA.getString();
// encodingEnvironment->h_puts(tempUA.getString()); // Debugger
#endif // LYNX_USER_AGENT
#ifdef LYNX_CFG_FILE
if(!canUseWget) lynx_argv[lPtr++]="-cfg=" LYNX_CFG_FILE;
#endif
// if(canUseWget) { if(hasMimeHeader) lynx_argv[lPtr++]="-s"; } else
if(!canUseWget) lynx_argv[lPtr++]=hasMimeHeader?"-mime_header":"-source";
if(isDataToPost) lynx_argv[lPtr++]=(postInBody?"-post_data":"-get_data");
if(isAuthentication) {
lynx_argv[lPtr++]=authtemp.getString();
if(!authtemp2.isEmpty()) lynx_argv[lPtr++]=authtemp2.getString();
}
if(proxyauth()) {
// We're behind a proxy that needs authentication
if(canUseWget) {
lynx_argv[lPtr++]=proxyauth_wget1();
lynx_argv[lPtr++]=proxyauth_wget2();
} else lynx_argv[lPtr++]=proxyauth();
}
#ifdef USE_POSIX_FORK
lynx_argv[lPtr++]=url;
#else
InString tempNewUrl; // Don't put braces around it,
// because it needs to exist at the system call
tempNewUrl.addStringWithCommandPrecautions(url);
lynx_argv[lPtr++]=tempNewUrl.getString();
#endif
#ifndef USE_POSIX_FORK
if(isDataToPost) {
InString intemp;
intemp.addCharacter('<'); intemp.addStringWithCommandPrecautions(lynxInFile);
lynx_argv[lPtr++]=intemp.getString();
}
InString rdrtemp;
rdrtemp.addCharacter('>'); rdrtemp.addStringWithCommandPrecautions(redirFile);
#ifdef LYNX_NEEDS_CLOSE_QUOTE
rdrtemp.addCharacter('\"');
#endif
lynx_argv[lPtr++]=rdrtemp.getString();
#endif
// Need to increase MAXARGS above if adding
// Also check the case of USE_GNU_SOCKETS
lynx_argv[lPtr]=NULL;
#ifdef LYNX_NEEDS_TERM
putenv((char*)"TERM=vt100");
// (this will be done for wget as well, but never mind.
// The next lot (proxy) must be done for lynx AND wget.)
// setenv(name,val,overwrite) - BSD
// putenv("name=val") - posix
// ISO only getenv
#endif
if(proxy()) {
char buf[BUFLEN+1];
sprintf(buf,"http_proxy=http://%s:%d",proxy(),proxyPort()); // %s OK (this info is supplied by the sysadmin)
putenv(buf);
if(proxyFtp()) {
sprintf(buf,"ftp_proxy=http://%s:%d",proxy(),proxyPort()); // %s OK (this info is supplied by the sysadmin)
putenv(buf);
}
putenv((char*)"no_proxy="); // in case it was set to something
}
#ifdef USE_POSIX_FORK
callLynx(lynx_argv[0],isDataToPost?lynxInFile:NULL,redirFile,(char*const*)lynx_argv);
#else
int lenCnt=0;
#ifdef TMP_PREFIX
#ifdef CD_TO_TMP_PREFIX
#define CD_GETSTRING
InString cd; cd.addString("cd ");
cd.addString(TMP_PREFIX); cd.addCharacter(COMMAND_SEPARATOR);
lenCnt+=cd.length();
#endif
#endif
for(int i=0; i<lPtr; i++) lenCnt+=(strlen(lynx_argv[i])+1);
char* x=new char[lenCnt+1]; x[0]=0; // +1 perhaps not required but...
#ifdef CD_GETSTRING
strcpy(x,cd.getString());
#endif
for(int i=0; i<lPtr; i++) {
strcat(x,lynx_argv[i]);
if(i<lPtr-1) strcat(x," ");
}
system(x);
delete[] x;
#endif
#ifdef USE_GNU_SOCKETS
}
#endif

} // end of !textSubmittedByUser

// Finished all that Lynx-calling or socket-calling stuff
// Now figure out what happened

// NEEDATTENTION If a throw clobbers htmlInput once it
// has already been closed, we'll get fclose being called
// twice
FILE* htmlInput=NULL; try {
namedCheckPoint(debugCheckPoint4);
// BEGIN open redirFile and get the headers
#ifdef DEBUG_MIME_HEADER
static int firstFile=1;
htmlInput=fopen(firstFile?DEBUG_MIME_HEADER:redirFile,"rb");
firstFile=0;
#else
htmlInput=fopen(redirFile,"rb");
// NB Do NOT open "rt" because it might be compressed!
#endif
if(!htmlInput) throw "getWebPage: Failed to open Lynx output file";
namedCheckPoint(debugCheckPoint5);
// Nasty patch follows NEEDATTENTION Sort this out
// (it's patching ftp://nic.funet.fi/pub/culture/japan/mirrors/monash/pub/nihongo/00INDEX.html
// which doesn't have <HTML> in it)
int forceOverride=0; if(!hasMimeHeader && strlen(url)>=6) {
if(!stricmp(url+strlen(url)-5,".html")
|| !stricmp(url+strlen(url)-4,".htm")
|| !stricmp(url+strlen(url)-6,".shtml")) forceOverride=1;
}
namedCheckPoint(debugCheckPoint6);
HttpHeader header(htmlInput,hasMimeHeader?0:forceOverride?2:1,encodingEnvironment,lynxInFile,redirFile,(mayReturnNull==MAY_NOT_RETURN_NULL));
#ifdef DEBUGGING_WEBGET_FN
encodingEnvironment->h_printf("Debugger: header.wasItSuccessful()=%d<BR>\n",header.wasItSuccessful());
encodingEnvironment->h_printf("Debugger: url=%s<BR>\n",url); // %s OK (debugging)
#endif
namedCheckPoint(debugCheckPoint7);
if(!header.wasItSuccessful()) {
if(mayReturnNull==MAY_RETURN_NULL) { fclose(htmlInput); return(NULL); }
else throw "getWebPage: HttpHeader constructor failed";
}
// Before adding cookies, check the rating
// Don't just call header.getValueOf("PICS-Label")
// because that will only return the last one.  Need
// to check all of them.
if(!encodingEnvironment->getName(CONTINUE_ON_RATING,MAY_RETURN_NULL) && !encodingEnvironment->getName(PREPROCESS_ENV,MAY_RETURN_NULL)) {
for(int i=0; i<header.getNumberOfValues(); i++) {
if(!stricmp(header.getKeyByNumber(i),"PICS-Label")){
// (actually it should probably be strcmp, but need to be resilient)
checkRating(header.getValueByNumber(i));
}
}
}
// Add cookies (2000-07-15):
// (we need to do this BEFORE redirection, because of
// some servers that don't let you in unless you accept
// a cookie AND a redirection)
#if (defined(LYNX_USER_AGENT) || defined(USE_GNU_SOCKETS))
// NEEDATTENTION Not handling the case when not
// LYNX_USER_AGENT and USE_GNU_SOCKETS and we're not
// using our own code due to authentication or SSL
// (it's harmless, but could give cookie problems that
// are hard to track down)
if(!(encodingEnvironment->getName(ENV_PREFIX "B",MAY_RETURN_NULL)) && header.handleSetCookies(encodingEnvironment->theCookies)) {
// There were some, so write it back to the environment
InString s;
encodingEnvironment->theCookies.writeToString(s);
encodingEnvironment->setName(ENV_COOKIES,s.getString());
}
#endif

#ifdef DEBUGGING_WEBGET_FN
encodingEnvironment->h_printf("Debugger: url=%s<BR>\n",url); // %s OK (debugging)
#endif
namedCheckPoint(debugCheckPoint8);
const char* x=header.getValueOf("Location"); if(x) {
checkPoint();
#ifdef DEBUGGING_WEBGET_FN
encodingEnvironment->h_puts("Debugger: Redirecting<BR>");
encodingEnvironment->h_puts(x);
#endif
// We're redirecting...
fclose(htmlInput); htmlInput=NULL; // (for the exception handler)
// Need to sort this out - a bit nasty
// NB postDataOrNull ?: stuff commented out, because it causes problems -
// apparently, when you get a redirection request, you're not supposed
// to re-post the data (if you do, the web server may return a 405 invalid request)
if(x[0]=='/' && x[1]=='/') return(getWebPage(x+2,/*postInBody?postDataOrNull:*/NULL,mayReturnNull,lastPageReturned,encodingEnvironment,dontTouchRefresh,isTopLevel,nestLevel+1,returnAnyContentType));
x=sortDirectories(x,url); // It may be relative HTML
// (The check for data needing posting is because when a GET query is redirected you should drop the extra data)
FILE* p=getWebPage(x,/*postInBody?postDataOrNull:*/NULL,mayReturnNull,lastPageReturned,encodingEnvironment,dontTouchRefresh,isTopLevel,nestLevel+1,returnAnyContentType);
delete[] x; return(p);
}
// END open redirFile and get the headers
// Now htmlInput is in position to start reading the
// HTML, and we're in the right file (followed any
// re-directions), so time to call the external filters
// if any.
// However, need to initialise the character converters
// first (especially if there are XHTML filters).  Do
// as much as possible before calling the external
// filters.
// ======================================
// At this point we can start writing to encodingEnvironment.
// Because we need to get the cookies before writing the
// options link, the options link and associated
// stuff has been moved to here instead of main.c++.
int needALine=0; long posAtStart=0;
if(isTopLevel) {
checkPoint();
needALine=outputOptionsLink(encodingEnvironment,url);
posAtStart=encodingEnvironment->getPos();
#ifdef MIRRORS
// Is the user coming from an area where a different
// copy of the gateway might be more appropriate?
// (Controlled by platform.h)
checkForMirrors(encodingEnvironment);
#endif
}
namedCheckPoint(debugCheckPoint9);
// Now write anything that the user needs to know about
// the page (the "mode line" or whatever); add a <BR>
// before each line.
#ifdef LYNX_SSL
// Do we need to warn about security?
if(isSSL) {
// Don't bother to check isTopLevel - an SSL document
// in a frame still needs a warning
encodingEnvironment->h_fputs("<BR>");
extern bool UsingSSL;
encodingEnvironment->h_puts(UsingSSL?L_SSL_WARNING_SSL:L_SSL_WARNING);
}
#endif
// Does the page have any refresh tags?
x=header.getValueOf("Refresh"); if(x) {
checkPoint();
#ifdef DEBUGGING_WEBGET_FN
encodingEnvironment->h_puts("Debugger: Refreshing<BR>");
#endif
if(dontTouchRefresh) {
HTMLTag myTag("META");
myTag.addAttribute("HTTP-EQUIV","Refresh");
myTag.addAttribute("CONTENT",x);
myTag.outputWithModifiedContent(url,encodingEnvironment);
} else {
char* loc=(char*)strchr(x,'='); if(loc) {
loc=loc+1;
encodingEnvironment->h_fputs("<BR>");
encodingEnvironment->h_fputs(L_PAGE_REFRESH);
HTMLTag myTag("A");
myTag.addAttribute("HREF",loc);
myTag.outputWithModifiedURLs(url,encodingEnvironment);
encodingEnvironment->h_fputs(loc); encodingEnvironment->h_puts("</A>.");
} else {
encodingEnvironment->h_fputs("<BR>");
encodingEnvironment->h_puts(L_PAGE_NOURL_REFRESH);
}
}
}
// Do we need to output the date stamp?
if(isTopLevel && !(encodingEnvironment->getName(ENV_PREFIX "l",MAY_RETURN_NULL))) {
checkPoint();
x=header.getValueOf("Last-Modified"); if(x) {
encodingEnvironment->h_fputs(encodingEnvironment->hasAdvancedSince(posAtStart)?"<BR>":"&nbsp; ");
encodingEnvironment->h_fputs(L_LAST_MODIFIED);
encodingEnvironment->h_fputs(": ");
encodingEnvironment->h_fputs(x);
}
}
// Does the page contain any mediator warnings?
x=header.getValueOf("X-Mediator-Warning"); if(x && !strnicmp(x,CANONICAL_NAME "=",(CANONICAL_LEN+1))) {
encodingEnvironment->h_fputs("<BR>");
encodingEnvironment->h_fputs(L_PAGE_CONTAINS_WARNING);
encodingEnvironment->h_fputs(": ");
encodingEnvironment->outHTMLLine(x+(CANONICAL_LEN+1));
const char* x=encodingEnvironment->getName(URL_ENV,MAY_NOT_RETURN_NULL);
if(!strncmp(x,"//",2)) x=x+2; // A bit nasty
encodingEnvironment->h_fputs("&nbsp; <A HREF=\""); if(needProtocolFor(x)) encodingEnvironment->h_fputs("http://");
encodingEnvironment->h_fputs(x);
encodingEnvironment->h_fputs("\">");
encodingEnvironment->h_fputs(L_EXIT);
encodingEnvironment->h_fputs("</A>");
}
// also do we need a message about PDF conversion
adobePDFHackMessage(encodingEnvironment);
// Any other headers that are to be processed
// should go here (before Content-Type)
x=header.getValueOf("Content-Type"); if(x || returnAnyContentType) {
checkPoint();
#ifdef DEBUGGING_GETWEBPAGE
if(x) encodingEnvironment->h_printf("Debugger: Content-Type=%s returnAnyContentType=%d<BR>\n",x,returnAnyContentType); // %s OK (debugging)
#endif
#ifdef ENCODINGS_AVAILABLE
if(!returnAnyContentType) setUpCharacterTranslation(encodingEnvironment,x,htmlInput);
// (2001-02-14: !returnAnyContentType added because otherwise outputs "Overridden to ..." or whatever every time it fetches a SCRIPT SRC)
#endif
// NB Checking for text/htm rather than text/html because some pages have funny ideas
// 2000-06-17: Now checking just for text/ (rather
// than text/htm and text/plain) because of a comment
// that
// http://homepages.tcp.co.uk/~kiyo/link/link.htm
// didn't work (it said text/heml (sic))
if(returnAnyContentType || !strnicmp(x,"text",4) || strstr(x,"html")) {
// 2000-11-27: Now checking for "text" only (not /)
// because one page was found using a hyphen instead
// 2006-08-01 added strstr(x,"html") due to application/xhtml+xml
lastPageReturned.clear(); lastPageReturned.addString(url);
wasPlainText=(x && !strnicmp(x,"text/plain",10));
// About to return, so start of real page
// Output a line if necessary
if(isTopLevel && (needALine || encodingEnvironment->hasAdvancedSince(posAtStart))) outputStartPageLine(encodingEnvironment);
// This is where we return htmlInput.
// But before that happens, call the external
// extensions if there are any.
#ifdef External_Extensions
void fileCopy(FILE* fp,const char* newName);
fileCopy(htmlInput,lynxInFile);
fclose(htmlInput); htmlInput=NULL; // for 'catch'
htmlInput=fopen(callFilters(lynxInFile,redirFile,encodingEnvironment),"rb");
if(!htmlInput) throw "Failed to open output of external extension";
#endif
return(htmlInput);
} // else say what it is?
} // else no content-type
// Any other headers that are to be processed
// should go before Content-Type
#ifdef DEBUGGING_WEBGET_FN
encodingEnvironment->h_puts("Debugger: Dropped out of structure<BR>");
#endif
fclose(htmlInput); htmlInput=NULL; if(mayReturnNull==MAY_RETURN_NULL) return(NULL);
else throw "getWebPage: Page is not a HTML document";
} catch(...) { if(htmlInput) fclose(htmlInput); throw; }
}

void removeTempFiles() {
#ifndef PRESERVE_TEMP_FILES
char uniqueID[UNIQUEID_MAXLEN];
char fileToDelete[UNIQUEID_PLUS_MORE];
for(int i=0; i<count; i++) {
sprintf(uniqueID,"%d$%d",getpid(),i);
sprintf(fileToDelete,TMP_PREFIX "%si.$$$",uniqueID); remove(fileToDelete); // %s OK
sprintf(fileToDelete,TMP_PREFIX "%so.$$$",uniqueID); remove(fileToDelete); // %s OK
}
#endif
}
