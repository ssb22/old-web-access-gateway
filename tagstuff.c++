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
#include "tagstuff.h"
#include "webget.h"
#include "literals.h"
#include "subhtml.h"
#include "images.h"
#include "debug.h"
#include "action.h"
#include "ampersnd.h"
#include <ctype.h>
#include <string.h>
#ifdef MEMCPY_IS_IN_MEM_DOT_H
#include <mem.h>
#endif

#define T_Value "VALUE" // For the 2000-10-25 hack (below)

const char* problemExtentions[]={
"arc","arj","b64","bhx","gz","hqx","lzh","tar","taz","tgz","tz","uu","uue","xxe","z","zip","bz2", // Compressed files
"c","c++","cpp","h","java", // Source code
// "txt","text","asc", // Plain text
"wav","aif","au","ra","rm","ram","aiff","aifc","snd","mp2","mp3","mpg","m3u", // Audio files
"avi","mpeg","rpm","deb","swf", // Video (and linux packages)
"mid","smf","midi", // MIDI
"tga","xif","tif","tiff","pcd","png","pcx","bmp","drw","gif","jpe","jpeg","jpg","xbm", // Images
"wrl", // VRML
"pdf","eps","ps","dvi","tex", // Postscript stuff
"class", // Java class
"exe", // Careful!  Sometimes in CGI, but more often than not it isn't
// (2002-12-10: Making an execption for URLs with /cgi-bin or /scripts in them)
"sty", // LaTeX style file
"1st", // readme.1st
"readme", // .readme
"xls", // Excel spreadsheet
NULL};

inline int needUrlSubstitution(const char* lhs) {
checkPoint();
// If add any more stuff to this, have a data structure!
return(!(stricmp(lhs,"HREF")
&& stricmp(lhs,"SRC")
&& stricmp(lhs,"LOWSRC")
&& stricmp(lhs,"CODEBASE")
&& stricmp(lhs,"PLUGINSPAGE")
&& stricmp(lhs,"LONGDESC")
&& stricmp(lhs,"ACTION")
&& stricmp(lhs,"BACKGROUND")));
}

inline int cantHandleProtocol(const char* url) {
checkPoint();
const char* colon=strchr(url,':');
const char* slash=strchr(url,'/');
const char* sharp=strchr(url,'#');
return(colon && (!slash || colon<slash) &&
(!sharp || colon<sharp) &&
strnicmp(url,"http:",5) &&
#ifdef LYNX_SSL
strnicmp(url,"https:",6) &&
#endif
strnicmp(url,"ftp:",4));
}

inline int protocolOK(const char* url) {
return(!cantHandleProtocol(url));
}

// Separating the protocol check from the extensions
// etc check, because may have eg. FORM ACTION=mailto:...

int notAnHtmlDocument(const char* url,const CGIEnvironment* e) {
checkPoint();
#ifdef DEVELOPMENT_COPY
extern int dontProcessLinks;
if(dontProcessLinks) return(1);
#endif
// Check for anchor within current document
if(*url=='#') {
// 2000-06-22: Netscape 4.7 has some sort of bug whereby
// anchors within the current document are taken as
// referring to the BASE HREF (which is used by the
// character display stuff).  At least I think this is a
// bug; I haven't checked the spec.  At any rate, we can
// work around it by not returning 1 here if we've set
// the BASE HREF.
return(!UsingImages);
}
//    // Check for unusual protocols
if(cantHandleProtocol(url)) return(1);
// not necessary - already done - NO! (This function called elsewhere - this was a mistake)
// Don't only accept known extentions, because this won't work
// properly with CGI.  Instead, look for known
// problem extentions.  This of course won't always
// work, but it should at least help prevent large data files
// being sucked through the access gateway
// Update: There was a problem with .au (Australian) pages
// because it thought they were .au (audio) files!  Therefore
// it needs to check that there is a slash first
// This is the point of the enclosing conditional:
// "if the last slash is after the first dot"
// (doesn't matter if no dots - the rest won't work)
if(strrchr(url,'/')>strchr(url,'.')) {
const char* x=strrchr(url,'.'); if(x) { x=x+1;
for(int i=0; problemExtentions[i]; i++)
if(!stricmp(x,problemExtentions[i])) {
// 2002-12-10: Hacky exception for .exe's in cgi-bin
if(!stricmp(x,"exe") && (strstr(url,"/cgi-bin/") || strstr(url,"/scripts/"))) return 0;
return(1);
}
}
} return(0);
}

