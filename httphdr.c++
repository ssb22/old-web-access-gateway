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
#include "httphdr.h"
#include "strfunc.h"
#include "instring.h"
#include "!debug.h"
#include "tagstuff.h"
#include "extensions.h"
#include "cgilib.h"
#include "debug.h"
#include <ctype.h>
#include <stdlib.h> // for system() (if not USE_POSIX_FORK)

#define TEMP_BUFLEN 80

int text_fgetc(FILE* stream);

void blankLineSeekPatch(FILE* fp,long pos) {
// 'Orrible nasty patch to get round bugs in
// some compilers with text mode file seeking
// Harmless in others (at least, the way the
// access gateway uses it)
/*if(pos>=4L) pos-=4L; else*/ pos=0L;
fseek(fp,pos,SEEK_SET);
while(text_fgetc(fp)!='\n' || text_fgetc(fp)!='\n') if(feof(fp)) /*throw "blankLineSeekPatch reached EOF"*/ break;
}

// isFTP==2 for forced interpretation as HTML
HttpHeader::HttpHeader(FILE* &fp,int isFTP,CGIEnvironment* theEnvironment,const char* alternativeFilenameForDecompression,const char* oldFilename,bool canThrow) {
checkPoint();
theSuccessStatus=1;
if(isFTP) {
if(readHttpEquivs(fp,theEnvironment) || isFTP==2)
addHeaderLine("Content-Type","text/html");
else addHeaderLine("Content-Type","text/plain");
fseek(fp,0L,SEEK_SET);
} else {
// Some proxies return a blank line to begin with,
// which confuses readMimeHeader, so is dealt with here.
// This has the side effect of dropping the first character
// from the "HTTP OK" thing, but that doesn't matter to readMimeHeader.
while(text_fgetc(fp)=='\n');
readMimeHeader(fp,canThrow);
if(theSuccessStatus) {
const char* x=getValueOf("Content-encoding"); if(x && (!stristr(x,"gzip") || !stristr(x,"compress"))) decompress(fp,alternativeFilenameForDecompression,oldFilename);
long oldPos=ftell(fp);
// Call readHttpEquivs, and also check that there is a
// content-type.  Some versions of the Apache proxy
// occasionally delete random HTTP response headers,
// including Content-Type, and this results in
// spurious "page is not an HTML document" errors when
// these proxies are used upstream.  So if there isn't
// a Content-Type, and if it does look like an HTML
// document, put one in.  2000-11-24.
if(readHttpEquivs(fp,theEnvironment) && !getValueOf("Content-Type")) addHeaderLine("Content-Type","text/html");
blankLineSeekPatch(fp,oldPos);
}
}
#ifdef DEBUGGING_HTTPHDR
printf("Debugger: HttpHeader::HttpHeader finished %s<BR>\n",theSuccessStatus?"OK":"in failure"); // %s OK here
#endif
}

int stripSpacesFgetc(FILE* fp) {
int ch=0;
do if((ch=text_fgetc(fp))==EOF) break; while(ch<=' ');
return(ch);
}

int HttpHeader::readHttpEquivs(FILE* fp,CGIEnvironment* theEnvironment) {
checkPoint();
// NEEDATTENTION Need to improve the efficiency here
// (do we really need to read through the whole file?
// Wouldn't just the header be better?)
// Also NB the comment --> / > thing though
// And now we've got extensions to worry about
int ch=0,isHtmlDocument=0;
while(ch!=EOF) {
ch=text_fgetc(fp);
if(ch=='<') {
HTMLTag theTag(fp);
if(!theTag.wasOnlyAComment()) {
#ifdef Internal_Extensions1
callExtensions(&theTag,theEnvironment,0);
#endif
if(!stricmp(theTag.getName(),"META")) {
const char* lhs=theTag.getAttributeFromLeft("HTTP-EQUIV");
if(lhs) {
const char* rhs=theTag.getAttributeFromLeft("CONTENT");
// Patch for some bad HTML:
const char* rhs2=theTag.getAttributeFromLeft("CHARSET");
InString s;
if(rhs2) {
s.addString(rhs);
s.addString(";charset="); s.addString(rhs2);
rhs=s.getString();
}
// End of patch
// puts(rhs); // correct at this point
if(rhs) addHeaderLine(lhs,rhs);
}
} else if(!stricmp(theTag.getName(),"HTML") || !stricmp(theTag.getName(),"HTM")) isHtmlDocument=1;
// (HTM added because some webmasters have unusual ideas)
}
}
} return(isHtmlDocument);
}

