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
#include "platform.h" /* above stdio.h, for _GNU_SOURCE and vsnprintf */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#ifdef MEMCPY_IS_IN_MEM_DOT_H
#include <mem.h>
#endif
#include "cgilib.h"
#include "literals.h"
#include "!debug.h"
#include "debug.h"
#include "webget.h"
#include "images.h"
#include "lastupd.h"
#include "exceptn.h"

#ifdef DEVELOPMENT_COPY
extern const char* qsOverride;
#define getQueryString (qsOverride?qsOverride:getenv("QUERY_STRING"))
#else
#define getQueryString getenv("QUERY_STRING")
#endif

#define FILE_LEN 8
#define FULL_PATHLEN 100

#define TB_LEN 1024  // For h_printf
#define TB_IS_STATIC // For h_printf

#ifndef BRIEF_RESPONSE
#ifdef WEB_SERVER_ADDS_CHARACTER_13
int crCount(const char* string) {
const char* s=string; int count=0;
while((s=strchr(s,'\n'))!=NULL) { count++; s=s+1; }
return(count);
}
#endif
#endif

void CGIEnvironment::outHTMLCharacter(int c) {
const char* translateString[/*255*/ 127]={
"", // 0: DO NOT USE
"<b>","</b>","<i>","</i>","<HR>","<h2>", // 1 to 6
"\x07",
"</h2>","<h3>","</h3>","","","","",
"&curren;", // 15
"","","","", // 16-19
"&para;","&sect;", // 20-21
"","","","","","","","","","", // 22-31
" ","!","\"","#","$","%","&amp;","'","(",")","*","+",
",","-",".","/","0","1","2","3","4","5","6","7","8","9",":",
";","&lt;","=",">","?","@",
"A","B","C","D","E","F","G","H","I","J","K","L","M","N",
"O","P","Q","R","S","T","U","V","W","X","Y","Z",
"[","\\","]","^","_","`",
"a","b","c","d","e","f","g","h","i","j","k","l","m","n","o","p","q","r","s","t","u","v","w","x","y","z",
"{","|","}"
// ,"~&brvbar;", // This is 127
// Commenting out this lot - not used by the gateway:
//"&Ccedil;","&uuml;","&eacute;","&acirc;","&auml;","&agrave;",
//"&aring;","&ccedil;","&ecirc;","&euml;","&egrave;","&iuml;",
//"&icirc;","&igrave;","&Auml;","&Aring;","&Eacute;","&aelig;",
//"&AElig;","&ocirc;","&ouml;","&ograve;","&ucirc;","&ugrave;",
//"&yuml;","&Ouml;","&Uuml;","&cent;","&pound;","&yen;","P","\x9f",
//"&aacute;","&iacute;","&oacute;","&uacute;","&ntilde;","&Ntilde;",
//"&ordf;","&ordm;","&iquest;","&not;","&not;","&frac12;","&frac14;","&iexcl;",
//"&laquo;","&raquo;","&brvbar;","&brvbar;","&brvbar;","&brvbar;","&brvbar;","&brvbar;",
//"&brvbar;","+","+","&brvbar;","&brvbar;","+","+","+","+","+",
//"+","-","-","+","-","+","&brvbar;","&brvbar;","+","+","-",
//"-","&brvbar;","-","+","-","-","-","-","+","+","+","+","+",
//"+","+","+","&brvbar;","_","&brvbar;","&brvbar;","&macr;",
//"a","&szlig;","G","p","S","s","&micro;","t","F",
//"T","O","?","8","f","e","n","=","&plusmn;", // Remove the ? here if too many
//"=","=","(",")","&divide;","~","&deg;","&middot;","&middot;",
//"v","n","&sup2;","&brvbar;"
};
if(c<0 || c>/*255*/126) h_putchar('?');
else h_fputs(translateString[c]);
}

void CGIEnvironment::outHTMLLine(const char* line) {
int oldLp=0;
for(int lp=0; line[lp]; lp++) {
if(line[lp]==' ' && line[lp+1]==' ') h_fputs("&nbsp;"); // Non-breaking space
else outHTMLCharacter(line[lp]);
if(lp-oldLp>60 && line[lp]==' ' && line[lp+1]!=' ') { oldLp=lp; h_putchar('\n'); }
}
}


class CgiInQuery : public CGIEnvironment::CgiInput {
public:
CgiInQuery() : CGIEnvironment::CgiInput() { ptr=0; theQueryString=getQueryString; }
// (need to fully qualify CGIEnvironment::CgiInput for gcc 2.7)
~CgiInQuery() {};
int getCharacter();
protected:
int ptr;
const char* theQueryString;
};


int CgiInQuery::getCharacter() {
int c=theQueryString[ptr++];
if(!c) { ptr--; return(EOF); }
else return(c);
}