char* sortDirectories(const char* relativeHTML,const char* currentPage) {
// Note: If this function doesn't correctly strip down "../" relative URLs, check you're not using a buggy version of Mozilla that submits "." as %A9 (and a CGI script that echos the submitted URL, like FormMail)  (2002-01-16)
checkPoint();
if(!*relativeHTML) return(strnew(currentPage)); // for # links with the # stripped off
// Need to count out # and ? arguments from this lot
// NB Sort out this code - it's a bit messy
if(strchr(relativeHTML,'?') || strchr(relativeHTML,'#')) {
char* x=strnew(relativeHTML);
if(*x=='#' && !UsingImages) return(x); // For references within the same page
// 2000-10-06: The above line was commented out previously.  I commented it back in, adding UsingImages for the same reason as the UsingImages above (BASE HREF problem)
char *y=strchr(x,'?'),*z=strchr(x,'#');
int wasY=0;
if(y && (y<z || !z)) { *y=0; wasY=1; } else if (z) *z=0; else throw "sortDirectories assertion failure";
char* retVal=sortDirectories(x,currentPage);
InString url; url.addString(retVal); delete[] retVal;
if(wasY) { *y='?'; url.addString(y); }
else { *z='#'; url.addString(z); } delete[] x;
return(strnew(url.getString()));
} else if(strchr(currentPage,'?') || strchr(currentPage,'#')) {
char* x=strnew(currentPage);
char *y=strchr(x,'?'),*z=strchr(x,'#');
if(y && (y<z || !z)) *y=0; else if (z) *z=0; else throw "sortDirectories assertion failure";
char* retVal=sortDirectories(relativeHTML,x);
delete[] x; return(retVal);
} if(cantHandleProtocol(relativeHTML) ||
!strnicmp(relativeHTML,"http://",7) ||
#ifdef LYNX_SSL
!strnicmp(relativeHTML,"https://",8) ||
#endif
!strnicmp(relativeHTML,"ftp://",6)) return(strnew(relativeHTML));
else if(relativeHTML[0]=='/' && relativeHTML[1]=='/') {
return strnew(relativeHTML); // webget code should sort it out
} else {
InString url;
// Check for URLs like http:/users or ftp:subdir
if(!strnicmp(relativeHTML,"ftp:",4)) {
url.addString("ftp://");
relativeHTML=relativeHTML+4;
} else if(!strnicmp(relativeHTML,"http:",5)) {
url.addString("http://");
relativeHTML=relativeHTML+5;
}
#ifdef LYNX_SSL
else if(!strnicmp(relativeHTML,"https:",6)) {
url.addString("https://");
relativeHTML=relativeHTML+6;
}
#endif
else if(!strnicmp(currentPage,"ftp:",4)) {
url.addString("ftp://");
currentPage=currentPage+4;
}
#ifdef LYNX_SSL
else if(!strnicmp(currentPage,"https:",6)) {
url.addString("https://");
currentPage=currentPage+6;
}
#endif
else url.addString("http://");
if(!strnicmp(currentPage,"http:",5)
#ifdef LYNX_SSL
|| !strnicmp(currentPage,"https:",6)
#endif
|| !strnicmp(currentPage,"ftp:",4)) currentPage=strchr(currentPage,':')+1;
if(*currentPage=='/') currentPage=currentPage+((currentPage[1]=='/')?2:1);
if(relativeHTML[0]=='/')  {
// Just take the domain name out
const char* slash=strchr(currentPage,'/');
int q=(slash?(slash-currentPage):strlen(currentPage));
for(int i=0; i<q; i++) url.addCharacter(currentPage[i]);
url.addString(relativeHTML);
} else {
// NEEDATTENTION This code needs cleaning up (also optimising?)
const char* sentenel=strrchr(currentPage,'/');
// The next line assumes that, in an address like
// "http://members.aol.com/realjapan", "realjapan" is a directory
// It is now commented out, because it caused problems with CGIs
// (Servers should re-direct to realjapan/)
// if(sentenel && sentenel[1] && !strchr(sentenel,'.')) sentenel+=strlen(sentenel);
const char* afterServer=strchr(currentPage,'/');
// afterServer may be null
// If !afterServer, currentPage address is just http://x.y.z
// Check for ../ :
int z=0;
for(;relativeHTML[z];z+=3) if(!strncmp(relativeHTML+z,"../",3)) {
if(afterServer && sentenel>afterServer) {
do sentenel=sentenel-1; while(sentenel>afterServer && sentenel[0]!='/');
}
} else break; // (out of the 'for')
// Done checking for ../
if(!afterServer || sentenel<afterServer) sentenel=NULL;
int q=(sentenel?(sentenel-currentPage):strlen(currentPage));
for(int i=0; i<=q; i++) url.addCharacter(currentPage[i]?currentPage[i]:'/');
url.addString(relativeHTML+z);
}
return(strnew(url.getString()));
}
}