void HttpHeader::readMimeHeader(FILE* fp,bool canThrow) {
checkPoint();
InString s;
while(!feof(fp)) {
s.clear();
s.addLineFromFile(fp);
const char* str=s.getString();
if(!str[0]) return; // Reached the blank line
#ifdef DEBUGGING_HTTPHDR
printf("<BR>Debugger: HttpHeader::readMimeHeader: str=%s<BR>\n",str); // %s OK here (debugging)
#endif
const char* c=strchr(str,':');
if(c) {
((char*)c)[0]=0; // Might as well
do c=c+1; while(*c && isspace(*c));
addHeaderLine(str,c);
} // Else something like HTTP OK, so ignore
#ifdef DEBUGGING_HTTPHDR
else puts("-- Ignoring it<BR>");
#endif
} theSuccessStatus=0; // because no blank line
if(canThrow) throw "readMimeHeader: No blank line";
// (This can be caused by a lynx failure.  E.g. if USE_GNU_SOCKETS is not defined, and LYNX_NEEDS_TERM is not defined but needs to be.)
#ifdef DEBUGGING_HTTPHDR
puts("Debugger: No blank line!<BR>");
#endif
}

void callLynx(const char* lynxCommand,const char* inputFile,const char* outputFile,char*const* lynx_argv);

#ifndef GZIP_COMMAND
#error "In this version, you need to define GZIP_COMMAND as well as LYNX_COMMAND in platform.h."
#endif

void fileCopy(FILE* fp,const char* newName) {
enum { Buflen=255 };
char buf[Buflen+1];
size_t s;
FILE* newF=fopen(newName,"wb");
do {
s=fread(buf,1,Buflen,fp); fwrite(buf,1,s,newF);
} while(s);
// Do NOT use fgets instead of fread, because this is pure binary data that may well contain nulls
fclose(newF);
}

void HttpHeader::decompress(FILE* &fp,const char* alternativeFilenameForDecompression,const char* oldFilename) {
checkPoint();
// This is probably not a very good way of doing it, but....
#ifdef DEBUGGING_HTTPHDR
printf("Debugger: HttpHeader::decompress %s to %s<BR>\n",alternativeFilenameForDecompression,oldFilename); // %s OK here (debugging)
#endif
fileCopy(fp,alternativeFilenameForDecompression);
#define MAXARGS 8
const char* gzip_argv[MAXARGS]; int lPtr=0;
gzip_argv[lPtr++]=GZIP_COMMAND;
gzip_argv[lPtr++]="-df";
gzip_argv[lPtr++]="-";
#ifndef USE_POSIX_FORK
gzip_argv[lPtr++]="<";
gzip_argv[lPtr++]=alternativeFilenameForDecompression;
gzip_argv[lPtr++]=">";
gzip_argv[lPtr++]=oldFilename;
#endif
gzip_argv[lPtr]=NULL;
#ifdef USE_POSIX_FORK
callLynx(GZIP_COMMAND,alternativeFilenameForDecompression,oldFilename,(char*const*)gzip_argv);
#else
int lenCnt=0;
for(int i=0; i<lPtr; i++) lenCnt+=(strlen(gzip_argv[i])+1);
char* x=new char[lenCnt+1]; x[0]=0; // +1 perhaps not required but...
for(int i=0; i<lPtr; i++) {
strcat(x,gzip_argv[i]);
if(i<lPtr-1) strcat(x," ");
}
system(x);
delete[] x;
#endif

fp=fopen(oldFilename,"rb");
}

int HttpHeader::handleSetCookies(CookieTable &c) {
checkPoint();
int retVal=0;
for(int i=theHeaders.getNumberOfValues()-1;i>=0;i--) {
if(!stricmp(theHeaders.getLhsByNumber(i),"Set-Cookie")) {
c.processSetCookieHeader(theHeaders.getRhsByNumber(i));
retVal=1;
}
}
return retVal;
}