class CgiInPost : public CGIEnvironment::CgiInput {
public:
CgiInPost() : CGIEnvironment::CgiInput() { lengthToGo=atoi(getenv("CONTENT_LENGTH")); }
// (need to fully qualify CGIEnvironment::CgiInput for gcc 2.7)
~CgiInPost() {};
int getCharacter();
protected:
int lengthToGo;
};

int CgiInPost::getCharacter() {
if(!lengthToGo) return(EOF);
else { lengthToGo--; return(getchar()); }
}

class CgiInCookies : public CgiInQuery {
public:
CgiInCookies(const char* myQueryString) : CgiInQuery() {
theQueryString=myQueryString;
}
~CgiInCookies() { delete[] theQueryString; }
};

// ==== Stuff for parsing multipart ====

// need to delete[] the return value
const char* escapeNulls(const char* data,int length,
int null,int subst,int escape) {
int extraBytes=1; // for a terminating null
for(int i=0; i<length; i++) if(data[i]==null || data[i]==escape) extraBytes++;
char* newData=new char[length+extraBytes];
int j=0;
for(int i=0; i<length; i++,j++) {
if(data[i]==null) { newData[j++]=escape; newData[j]=subst; }
else if(data[i]==escape) { newData[j++]=escape; newData[j]=escape; }
else newData[j]=data[i];
}
newData[j]=0; // terminating null
return newData;
}

const char* unescapeNulls(const char* data,int null,int subst,int escape,int &length) {
char* newData=new char[strlen(data)]; // (actually it doesn't have to be that big but...)
int j=0;
for(int i=0; data[i]; i++,j++) {
if(data[i]==escape) {
i++;
if(data[i]==subst) newData[j]=null;
else if(data[i]==escape) newData[j]=escape;
else throw "unescapeNulls: Invalid escape sequence";
} else newData[j]=data[i];
}
length=j; return newData;
}

enum EscapeStuff {
NullToUse = 0,
SubstToUse = 1,
EscapeToUse = 27 // Theoretically should choose the least common byte in any binary format
};

int newlineAdvance(const char* input) {
// Deal with all possible formats
if(*input=='\n') return((input[1]=='\r') ? 2 : 1);
else if(*input=='\r') return((input[1]=='\n') ? 2 : 1);
else return 0;
}

char* endOfBoundary(char* input,const char* boundary,int newLineBefore) {
if(newLineBefore) {
int n=newlineAdvance(input);
if(n) input+=n; else return NULL;
// NEEDATTENTION Is this correct?  i.e. browser never sends separator on the end of a line?
}
if(input[0]=='-' && input[1]=='-') input+=2; else return NULL;
if(!strncmp(input,boundary,strlen(boundary))) return(input+strlen(boundary));
else return NULL;
}

int CGIEnvironment::tryDecodingMultipart() {
#define Multipart_String "multipart/form-data"
#define Boundary_String "; boundary="
const char* e=getenv("CONTENT_TYPE");
if(e && !strncmp(e,Multipart_String,strlen(Multipart_String))) {
e+=strlen(Multipart_String);
const char* boundary="";
if(!strncmp(e,Boundary_String,strlen(Boundary_String)))
boundary=e+strlen(Boundary_String);
int lengthOfInput=atoi(getenv("CONTENT_LENGTH"));
// Might as well read it all into a buffer (going to need
// look-ahead anyway) - might not be as efficient (more copying)
// but these multipart forms don't occur very often (at the moment...)
char* input=new char[lengthOfInput];
const char* toDelete=input; // really need a "try" NEEDATTENTION
fread(input,sizeof(char),lengthOfInput,stdin);
input=endOfBoundary(input,boundary,0);
if(!input) throw "tryDecodingMultipart: Initial boundary not found";
int firstTime=1;
while(input-toDelete<lengthOfInput) {
// NEEDATTENTION That condition alone is not sufficient to prevent all segfaults if the browser sends incorrect data
int n=newlineAdvance(input);
if(n) input+=n; else if(!firstTime) break; // Reached the -- at EOF
firstTime=0;
// Content-Disposition: form-data; name="..."
// (NEEDATTENTION is "name" always first?)
input=strchr(input,'"');
if(!input) throw "tryDecodingMultipart: No quote on content-disposition line";
const char* name=input+1;
input=(char*)strchr(name,'"');
if(!input) throw "tryDecodingMultipart: Unterminated name";
*(input++)=0;
if(*input==';') {
// Something like "; Filename=...".  Since the
// access gateway never needs to use this type of
// input itself, we might as well record the lot,
// from here, for passing through.  Just make sure
// that we've recorded the fact that we've done
// this.  ***************
// (need some sort of "special multipart prefix" to "name")
const char* value=input; char* boundEnd;
while(!(boundEnd=endOfBoundary(input,boundary,1))) input++;
const char* newValue=escapeNulls(value,input-value,NullToUse,SubstToUse,EscapeToUse);
// addName(name,newValue);
// ****** (put this back in when it's passing on correctly)
delete[] newValue;
input=boundEnd;
} else {
// No "Filename" etc - just decode it
while(!newlineAdvance(input)) input++; input+=newlineAdvance(input);
input+=newlineAdvance(input); // the blank line
const char* value=input; char* boundEnd;
while(!(boundEnd=endOfBoundary(input,boundary,1))) input++;
*input=0; addName(name,value);
input=boundEnd;
}
}
// ********************************************
// need to add a "dump to multipart" (using what separator?)
// remember unescapeNulls
// (+ how to do it in lynx?)
// degrade to POST, so need a var that puts it to multipart
delete[] toDelete;
return 1;
} else return 0; // failed
}