HTMLTag::HTMLTag(FILE* in) {
int dummy=0;
construct(0,in,theTagName /* unused InString */,dummy);
}

int text_fgetc(FILE* stream);
inline int htmlGetc(int usingSubHtml,FILE* in,InString &htmlLineInput,int &lineInputPtr) {
return(usingSubHtml?subHtmlFgetc(in,htmlLineInput,lineInputPtr):text_fgetc(in));
}

int HTMLTag::useOldCommentStyle=0;
char undocEnableIgnore=0;

void HTMLTag::construct(int usingSubHtml,FILE* in,InString &htmlLineInput,int &lineInputPtr) {
checkPoint();
int ch; InString lhs,rhs; theCommentStatus=0;
while(1) {
if((ch=htmlGetc(usingSubHtml,in,htmlLineInput,lineInputPtr))==EOF) return;
else if(isspace(ch) || ch=='>') break;
else if(ch!='\"' && ch!='\'') { // Condition as patch for pages with surplus quotes in tag names
theTagName.addCharacter(ch);
if(theTagName.length()==3 && !strcmp(theTagName.getString(),"!--")) {
theCommentStatus=1;
// Read through to -->, or > if the file does not contain -->s
// (do this on the preprocessing run)
// Seeking can have problems with the fpos bug,
// but this shouldn't matter in this case
long pos=0; if(!usingSubHtml && !useOldCommentStyle) pos=ftell(in);
int backslashHack=0; // For "gateway ignore this" comments
if(undocEnableIgnore) {
ch=0; while(ch!='-' && ch!='>' && ch!=EOF) {
ch=htmlGetc(usingSubHtml,in,htmlLineInput,lineInputPtr);
theTagName.addCharacter(ch);
if(theTagName.length()==(BACKSLASH_ENABLE_LEN+3)) {
if(!strcmp(theTagName.getString(),"!--" BACKSLASH_ENABLE_TOKEN)) backslashHack=1;
break;
}
}
} else ch=htmlGetc(usingSubHtml,in,htmlLineInput,lineInputPtr);
while(1) {
if(useOldCommentStyle) { if(ch=='>') break; }
else {
int brk=0;
while(ch=='-' && htmlGetc(usingSubHtml,in,htmlLineInput,lineInputPtr)=='-') {
do ch=htmlGetc(usingSubHtml,in,htmlLineInput,lineInputPtr); while(ch=='-');
while(isspace(ch)) ch=htmlGetc(usingSubHtml,in,htmlLineInput,lineInputPtr);
if(ch=='>') brk=1;
}
if(brk) break;
else if(ch=='\\' && backslashHack) htmlGetc(usingSubHtml,in,htmlLineInput,lineInputPtr);
}
if(ch==EOF) {
if(!usingSubHtml && !useOldCommentStyle) {
fseek(in,pos,SEEK_SET);
useOldCommentStyle=1;
} else break; // throw "Untruncated comment";
} ch=htmlGetc(usingSubHtml,in,htmlLineInput,lineInputPtr);
} return;
}
}
}
// Finished reading theTagName
// if(theTagName.isEmpty()), it wasn't really a tag
// but we want to output &lt; followed by ch
// (which will normally be > or whitespace)
// 2000-10-25: Added a hack to represent this as a tag
// Note that xhtml.c++ now depends on this hack
if(theTagName.isEmpty()) {
rhs.addString("&lt;"); rhs.addCharacter(ch);
addAttribute(T_Value,rhs.getString());
} else while(ch!='>') {
while(isspace(ch) || ch=='\"' || ch=='\'') if((ch=htmlGetc(usingSubHtml,in,htmlLineInput,lineInputPtr))==EOF) return;
// Quote condition as patch to avoid surplus quotes in tags
if(ch=='>') break;
do {
if(ch!='\"' && ch!='\'') // Condition as patch to avoid surplus quotes in tag attribute names
lhs.addCharacter(ch);
ch=htmlGetc(usingSubHtml,in,htmlLineInput,lineInputPtr);
if(ch==EOF) return; // Probably corrupt HTML (but we're supposed to do something)
} while(!isspace(ch) && ch!='>' && ch!='=');
while(isspace(ch) || ch=='\"' || ch=='\'') if((ch=htmlGetc(usingSubHtml,in,htmlLineInput,lineInputPtr))==EOF) return;
if(ch=='=') {
do if((ch=htmlGetc(usingSubHtml,in,htmlLineInput,lineInputPtr))==EOF) return; while(isspace(ch));
char firstQuoteChar=0;
if(ch=='\"' || ch=='\'') firstQuoteChar=ch;
else if(ch=='>') {
// Strange <TYPE XYZ=> tag
theAttributes.addLine(lhs.getString(),"");
return;
} else rhs.addCharacter(ch);
while(1) {
if((ch=htmlGetc(usingSubHtml,in,htmlLineInput,lineInputPtr))==EOF) break;
if(firstQuoteChar && ch==firstQuoteChar) { ch=htmlGetc(usingSubHtml,in,htmlLineInput,lineInputPtr); break; }
else if(!firstQuoteChar && (ch=='>' || isspace(ch))) break;
else if(ch=='\"') ch='\'';
rhs.addCharacter(ch);
}
}
addAttribute(lhs.getString(),rhs.getString());
// 2000-08-27 NEEDATTENTION: If lhs is "/" and rhs is empty, should we strip it?  (XHTML has <br /> tags etc, which could potentially confuse things although it shouldn't really be a problem)
lhs.clear(); rhs.clear();
}
}

