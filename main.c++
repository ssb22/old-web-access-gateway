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
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "bitsize.h"
#include "platform.h"
#include "strfunc.h"
#include "cgilib.h"
#include "makeform.h"
#include "webget.h"
#include "htmlfilt.h"
#include "localusr.h"
#include "literals.h"
#include "images.h"
#include "!debug.h"
#include "welcome.h"
#include "prxyprob.h"
#include "extensions.h"
#include "lastupd.h"
#include "help.h"
#include "exceptn.h"
#include "debug.h"
#include "rating.h"

#ifdef ENCODINGS_VISIBLE
#ifndef ENCODINGS_AVAILABLE
#error "You're not supposed to define ENCODINGS_VISIBLE without ENCODINGS_AVAILABLE"
#endif
#endif

#ifdef LOGIN_TOUCH
void touchFileForLoggedIn() {
checkPoint();
FILE* fp=fopen(LOGIN_TOUCH,"wb");
if(fp) fclose(fp);
}
#endif

#ifdef STR_FUNCTIONS_NEED_IMPLEMENTING
char* strchr (const char* szSearch, int cFor);
#endif

void redirectAndAbort(const char* url) {
checkPoint();
// For when they press "Exit"
#ifndef BRIEF_RESPONSE
puts("HTTP/1.0 200 OK");
#endif
puts("Content-type: text/html");
fputs("Location: ",stdout);
if(!strncmp(url,"//",2)) url+=2; // nasty
if(needProtocolFor(url)) fputs("http://",stdout);
puts(url);
putchar('\n'); // blank line
exit(0);
}

bool needProtocolFor(const char* url) {
const char *colon,*slash;
return !((colon=strchr(url,':')) &&
(!(slash=strchr(url,'/')) ||
colon < slash));
}

inline int realString(const char* c) {
return (c && *c);
}

bool UsingSSL=false;
#ifdef CAN_SWITCH_SSL
bool WantSSL=false;
#endif