// ==== End of multipart stuff ====

void CGIEnvironment::buildCGIInput(CgiInput &i,int dontReplaceExisting) {
InString name,value;
int inValue=0; char optionsToDecompress=0;
while(1) {
int c=i.getCharacter();
#ifdef DEBUGGING_CGI_INPUT
h_printf("Debugger: Got character %3d (%c)<BR>\n",c,c);
#endif
if(c==EOF || c=='&') {
//	    	if(!inValue) throw ((c=='&')?"Ampersand before equals on CGI input":"End of input before equality on CGI input");
// No, DON'T throw error: Access gateway can produce this
// Just ignore the rogue term
if(inValue) {
#ifdef DEBUGGING_CGI_INPUT
(*this)<<"<BR>Debugger: name="<<name.getString()<<" value="<<value.getString()<<"<BR>\n";
#endif
const char* nStr=name.getString();
if(!strncmp(nStr,"amp;",4)) nStr+=4; // cope with browsers that don't decode &amp; in links
if(!strcmp(nStr,CGI_COMPRESSION)) optionsToDecompress=value.getString()[0];
else if(!dontReplaceExisting || !getName(nStr,MAY_RETURN_NULL)) addName(nStr,value.getString());
name.clear(); value.clear();
inValue=0;
}
if(c==EOF) break;
} else if(c=='=') {
if(inValue)
// throw "Equals before ampersand on CGI input";
// We have to be tolerant of Netscape 3
value.addCharacter(c);
else inValue=1;
} else {
if(c=='%') {
// Get two-digit hex code
int hex1=i.getCharacter();
#ifdef DEBUGGING_CGI_INPUT
h_printf("Debugger: Got character %3d (%c)<BR>\n",hex1,hex1);
#endif
int hex2=i.getCharacter();
#ifdef DEBUGGING_CGI_INPUT
h_printf("Debugger: Got character %3d (%c)<BR>\n",hex2,hex2);
#endif
if (!(isascii(hex1) && isascii(hex2) && isxdigit(hex1) && isxdigit(hex2)))
throw "CGI input gave percent followed by invalid hex code";
hex1=toupper(hex1); hex2=toupper(hex2);
if(hex1>='A') c=hex1-'A'+10; else c=hex1-'0';
c<<=4;
if(hex2>='A') c|=hex2-'A'+10; else c|=hex2-'0';
} else if(c=='+') c=' ';
else if(!(isascii(c) && isprint(c))
&& (!(c&0x80))) {
// 2001-08-28: High-bit test added because some
// CJK pages have UTF etc in the filenames
throw "CGIEnvironment::buildCGIInput got non-printing character";
}
if(inValue) value.addCharacter(c); else name.addCharacter(c);
} // 'if' around c
} // while 1
if(optionsToDecompress) decompressOptions(optionsToDecompress); // After everything else, so overridden options are never replaced
}

void CGIEnvironment::h_puts(const char* string) {
whereToOutput->addString(string);
whereToOutput->addCharacter('\n');
}

void CGIEnvironment::h_printf(const char* format,...) {
// Patch for GNU to suppress warning:
#ifdef __GNUC_VA_LIST
#define va_list __gnuc_va_list
#endif
va_list argptr;
#ifdef __GNUC_VA_LIST
#undef va_list
#endif
va_start(argptr,format);
#ifdef TB_IS_STATIC
static
#endif
char tempBuf[TB_LEN]; tempBuf[TB_LEN-1]=0;
#ifdef _GNU_SOURCE
int r=vsnprintf(tempBuf,TB_LEN,format,argptr);
if(r==-1) throw "CGIEnvironment::h_printf buffer overflow (GNU vsnprintf used)";
#else
tempBuf[TB_LEN-1]=0;
 vsnprintf(tempBuf,sizeof(tempBuf),format,argptr);
if(tempBuf[TB_LEN-1]) throw "CGIEnvironment::h_printf buffer overflow (vsprintf used)";
#endif
else whereToOutput->addString(tempBuf);
va_end(argptr); // Clean up the multiple argument list
}