int attributeNeedsQuote(const char* attrib) {
if(!*attrib) return 1; // always quote the empty string
for(int i=0; attrib[i]; i++) {
if(!isalnum(attrib[i])) return 1;
}
return 0;
}

int HTMLTag::outputIfNotReallyATag(CGIEnvironment* e) const {
// 2000-10-25 hack, see above
if(theTagName.isEmpty()) {
e->h_fputs(getAttribute(T_Value));
return 1;
} else return 0;
}

void HTMLTag::debugDump(CGIEnvironment* e) const {
e->h_fputs("<P>Tag name: ");
e->h_fputs(theTagName.getString());
e->h_fputs("<UL>");
int numVals=theAttributes.getNumberOfValues();
for(int i=0; i<numVals; i++) {
const char *x=theAttributes.getLhsByNumber(i),*y=theAttributes.getRhsByNumber(i);
e->h_fputs("<LI>");
e->h_fputs(x);
e->h_putchar('=');
e->h_fputs(y);
}
e->h_fputs("</UL>");
}

void HTMLTag::outputTo(CGIEnvironment* e) const {
// NEEDATTENTION Duplicate code (see below)
checkPoint();
if(outputIfNotReallyATag(e)) return;
e->h_putchar('<'); e->h_fputs(theTagName.getString());
int numVals=theAttributes.getNumberOfValues();
for(int i=0; i<numVals; i++) {
const char *x=theAttributes.getLhsByNumber(i),*y=theAttributes.getRhsByNumber(i);
e->h_putchar(' '); e->h_fputs(x);
if(*y) {
e->h_putchar('=');
int needQuote=attributeNeedsQuote(y);
if(needQuote) e->h_putchar('\"');
e->h_fputs(y);
if(needQuote) e->h_putchar('\"');
}
}
e->h_putchar('>');
}