int main(int argc,char* argv[]) {
checkPoint();
#ifdef DEVELOPMENT_COPY
void processArguments(int argc,char* argv[]);
if(argc>1 && !getenv("REMOTE_ADDR")) { processArguments(argc,argv); return 0; }
#endif
#ifdef USE_POSIX_FORK
void setUpSignalHandlers();
setUpSignalHandlers();
#endif
#ifdef DEBUG_ADD_TEXT_PLAIN
puts("Content-type: text/plain\n");
#endif
#ifdef LOGIN_TOUCH
touchFileForLoggedIn();
#endif
#ifdef DELETE_ON_LOCALHOST
const char* x=getenv("REMOTE_ADDR");
if(x && !strcmp(x,"127.0.0.1")) remove(EXCEPTION_LOGFILE);
#endif
{ const char* https=getenv("HTTPS"); if(https && !strcmp(https,"on")) UsingSSL=true; }
#ifdef CAN_SWITCH_SSL
WantSSL = UsingSSL; // initially
#endif
try {
CGIEnvironment* theEnvironment=NULL;
try {
theEnvironment=new CGIEnvironment(ENV_PREFIX,ENV_PREFIX "C"); // must be literals
} catch(...) {
// The reason for separating out the two 'try' blocks like this
// is that, if theEnvironment is stored as a register variable,
// it could get re-set to NULL if an exception is thrown.  This is
// pretty bad since it means the user never finds out about the
// exception.
// At this point, we DON'T want to run the second block.  So...
throw;
} try {
#ifdef PLATFORM_SPECIFIC_AFTER_CGIINPUT_CODE
PLATFORM_SPECIFIC_AFTER_CGIINPUT_CODE(theEnvironment);
#endif
// Proxy problem etc later (let them into the forms)
#ifdef ENCODINGS_AVAILABLE
// Convert legacy parameters:
if(theEnvironment->getName(ENV_LANG_DONT_SHOW_IMAGES,MAY_RETURN_NULL)) { theEnvironment->deleteName(ENV_LANG_DONT_SHOW_IMAGES); theEnvironment->setName(ENV_LANG_SHOW_CHARS_AS,LCharsAmp); }
#ifdef UNICODE_HEIGHT
if(theEnvironment->getName(ENV_LANG_DOUBLE_IMAGE_SIZE,MAY_RETURN_NULL)) { theEnvironment->deleteName(ENV_LANG_DOUBLE_IMAGE_SIZE); theEnvironment->setName(ENV_LANG_SHOW_CHARS_AS,LCharsDbl); }
#endif
// End of converting legacy parameters
const char* c=theEnvironment->getName(ENV_LANG_SHOW_CHARS_AS,MAY_RETURN_NULL);
if(c) {
subHtmlShowCharsAs=(ShowCharsAs)atoi(c);
// Can delete it if it's default:
if(subHtmlShowCharsAs==CharsIMG) theEnvironment->deleteName(ENV_LANG_SHOW_CHARS_AS);
}
// Don't have CharsIMG or anything
// if no conversion (otherwise would output
// BASE HREF etc unnecessarily)
c=theEnvironment->getName(ENV_LANGUAGE_TABLE,MAY_RETURN_NULL);
if(!c || !strcmp(c,NO_CONVERS_VAL)) subHtmlShowCharsAs=CharsNoConv;
#endif // ENCODINGS_AVAILABLE
Helper::dealWithHelpSwitch(theEnvironment);
outputBodyTag(theEnvironment);
// From this point forward, the user's options are set
#ifdef SPECIFIC_BAN_STRSTR
if(getenv("REMOTE_HOST") && stristr(getenv("REMOTE_HOST"),SPECIFIC_BAN_STRSTR)) theEnvironment->h_puts(SPECIFIC_BAN_HTML);
else
#endif
#ifdef LOCAL_IP_PREFIX
if(userIsLocal(theEnvironment) || dealWithUnlocalUser(theEnvironment)) {
#endif
if(theEnvironment->getName(HELP_ENV,MAY_RETURN_NULL) && !theEnvironment->getName(ENV_SHOW_OPTIONS,MAY_RETURN_NULL)) outputHelp(theEnvironment);
else if(theEnvironment->getName(ENV_SHOW_OPTIONS,MAY_RETURN_NULL) || !realString(theEnvironment->getName(URL_ENV,MAY_RETURN_NULL))) {
// Showing options, or no URL in box
if (theEnvironment->getName(ENV_SHOW_OPTIONS,MAY_RETURN_NULL) && !strcmp(theEnvironment->getName(ENV_SHOW_OPTIONS,MAY_NOT_RETURN_NULL),L_EXIT)) {
// "Leave gateway" button has been pressed
// Go to the page that was in the URL box, or
// (if it was blank) go to the gateway website
if(realString(theEnvironment->getName(URL_ENV,MAY_RETURN_NULL))) redirectAndAbort(theEnvironment->getName(URL_ENV,MAY_RETURN_NULL));
else redirectAndAbort(WEBSITE);
} else outputAllOptions(theEnvironment);
} else {
// Getting a page
if(proxyProblem()) {
printProxyPage(theEnvironment);
#ifdef Is_Monash
} else if(isFromOutside()) {
printUnwelcomeMessage(theEnvironment);
#endif
} else {
if(isFromOutside()) printWelcomeMessage(theEnvironment);
// careful - don't want welcome message in every single frame (Netscape sends referrer of page *before* main frame page) - have some kind of "don't welcome" key with a timeout?  No - have Javascript and status line (doesn't matter if not everybody sees it)
InString actualPage;
#ifdef Internal_Extensions1
ExtensionContainer* c=makeNewReturnOldExtensions();
#endif
FILE* inPage=NULL;
long posAtStart=theEnvironment->getPos();
try {
inPage=getWebPage(NULL,theEnvironment,MAY_NOT_RETURN_NULL,actualPage,theEnvironment,theEnvironment->getName(ENV_NOT_VI,MAY_RETURN_NULL) || theEnvironment->getName(ENV_PREFIX "k",MAY_RETURN_NULL),1);
} catch(CatchLiteral msg) {
// There was a problem getting the page.
// Might still need to output the options link (if getWebPage hasn't already done it)
if(!theEnvironment->hasAdvancedSince(posAtStart) && outputOptionsLink(theEnvironment))
outputStartPageLine(theEnvironment);
// Now report the problem
theEnvironment->outHTMLParagraph(L_RETRIEVE_PROBLEM);
const char* x=theEnvironment->getName(URL_ENV,MAY_NOT_RETURN_NULL);
if(!strncmp(x,"//",2)) x=x+2; // A bit nasty
theEnvironment->h_fputs("<A HREF=\""); if(needProtocolFor(x)) theEnvironment->h_fputs("http://");
theEnvironment->h_fputs(x);
theEnvironment->h_fputs("\">");
theEnvironment->h_fputs(L_RETRPROB_LINKTEXT);
theEnvironment->h_fputs("</A>");
theEnvironment->outHTMLParagraph(L_RETRIEVE_PROB_2);
theEnvironment->h_puts(msg);
theEnvironment->outHTMLParagraph(L_RETRIEVE_PROB_3);
theEnvironment->h_puts("<HR>"); // Don't change to h_fputs (see comment in access.c++ about Mozilla bug)
outputLastUpdate(theEnvironment);
inPage=NULL; // just to make sure (in case of compiler bugs etc)
}
if(inPage) {
theEnvironment->setStartOfRealPage(); // In case it's a frameset, the user's a sightie and we want to ditch the speil we've added so far
try {
extern bool wasPlainText;
// (see webget.c++)
htmlFilter(theEnvironment,inPage,actualPage.getString(),wasPlainText);
} catch(...) { fclose(inPage); throw; }
fclose(inPage);
}
#ifdef Internal_Extensions1
resetOldExtensions(c);
#endif
}
}
#ifdef LOCAL_IP_PREFIX
}
#endif
} catch(CatchLiteral x) {
theEnvironment->exceptionHandle(x);
} catch(CatchBroken x) {
theEnvironment->exceptionHandle("This server has not been correctly configured - please check BROKEN_EXCEPTION_HANDLING in platform.h");
} catch(RatingWarning x) {
// This must be done at this level (not after
// fetching the page), since htmlFilter can fetch
// further pages (e.g. as frames) that may also have
// ratings
handleRatingWarningException(theEnvironment);
} catch(...) {
theEnvironment->exceptionHandle("Unhandled exception type!");
}
delete theEnvironment;
} catch(const CgiExceptionDealtWith* c) {
// Exception arose in creation of environment
// but it has been dealt with, so we don't have
// to do anything (just exit)
} catch(CgiExceptionDealtWith* c) {
// Same again for the broken compilers
} catch(...) {
// Very little we can do about this one
// (well, printing the following might help a little)
puts("Content-type: text/plain\n\nmain.c++: Unhandled exception\nin creation of environment\n");
}
#ifdef MAKE_FREQUENCY_TABLE
void exitFrequencyTable();
exitFrequencyTable();
#endif
removeTempFiles(); return 0;
}

#ifdef NEED_TERMINATE_FUNCTION
void terminate(void) {
puts("Hello world, this is terminate()");
}
#endif