CGIEnvironment::CGIEnvironment(const char* cookiePrefix,const char* usrWantsCookiesVar)
: theCookiePrefix(cookiePrefix), theUsrWantsCookiesVar(usrWantsCookiesVar) {
try {
theRealPageStart=addArtificialNoframes=theUserSupportsCookies=0; addServiceTitle=1;
#ifdef MULTIPLE_STYLES_SUPPORTED
theStyle=NULL;
#endif
outputToMainPage();
theCharset=NULL; initOutputData();
const char* query_string=getQueryString;
if(query_string && query_string[0]) { CgiInQuery x; buildCGIInput(x); }
else if(!tryDecodingMultipart() &&
getenv("CONTENT_LENGTH")) { CgiInPost x; buildCGIInput(x); }

/* ***************************************************
else might not be on web (or might have no options)
see if on web first

INCLUDE_PROXY_CODE

Run as proxy from inetd?  What about logging ?????
(can we get the address from the stdin handle ??????)
Also if it's a proxy it will have to handle ALL content types!  (What about the overhead on spawning this program for all those CJK images !?)  More lightweight proxy that includes the image server & spawns this program as necessary?
If not on web & no args, get request from stdin (&
work out query string) (this as optional feature)
(also sort out different URL re-writing etc)
Watch denial of service "send infinite string" or "get infinite web page" (will time out, but will it thrash before then?)
M-x grep getenv *.c++ *.h
Use not BRIEF_RESPONSE (including in the signal
handlers!  grep Content-type.  But Content-length not
necessary)
*/

// See if the user sent any cookies.  If so, add them
// but make sure that the query options override them.
// (If the user has just disabled cookies, then the
// cookies they sent will still be valid at this point.
// However, they should be blanked; this is done
// when returning the page.)

const char* userCookies=getenv("HTTP_COOKIE");
if(userCookies && *userCookies) {
const char* cookieVal=getUserCookie(userCookies);
if(cookieVal) {
theOriginalCookie.addString(cookieVal);
CgiInCookies x(cookieVal);
buildCGIInput(x,1);
theUserSupportsCookies=1;
}
}
// We check optionIsRedundant as a separate pass,
// otherwise dontReplaceExisting would not work properly
// and a cookie option that is cancelled by a
// "redundant" query option will not be properly
// cancelled (eg. try setting a language and then
// going back to "No conversion" while using cookies
// and the old code)
for(int lp=0; lp<theData.getNumberOfValues(); lp++) {
if(optionIsRedundant(theData.getLhsByNumber(lp),theData.getRhsByNumber(lp))) theData.deleteByNumber(lp--);
}
// Initialise any remote cookies:
const char* cookies=getName(ENV_COOKIES,MAY_RETURN_NULL);
if(cookies) {
if(getName(ENV_PREFIX "B",MAY_RETURN_NULL)) setName(ENV_COOKIES,"");
else theCookies.readFromString(cookies);
}
} catch(CatchLiteral string) {
exceptionHandle(string); cleanup();
throw new CgiExceptionDealtWith;
}
}

CGIEnvironment::~CGIEnvironment() {
cleanup();
}

void CGIEnvironment::outputHeader(
#ifndef BRIEF_RESPONSE
long contentLength
#endif
) {
#ifdef DEVELOPMENT_COPY
extern int dontProcessLinks;
if(dontProcessLinks) return;
#endif
#ifndef BRIEF_RESPONSE
puts("HTTP/1.0 200 OK\nConnection: Close");
#endif
if(theCharset) printf("Content-Type: text/html; charset=%s\n",theCharset); // %s OK here
else puts("Content-Type: text/html");
if(userWantsCookies()) {
InString cQuery,cLowLevel;
cookieDumpCurrentOptions(cQuery);
if(strcmp(cQuery.getString(),theOriginalCookie.getString())) {
// Cookie has changed
encodeUserCookie(cQuery.getString(),cLowLevel);
fputs("Set-Cookie: ",stdout);
fputs(cLowLevel.getString(),stdout);
puts("; domain=" IP_ADDRESS "; path=" CGI_LOCATION
"; expires=Tue, 29-Dec-2037 12:00:00 GMT");
// The expiry date is necessary because otherwise
// the cookie will only last for the session, and
// bookmarks won't keep their options.  Unfortunately
// it introduces a date dependency; I hope users won't
// expect their bookmarks to last beyond 2037.
// (2000-07-19: I'm setting it to 2037 to avoid any
// S2G problems that might be lurking in current
// browsers.)
}
} else if(theUserSupportsCookies && !(theOriginalCookie.isEmpty())) {
// They sent one, but they don't want it back!
// We'd better clear it, otherwise its options
// might interfere later.
puts("Set-Cookie: " COOKIE_QUOTES_LETTER "=; domain=" IP_ADDRESS "; path=" CGI_LOCATION);
}
#ifndef BRIEF_RESPONSE
if(contentLength>=0) printf("Content-Length: %ld\n",contentLength);
#endif
putchar('\n'); // Blank line
}