void HTMLTag::outputTo(FILE* out) const {
// For the XHTML stuff.
// NEEDATTENTION Duplicate code (see above)
// (although this version is XML-ified - always quote)
// 2002-03-22: and don't o/p duplicate attribute names (e.g. multiple STYLE) (NEEDATTENTION they're just deleted)
// 2002-07-04: and do ampersand substitution in values (NEEDATTENTION might not be completely right)
checkPoint();
if(theTagName.isEmpty()) {
// (inlining the 2000-10-25 hack)
fputs(getAttribute(T_Value),out);
return;
}
fputc('<',out); fputs(theTagName.getString(),out);
int numVals=theAttributes.getNumberOfValues();
for(int i=0; i<numVals; i++) {
const char *x=theAttributes.getLhsByNumber(i),*y=theAttributes.getRhsByNumber(i);
// Check for duplicates (only naive code needed here)
int foundDup = 0;
for(int j=0; j<i; j++) {
if(!stricmp(theAttributes.getLhsByNumber(j),x)) {
foundDup = 1; break;
}
}
if(!foundDup) {
fputc(' ',out); fputs(x,out);
if(strcmp(x,"/")) {
fputs("=\"",out); // (always quote in XML, and always have ="something" even if empty string)
InString i; ampersandFilter(y,i,1);
fputs(i.getString(),out);
fputc('\"',out);
}
}
}
fputc('>',out);
}

void HTMLTag::removeJavascript() {
for(int i=0; i<theAttributes.getNumberOfValues(); i++)
if(!strnicmp(theAttributes.getLhsByNumber(i),"ON",2)
&& stricmp(theAttributes.getLhsByNumber(i),"ontimer")) // Exception for WAP (NEEDATTENTION this shouldn't be here)
theAttributes.deleteByNumber(i--);
}

void HTMLTag::setNameTo(const char* newName) {
theTagName.clear(); theTagName.addString(newName);
}

const char* leftStrip(const char* text) {
while(isspace(*text)) text++;
return(text);
}

void HTMLTag::makeURLsAbsolute(const char* currentPage) {
checkPoint();
#ifdef DEVELOPMENT_COPY
extern int dontProcessLinks;
if(dontProcessLinks) return;
#endif
int numVals=theAttributes.getNumberOfValues();
for(int i=0; i<numVals; i++) {
if(needUrlSubstitution(theAttributes.getLhsByNumber(i)))
theAttributes.setRhsDontNewIt(i,sortDirectories(leftStrip(theAttributes.getRhsByNumber(i)),currentPage));
// Also META CONTENT attributes with URL= in them
// hack for now NEEDATTENTION
if(!stricmp(theAttributes.getLhsByNumber(i),"CONTENT")) {
const char* fullVal=theAttributes.getRhsByNumber(i);
char* c=stristr(fullVal,"URL="); if(c) {
c[3]=0; // this hack is horrible
InString n; n.addString(fullVal);
n.addCharacter('=');
const char* dirs=sortDirectories(leftStrip(c+4),currentPage);
n.addString(dirs); delete[] dirs;
theAttributes.setRhs(i,n.getString());
}
}
}
}

enum { TooLongURL = 20, MuchTooLongURL = 30 }; // NEEDATTENTION ought to be somewhere else