#ifdef CAN_SHOW_IMAGES
void addUnicodeURL(InString &url,const CGIEnvironment* c) {
const char* x=c->getName(ENV_UNICODE_URL,MAY_RETURN_NULL);
if(x && *x) {
url.addString(x);
if(x[strlen(x)-1]!='/') url.addCharacter('/');
} else url.addString(UNICODE_URL);
#ifdef MULTIPLE_STYLES_SUPPORTED
const char* style=c->getStyle();
if(style) {
url.addString(style);
url.addCharacter('/');
}
#endif
}
#endif

void CGIEnvironment::cleanup() {
try {
outputToMainPage();
#ifdef Script_Extensions
appendScriptExtensionsTo(this);
#endif
if(addArtificialNoframes) {
h_fputs("<NOFRAMES>");
h_fputs(L_NOFRAMES_MSG);
h_puts("</NOFRAMES></HTML>");
} else h_puts("</BODY></HTML>");
} catch(CatchLiteral string) {
exceptionHandle(string); h_puts("</BODY></HTML>");
}
theHeaderData.prepend("<HTML><HEAD>");
if(!preprocessOnly(this)) {
theHeaderData.addString(
"<META NAME=\"robots\" CONTENT=\"NOINDEX, NOFOLLOW\">\n"
"<META HTTP-EQUIV=\"X-Mediator-Name\" CONTENT=\"" CANONICAL_NAME "\">\n"
"<META HTTP-EQUIV=\"X-Mediator-Warning\" CONTENT=\"" CANONICAL_NAME "=");
theHeaderData.addString(L_PAGE_ALREADY_PROCESSED);
theHeaderData.addString("\">\n");
}
#ifdef CAN_SHOW_IMAGES
if(UsingImages) {
theHeaderData.addString("<BASE HREF=\"");
addUnicodeURL(theHeaderData,this);
theHeaderData.addString("\">\n");
}
#endif
if(addServiceTitle) {
theHeaderData.addString("<TITLE>");
theHeaderData.addString(SERVICE_TITLE);
theHeaderData.addString("</TITLE>");
}
theHeaderData.addString("</HEAD>\n");
outputHeader(
#ifndef BRIEF_RESPONSE
theOutputData.length()+theHeaderData.length()
#ifdef WEB_SERVER_ADDS_CHARACTER_13
+crCount(theOutputData.getString())+crCount(theHeaderData.getString())
#endif
#endif
);
#ifdef ENCODINGS_AVAILABLE
if(subHtmlShowCharsAs==CharsSJIS) {
theHeaderData.eucjToSjis();
theOutputData.eucjToSjis();
}
#endif
fputs(theHeaderData.getString(),stdout);
puts(theOutputData.getString());
}

#ifdef EXCEPTION_LOGFILE
void envDump(const char* varName,FILE* fp) {
const char* c=getenv(varName);
fputs(varName,fp); fputc(' ',fp); fputs(c?c:"null",fp);
fputc('\n',fp);
}
#endif

void CGIEnvironment::htmlEnvDump(const char* varName) {
const char* c=getenv(varName);
if(c) {
h_fputs("<BR>"); h_fputs(varName); h_putchar('=');
outHTMLLine(c);
}
}