char* HTMLTag::getStrippedHrefForAlt() {
checkPoint();
// This is the routine that tries to turn a URL into an
// ALT tag for its link.  URLs are getting increasingly
// cryptic so this is not easy.
const char* x=getAttribute("HREF");
if(x) {
// If the link is absolute and it points to a
// different domain, its beginning is important.
// (Especially if it's an advertising server with
// the referring page appended to the URL - we don't
// want to lose the fact that it's an advertisement
// rather than a page on the current site.)
// But strip any : or :// at the beginning.
int absolute=0;
const char* y=strchr(x,':'); if(y) { x=y+1; absolute=1; }
if(x[0]=='/' && x[1]=='/') { x=x+2; absolute=1; }
if(!absolute) {
// Except for any trailing /, begin after the last but one /:
// (NB Please don't change i>0 to i, since strlen-2 might be <0)
for(int i=strlen(x)-2,foundSlash=0; i>0; i--) if(x[i]=='/') {
foundSlash++; if(foundSlash==2) {
x=x+i+1; break;
}
}
}
char* z=strnew(x);
// Remove any trailing /, and those meaningless digits
// that visitor tracking systems like to add:
// (the >1 condition avoids problems with href="/";
// it does NOT mean >=1)
for(int i=strlen(z)-1; i>1; i--) if(strchr("0123456789/-",z[i])) z[i]=0; else break;
// Now remove .html and .htm (don't really need those)
// 2001-01-09: hacked to remove .stm et al also
char* c=strrchr(z,'.'); if(c && c[1] && !(strcmp(c+2,"tm") && strcmp(c+2,"tml") && strcmp(c+1,"asp"))) *c=0;
// 2001-01-09: We also want to remove "index" and "default" from this (assuming that there is something else!)
c=strrchr(z,'/'); if(c && !(strcmp(c+1,"index") && strcmp(c+1,"default"))) *c=0;
// If the URL is of the form
// domain/path/path/http://stuff
// then re-write it as domain://stuff
// (2001-01-09: Moved this above the query trimming so that some CGI-based sites are more meaningful)
c=strstr(z,"://"); char* c2=strchr(z,'/');
if(c2 && c>c2) memmove(c2,c,strlen(c)+1);
// See if the result is meaningful.  If it contains
// characters like ; or = then it's probably a
// query of sorts, and we don't want long queries.
static const char* funnyChars=";=?&%";
for(int i=0; funnyChars[i]; i++) {
if(strlen(z)<TooLongURL) break;
c=strchr(z,funnyChars[i]); if(c) *c=0; // trim it
}

// NEEDATTENTION Implement the next lot (but make sure
// it doesn't interfere with URLs of the above form)

// The URL might still be too long, especially if
// it has been taken from an absolute URL to a
// different server.  We do want to keep this fact
// and the document it is pointing to, but some of
// the middle directories can go.
// Use MuchTooLongURL
return(z);
} else return(NULL);
}

void outputCGIEncodedURL(CGIEnvironment* e,const char* url) {
checkPoint();
char hadHash=0;
for(int ij=0; url[ij]; ij++)
if(isalnum(url[ij]) || strchr(":/~.-_",url[ij]) || hadHash)
e->h_putchar(url[ij]);
else if(url[ij]=='#') { hadHash=1; e->h_putchar('#'); }
else if(!isspace(url[ij])) e->h_printf("%%%02X",(int)((unsigned char)url[ij]));
}

void outputModifiedURL(CGIEnvironment* e,const char* url) {
checkPoint();
if(e->urlDumpCurrentOptions(ENV_PREFIX)) e->h_putchar('&');
// (URL_ENV is last so any '#' present is easily dealt with)
e->h_fputs(URL_ENV "=");
if(!strncmp(url,"http://",7)) url+=7; // for compression
outputCGIEncodedURL(e,url);
e->h_putchar('\"');
}

#define ADOBE_CONVERTER "access.adobe.com/perl/convertPDF.pl" // NOT http:// because we'll be comparing it later (and gateway drops the http://)
void adobePDFHack(CGIEnvironment* e,const char* url) {
e->h_fputs(FORM_NEEDS_POSTING "=&" // Use method "post"
"extract_order=-d&"  // Default reading order
"reflow_p=1&"        // Reflow paragraphs
"url=");
outputCGIEncodedURL(e,url); // assume no # etc (NEEDATTENTION)
e->h_putchar('&');
outputModifiedURL(e,ADOBE_CONVERTER);
}
void adobePDFHackMessage(CGIEnvironment* e) {
const char* url1=e->getName(URL_ENV,MAY_RETURN_NULL);
if(url1 && !strcmp(url1,ADOBE_CONVERTER)) {
const char* url2=e->getName("url",MAY_RETURN_NULL);
if(url2) {
e->h_fputs("<BR>[<A HREF=\"");
e->h_fputs(url2); e->h_fputs("\">");
e->h_fputs(L_PDF_ORIG); e->h_fputs("</A>]");
}
}
}