void outputBodyTag(CGIEnvironment* theEnvironment); // Nasty
void CGIEnvironment::exceptionHandle(const char* string) {
#ifndef DONT_CLEAR_ON_EXCEPTIONS
initOutputData();
try { outputBodyTag(this); } catch(...) { initOutputData(); }
#endif
outHTMLParagraph(INTERNAL_ERROR);
#ifdef EXCEPTION_LOGFILE
outHTMLParagraph(INTERNAL_ERR_LOGGED);
#else
outHTMLParagraph(INTERNAL_ERR_CONTACT);
#endif
outHTMLParagraph(string);
// NOTE - DUPLICATE CODE WITH BELOW
htmlEnvDump("QUERY_STRING");
htmlEnvDump("CONTENT_LENGTH");
htmlEnvDump("REMOTE_HOST");
htmlEnvDump("REMOTE_ADDR");
htmlEnvDump("HTTP_USER_AGENT");
htmlEnvDump("HTTP_VIA");
htmlEnvDump("HTTP_REFERER");
htmlEnvDump("HTTP_COOKIE");
#ifdef EXCEPTION_LOGFILE
// NB Don't worry too much about parallelism here
// - it won't be the end of the world if there's a conflict
FILE* logFile=fopen(EXCEPTION_LOGFILE,"at");
if(logFile) {
time_t t=time(NULL);
fprintf(logFile,"%s (last compilation " __DATE__ " " __TIME__ ")\n",ctime(&t)); // %s OK here
// NOTE - DUPLICATE CODE WITH ABOVE
envDump("QUERY_STRING",logFile);
envDump("CONTENT_LENGTH",logFile);
envDump("REMOTE_HOST",logFile);
envDump("REMOTE_ADDR",logFile);
envDump("HTTP_USER_AGENT",logFile);
envDump("HTTP_REFERER",logFile);
envDump("HTTP_VIA",logFile);
envDump("HTTP_COOKIE",logFile);
fputs("Exception: ",logFile);
fputs(string,logFile); fputs("\n\n",logFile);
fclose(logFile);
#ifdef EXCEPTION_COMMAND
system(EXCEPTION_COMMAND);
#endif // EXCEPTION_COMMAND
} // else not that much we can do
#endif // EXCEPTION_LOGFILE
h_puts("<HR>"); // Don't change to h_fputs (see comment in access.c++ about Mozilla bug)
outputLastUpdate(this); // better do that, so they get the disclaimer (since this is an error message)
}

void CGIEnvironment::addName(const char* name,const char* value) {
#ifdef OLD_PREFIX
if(!strncmp(name,OLD_PREFIX,OLD_PREFIX_LEN)) name+=INCREMENT_OLD_TO_NEW;
#endif
// In this version, duplicate identifiers are acceptable
// (as used in third party web pages), but getName only
// returns the first instance (the list functions list
// all of them)
//    if(getName(name,MAY_RETURN_NULL) && strcmp(getName(name,MAY_NOT_RETURN_NULL),value))
//	throw "CGIEnvironment::addName: Duplicate identifier";
theData.addLine(name,value);
}

const char* CGIEnvironment::getName(const char* name,MayReturnNull mayReturnNull) const {
if(!strcmp(name,URL_ENV)) {
// NEEDATTENTION this is a horrible hack
// URL_ENV can be used as a prefix in the WAP stuff
int n=strlen(URL_ENV);
for(int i=0; i<theData.getNumberOfValues(); i++) {
const char* lhs=theData.getLhsByNumber(i);
if(!strncmp(lhs,name,n) && lhs[n]) {
// a prefix (that's not just URL_ENV) - takes
// priority
return lhs+n;
}
}
}
#ifdef DEBUGGING_CGI_INPUT
h_printf("Debugger: getCaseSensitiveName(%s)<BR>\n",name); // %s OK here (only used in debugging)
#endif
const char* r=theData.getCaseSensitiveValueOf(name); if(r) return(r);
#ifdef DEBUGGING_CGI_INPUT
h_puts("Debugger: name not found<BR>");
#endif
if(mayReturnNull==MAY_RETURN_NULL) return(NULL);
else throw "CGIEnvironment::getName - name not found";
}

const char* removeEscapes(const char* x) {
if(!strncmp(x,SSB22_ESCAPE,SSB22_ESCAPE_LEN)) return(x+SSB22_ESCAPE_LEN);
else return(x);
}

inline int isOKForQuery(int c) {
return (isalnum(c) || strchr(".:;/^*-_~@'",c));
// 2001-01-13: Not \" because it messes up link tags
}

int CGIEnvironment::queryDumpExcludePrefix(const char* prefix,InString &url,int addQuestionMark) const {
// NEEDATTENTION Duplicate code!
// Do NOT apply options compression to this lot.
// It is going to the remote web server!
checkPoint();
int retVal=0;
for(int lp=0; lp<theData.getNumberOfValues(); lp++) {
checkPoint();
const char* lhs=theData.getLhsByNumber(lp);
const char* lhs2=removeEscapes(lhs);
if(lhs2!=lhs || strncmp(lhs,prefix,strlen(prefix))) {
checkPoint();
if(retVal) url.addCharacter('&');
else if(addQuestionMark) url.addCharacter('?');
url.addString(lhs2); url.addCharacter('=');
const char* rhs=theData.getRhsByNumber(lp);
for(int i=0; rhs[i]; i++) {
if(isOKForQuery(rhs[i])) url.addCharacter(rhs[i]);
else if(rhs[i]==' ') url.addCharacter('+');
else {
char buf[5];
snprintf(buf,sizeof(buf),"%%%02X",(int)((unsigned char)rhs[i]));
url.addString(buf);
}
}
retVal=1;
}
}
return(retVal);
}