void HTMLTag::outputWithModifiedURLs(const char* currentPage,CGIEnvironment* e,int canUseJavascript,int usePdfHack) {
checkPoint();
if(outputIfNotReallyATag(e)) return;
char* oldRel=getStrippedHrefForAlt();
makeURLsAbsolute(currentPage);
if(preprocessOnly(e)) outputTo(e); else {
const char* needSomeAction=0;
e->h_putchar('<'); e->h_fputs(theTagName.getString());
int numVals=theAttributes.getNumberOfValues();
for(int i=0; i<numVals; i++) {
const char *x=theAttributes.getLhsByNumber(i),
*y=theAttributes.getRhsByNumber(i);
if(*y) {
int actionTagNow=!stricmp(x,"ACTION");
if(actionTagNow) needSomeAction=y;
// Hack: Never substitute the SRC tag of an EMBED
int isEmbedSrc=(!stricmp(x,"SRC") && !stricmp(theTagName.getString(),"EMBED"));
int canSubstitute=(protocolOK(y) && !isEmbedSrc && stricmp(theTagName.getString(),"APPLET") && (actionTagNow || (needUrlSubstitution(x) && (usePdfHack || !notAnHtmlDocument(y,e)))));
if(oldRel && (canSubstitute || *oldRel=='#') && canUseJavascript && !stricmp(theTagName.getString(),"A") && !strchr(oldRel,'\'') && !strchr(oldRel,'\"')) {
// Can put JavaScript
// (Condition on *oldRel=='#' causes links within current page to still contain Javascript)
if(!getAttribute("onMouseOver")) {
e->h_fputs(" onMouseOver=\"window.status='");
e->h_fputs(L_ACCESS_STATUS_PREFIX);
for(int j=0; oldRel[j]; j++)
if(!isspace(oldRel[j])) e->h_putchar(oldRel[j]);
e->h_fputs("'; return true\"");
}
if(!getAttribute("onMouseOut"))
e->h_fputs(" onMouseOut=\"window.status=' '; return true\"");
}
e->h_putchar(' ');
e->h_fputs(x);
if(canSubstitute) {
// Can substitute
SwitchIfNeeded(e,y);
if(actionTagNow) {
e->h_fputs(MakeRelativeActionURL("=\"","\""));
} else {
e->h_fputs(MakeRelativeActionURL("=\"","?"));
if(usePdfHack) adobePDFHack(e,y);
else outputModifiedURL(e,y);
}
UnSwitch;
} else {
e->h_putchar('=');
int needQuote=attributeNeedsQuote(y);
if(needQuote) e->h_putchar('\"');
e->h_fputs(y);
if(needQuote) e->h_putchar('\"');
}
} else { e->h_putchar(' '); e->h_fputs(x); }
}
e->h_putchar('>');
if(needSomeAction) {
e->formDumpCurrentOptions(ENV_PREFIX);
e->h_fputs("<INPUT TYPE=hidden NAME=" URL_ENV " VALUE=\"");
e->h_fputs(needSomeAction);
e->h_fputs("\">");
}
}
if(oldRel) delete[] oldRel;
}

void HTMLTag::outputWithModifiedContent(const char* currentPage,CGIEnvironment* e) {
checkPoint();
if(outputIfNotReallyATag(e)) return;
makeURLsAbsolute(currentPage);
if(preprocessOnly(e)) outputTo(e); else {
e->h_putchar('<'); e->h_fputs(theTagName.getString());
int numVals=theAttributes.getNumberOfValues();
for(int i=0; i<numVals; i++) {
const char *x=theAttributes.getLhsByNumber(i),
*y=theAttributes.getRhsByNumber(i);
if(*y) {
// **********************************************
// META CONTENT - makeURLsAbsolute done
// Hack for now - NEEDATTENTION
const char* oldY=y;
y=stristr(y,"URL="); if(y) y+=4;
if(!stricmp(x,"CONTENT") && y && protocolOK(y) && !notAnHtmlDocument(y,e)) {
e->h_putchar(' ');
e->h_fputs(x); e->h_fputs("=\"");
e->h_fputs(oldY,y-oldY);
SwitchIfNeeded(e,y);
e->h_fputs(MakeActionURL("","?"));
UnSwitch;
outputModifiedURL(e,y);
} else {
e->h_putchar(' ');
e->h_fputs(x);
e->h_putchar('=');
int needQuote=attributeNeedsQuote(oldY);
if(needQuote) e->h_putchar('\"');
e->h_fputs(oldY);
if(needQuote) e->h_putchar('\"');
}
} else { e->h_putchar(' '); e->h_fputs(x); }
}
e->h_putchar('>');
}
}