int CGIEnvironment::lynxDumpExcludePrefix(const char* prefix,FILE* fp) const {
// NEEDATTENTION Duplicate code!
// Do NOT apply options compression to this lot.
// It is going to the remote web server!
int retVal=0;
for(int lp=0; lp<theData.getNumberOfValues(); lp++) {
const char* lhs=theData.getLhsByNumber(lp);
const char* lhs2=removeEscapes(lhs);
if(lhs2!=lhs || strncmp(lhs,prefix,strlen(prefix))) {
if(retVal) fputs("&\n",fp);
fputs(lhs2,fp); fputc('=',fp);
const char* rhs=theData.getRhsByNumber(lp);
for(int i=0; rhs[i]; i++)
if(isOKForQuery(rhs[i])) fputc(rhs[i],fp);
else if(rhs[i]==' ') fputc('+',fp);
else fprintf(fp,"%%%02X",(int)((unsigned char)rhs[i]));
retVal=1;
}
}
if(retVal) fputs("\n---\n",fp);
return(retVal);
}

void CGIEnvironment::formDumpCurrentOption(const char* exact) {
const char *lhs,*rhs;
for(int lp=0; lp<theData.getNumberOfValues(); lp++) {
lhs=theData.getLhsByNumber(lp);
if(!strcmp(lhs,exact)
&& (relyOnCookies()==NoCookies
|| strncmp(lhs,theCookiePrefix,strlen(theCookiePrefix)))
&& (rhs=theData.getRhsByNumber(lp))/*[0]*/) {
// [0]: No!  Watch checkbox thing
outputHiddenOption(lhs,rhs);
break;
}
}
}

Subset* CGIEnvironment::selectOptionsToDump(const char* prefix,CookieStuff doRelyOnCookies,const char* except) const {
Subset* s=new Subset(theData.getNumberOfValues());
const char* lhs;
for(int i=0; i<s->length(); i++) {
lhs=theData.getLhsByNumber(i);
if(!(*prefix && strncmp(lhs,prefix,strlen(prefix)))
// *prefix - defensive coding - not sure about the implementations of strncmp out there (but haven't actually run into a problem)
&& (!except || strcmp(lhs,except))
&& (doRelyOnCookies==NoCookies || doRelyOnCookies==NoCookiesOrCompression
|| (doRelyOnCookies==RelyOnCookies && strncmp(lhs,theCookiePrefix,strlen(theCookiePrefix)))
|| (doRelyOnCookies==DoingCookies && !avoidSendingCookie(lhs)))
/*&& theData.getRhsByNumber(i)[0]*/) s->setTrue(i);
// Last condition: No!  Check box stuff!  optionIsRedundant is removed in the constructor, however.  Don't know why I put that condition in.
}
return s;
}

void CGIEnvironment::formButOneDumpCurrentOptions(const char* prefix,const char* except) {
Subset* s=selectOptionsToDump(prefix,relyOnCookies(),except);
char compressVal=compressOptions(s);
if(compressVal) {
char c[2]="\x0"; // so 2nd byte is 0
c[0]=compressVal;
outputHiddenOption(CGI_COMPRESSION,c);
// NB We're doing this, but most of the time it won't
// work, because makeform.c++ calls the options one
// by one.  However, it does work on the URL box.
}
for(int i=0; i<s->length(); i++) {
if(s->isTrue(i)) outputHiddenOption(theData.getLhsByNumber(i),theData.getRhsByNumber(i));
}
delete s;
}

int CGIEnvironment::urlDumpCurrentOptions(InString &str,const char* prefix,CookieStuff doRelyOnCookies) const {
Subset* s=selectOptionsToDump(prefix,doRelyOnCookies);
char compressVal=0;
if(doRelyOnCookies!=NoCookiesOrCompression) compressVal=compressOptions(s);
if(compressVal) {
str.addString(CGI_COMPRESSION "=");
str.addCharacter(compressVal);
}
int firstTime=!compressVal;
const char* rhs;
for(int i=0; i<s->length(); i++) {
if(s->isTrue(i)) {
if(!firstTime) str.addCharacter('&');
str.addString(theData.getLhsByNumber(i));
str.addCharacter('=');
rhs=theData.getRhsByNumber(i);
for(int j=0; rhs[j]; j++) {
if(isOKForQuery(rhs[j])) str.addCharacter(rhs[j]);
else if(rhs[j]==' ') str.addCharacter('+');
else {
char buf[5];
snprintf(buf,sizeof(buf),"%%%02X",(int)((unsigned char)rhs[j]));
str.addString(buf);
}
}
firstTime=0;
}
}
delete s;
return(!firstTime);
}

int CGIEnvironment::urlDumpCurrentOptions(const char* prefix) {
InString s;
int retVal=urlDumpCurrentOptions(s,prefix,relyOnCookies());
h_fputs(s.getString());
return retVal;
}

// compressOptions return value must be OK for a CGI query
// Can support up to 6 presets with a base64 encoding
// Not quite base64 because 0 has to map to 0 (option
// not present at all)
inline int compressEncode64(int sixbits) {
if(sixbits) {
if(sixbits <= 27) return(sixbits+('@'-1));
else if(sixbits <= (27+26)) return(sixbits+('a'-27-1));
else return(sixbits+('0'-(27+26)-1));
// (27+26+10=63, as required)
} else return 0;
}
inline int compressDecode64(int encoded) {
if(encoded) {
if(encoded>='@' && encoded<='Z')
return(encoded-('@'-1));
else if(encoded>='a' && encoded<='z')
return(encoded-('a'-27-1));
else return(encoded-('0'-(27+26)-1));
} else return 0;
}

#ifndef PRESET_OPTIONS
#endif

static const char* PresetOptions[]=
#ifdef PRESET_OPTIONS
PRESET_OPTIONS; // see platform.h
#else
{ "Aecb","000000","Aect","FFFF00","Aecl","00FF00","Aeca","FF0000","Aecv","00FFFF","AecL","0000C0","AecB","FFFFFF","Aech","8080FF","Aefs","5","Aesu",0,"Aez",0,"Aen",0,"AeK",0,  0,0, // @ - some sensible colours etc
"Aelt","Japanese","Aesi",0,"Aesu",0,"Aeso",0,"Ael",0,  0,0, // A - used for Japanese
"Aelt","Chinese","Aesi",0,"Aesu",0,"Aeso",0,"Ael",0,"Aeus","t", 0,0, // C - used for Traditional Chinese
// (don't forget this thing is encoded as binary + offset '@' - 1)
0 };
#endif

char CGIEnvironment::compressOptions(Subset* s) const {
int sixbits=0;
Subset unionToRemove(s->length());
int presetNum,bitPos; // Need that out of the loop for gcc 2.7
for(presetNum=0,bitPos=1; PresetOptions[presetNum]; bitPos<<=1) {
int found=1;
Subset toRemove(unionToRemove.length());
// (re-init toRemove every presetNum, because may need to abort)
for(;PresetOptions[presetNum]; presetNum+=2) {
if(found) {
// (That conditional should save some cycles,
// since we don't break out of this loop when we
// know the answer - need to advance presetNum)
int foundThis=0;
for(int i=0; i<s->length(); i++) {
if(s->isTrue(i) && !strcmp(theData.getLhsByNumber(i),PresetOptions[presetNum]) && (!PresetOptions[presetNum|1] || !strcmp(theData.getRhsByNumber(i),PresetOptions[presetNum|1]))) {
foundThis=1;
toRemove.setTrue(i);
break;
}
}
// If didn't find this option, can't find "all" the options
if(!foundThis) found=0; // and DON'T break, because
// we want to skip through the rest of the options
}
}
presetNum+=2; // again, to skip over the 0,0
if(found) {
sixbits|=bitPos;
// Add toRemove to unionToRemove
for(int i=0; i<toRemove.length(); i++) {
if(toRemove.isTrue(i)) unionToRemove.setTrue(i);
}
}
}
// Unset the relevant options of s
for(int i=0; i<unionToRemove.length(); i++) {
if(unionToRemove.isTrue(i)) s->setFalse(i);
}
return(compressEncode64(sixbits));
}

void CGIEnvironment::decompressOptions(char c) {
int sixbits=compressDecode64(c);
int presetNum,bitPos; // need that out of the loop for gcc 2.7
for(presetNum=0,bitPos=1; PresetOptions[presetNum]; bitPos<<=1) {
for(;PresetOptions[presetNum]; presetNum+=2) {
if(sixbits&bitPos) {
// (NB We need that condition INSIDE the loop;
// alternatively we'd have to write an "else skip
// over them" loop)
const char* name=PresetOptions[presetNum],
*value=PresetOptions[presetNum|1];
if(!value) value="1"; // ("" is risky - what if the user goes out of the preset and the empty string gets written on a hidden FORM option?  Use "1" to be safe)
if(!getName(name,MAY_RETURN_NULL)) addName(name,value);
// NEVER add to an existing name here.  Might want
// the existing names to override (as in CGI options
// overriding cookies); also we're using unions
// in compress.
}
}
presetNum+=2; // again, to skip over the 0,0
}
}
