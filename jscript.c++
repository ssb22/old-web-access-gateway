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
#include "jscript.h"
#include "instring.h"
#include "subhtml.h"
#include "access.h"
#include "webget.h"
#include "swf.h"
#include "debug.h"
#include <ctype.h>
#include <string.h>

// Javascript heuristics: A class to spot ".htm"
// in Javascript scripts
class HtmSpotter {
public:
HtmSpotter() { reset(); }
~HtmSpotter() {};
void processChar(char ch) {
checkPoint();
backslashOverride=false;
if(ch=='\n') reset();
else if(ch=='\'' || ch=='\"'
// 2001-01-09: Put the following line in to
// handle the BBC News javascript; I'm not
// 100% sure of this.
// Original line: document.all.incoming.innerHTML = "<DIV ID=\"properties\"><DIV ID=\"itemcount\">1</DIV></DIV><DIV ID=\"stories\"><DIV ID=\"1\"><DIV ID=\"Summary\">UK news stories online.</DIV><DIV ID=\"SiteLink\">http://news.bbc.co.uk/hi/english/uk/default.htm</DIV><DIV ID=\"UrlLink\"></DIV></DIV></DIV>";
// Old code generated: JavaScript contains URL; guessing  [<A onMouseOver="window.status='Access news.bbc.co.uk/hi/english/uk/default.htm</DIV><DIVID'; return true" onMouseOut="window.status=' '; return true" HREF="/cgi-bin/access?Ac=@&Au=http://news.bbc.co.uk/hi/english/uk/default.htm%3C/DIV%3E%3CDIVID%3D">news.bbc.co.uk/hi/english/uk/default.htm</DIV><DIV ID</A>]
|| (addingToBuffer && (ch=='<' || ch=='>'))
) {
if(lastWasBackslash) { // eg. write("href=\"...\"");
if(validState()) {
backslashOverride=true;
quoteBuffer.truncateAt(quoteBuffer.length()-1);
} else if(addingToBuffer) quoteBuffer.clear();
} else {
if(ch=='\"') {
if(inSingleQuote && !inDoubleQuote) addingToBuffer=false; // so made true below, eg. write('href="..."');
inDoubleQuote=!inDoubleQuote;
} else if(ch=='\'') {
if(inDoubleQuote && !inSingleQuote) addingToBuffer=false; // so made true below, eg. write("href='...'");
inSingleQuote=!inSingleQuote;
}
addingToBuffer=!addingToBuffer;
if(addingToBuffer) quoteBuffer.clear();
}
} else if(addingToBuffer) quoteBuffer.addCharacter(ch);
lastWasBackslash=(ch=='\\');
}
int gotOne() const {
return((!addingToBuffer || backslashOverride) && validState());
}
const char* whatIsItThen() const;
void reset() {
checkPoint();
addingToBuffer=lastWasBackslash=backslashOverride=false;
quoteBuffer.clear();
inSingleQuote=inDoubleQuote=false;
}
private:
int validState() const {
const char* c=quoteBuffer.getString();
return(strNotEnd(c,"http://") ||
stristr(c,".htm") ||
strAtEnd(c,".asp") ||
strAtEnd(c,".jsp") ||
strAtEnd(c,".xml") ||
strAtEnd(c,".pl"));
// (If adding any more, have a proper table)
}
static bool strAtEnd(const char* str,const char* test) {
checkPoint();
int l1=strlen(str),l2=strlen(test);
return((l1>l2)?(!stricmp(str+l1-l2,test)):false);
// (> not >= since we don't JUST want the .pl etc)
}
static bool strNotEnd(const char* str,const char* test) {
checkPoint();
const char* c=stristr(str,test);
return (c && strlen(c)>strlen(test));
}
InString quoteBuffer;
bool addingToBuffer,lastWasBackslash,backslashOverride;
bool inSingleQuote,inDoubleQuote;
};

const char* HtmSpotter::whatIsItThen() const {
checkPoint();
// 2000-08-10: In Netscape's site there are links that
// have things in qutoes before the URL starts, but
// when it does it starts with http://
// Doing the following might go wrong with things like
// <A HREF="javascript:my.cgi?http://...">Enter proxy</A>
// but this is only *guessing* code
const char* c=quoteBuffer.getString();
const char* httpPos=strstr(c,"http://");
return(httpPos?httpPos:c);
}

void outputBR(CGIEnvironment* theEnvironment) {
theEnvironment->outputBodyForNow();
bool needToSetStartAgain=theEnvironment->nothingSinceStartOfRealPage();
theEnvironment->h_puts("<BR>");
if(needToSetStartAgain) theEnvironment->setStartOfRealPageForFramesetOnly();
theEnvironment->restoreOutputSetting();
}

/**
Does "scripty stuff", ie. deletes JavaScript scripts and so forth.
Optimisation: The 'if' is now in the caller, so it is assumed that givenTag
matches matchString.
@param givenTag The tag to looking at
@param matchString The string to match it against
@param disable non-0 if it should be disabled
@return non-0 if it dealt with the tag
*/
int AccessOptions::doScriptyStuff(const HTMLTag &givenTag,const char* matchString,int disable,FILE* in,InString &htmlLineInput,int guessURLs) {
checkPoint();

HtmSpotter* spotter=NULL;
if(guessURLs && disable) spotter=new HtmSpotter;

// optimisation: the 'if' is now in the caller
//	if(!stricmp(givenTag.getName(),matchString)) {
// disable is 1 if removing the stuff
if(!disable) givenTag.outputTo(theEnvironment);
int state=-1; bool justHadOne=false;
// This code is a bit hacky but...
while(1) {
int c=subHtmlFgetc(in,htmlLineInput,lineInputPtr);
if(c==EOF) {
// throw "HTML error: Unterminated scripty stuff";
return(1);
}
if(!disable) theEnvironment->h_putchar(c);
else if(guessURLs) {
spotter->processChar(c);
if(spotter->gotOne()) {
outputGuessedLink(spotter->whatIsItThen(),htmlLineInput,justHadOne);
justHadOne=true;
spotter->reset();
}
}
// See if we're done yet:
if(c>=' ' || state>=0) switch(state) {
case -1: if(c=='<') state=-2; break;
case -2: if(c=='/') state=0; else state=-1; break;
default: if(matchString[state]) {
if(toupper(c)==toupper(matchString[state])) state++;
else state=-1;
} else if(c=='>') {
if(justHadOne) outputBR(theEnvironment);
delete spotter; return(1);
} else if(!isspace(c)) state=-1;
break;
}
}
if(justHadOne) outputBR(theEnvironment);
delete spotter; return(0);
}

/// This deals with <SCRIPT SRC=...> elements (the source that is referenced is sometimes the only thing containing navigation)
int AccessOptions::doScriptSrcStuff(const HTMLTag &theTag,int disable,FILE* in,InString &htmlLineInput,int guessURLs,const char* currentPage,CGIEnvironment* theEnvironment) {
checkPoint();
if(guessURLs && disable) {
const char* src=theTag.getAttribute("SRC");
if(src) {
char* y=sortDirectories(src,currentPage);
try {
InString dummy;
FILE* srcFile=getWebPage(y,NULL,MAY_NOT_RETURN_NULL,dummy,theEnvironment,0,0,0,1);

HtmSpotter spotter; bool justHadOne=false;
while(1) {
int c=text_fgetc(srcFile);
if(c==EOF) break;
spotter.processChar(c);
if(spotter.gotOne()) {
outputGuessedLink(spotter.whatIsItThen(),htmlLineInput,justHadOne);
justHadOne=true;
spotter.reset();
}
}
if(justHadOne) outputBR(theEnvironment);
doScriptyStuff(theTag,"SCRIPT",1,in,htmlLineInput,0); // Catch the </SCRIPT>
fclose(srcFile); delete[] y; return 1;
} catch(...) {
// Never mind, it only means we can't use that for navigation
(void)0;
}
// watch returning here
delete[] y;
}
}
return 0;
}

// This is similar to the above but for Flash
int AccessOptions::doSwfStuff(const char* src,InString &htmlLineInput,const char* currentPage,CGIEnvironment* theEnvironment) {
checkPoint();
char* y=sortDirectories(src,currentPage);
try {
InString dummy;
FILE* srcFile=getWebPage(y,NULL,MAY_NOT_RETURN_NULL,dummy,theEnvironment,0,0,0,1);
swf2html(srcFile,this,theEnvironment,htmlLineInput);
fclose(srcFile); delete[] y; return 1;
} catch(...) {
// Never mind (only means can't use the swf file)
(void)0;
}
// watch returning here
delete[] y;
return 0;
}

int javascriptToHref(HTMLTag &tag) {
checkPoint();
// returns 1 on success; may assume it's a "javascript:"
const char* oldHref=tag.getAttribute("HREF");
if(oldHref) {
HtmSpotter spot;
for(int i=0; oldHref[i]; i++) {
spot.processChar(oldHref[i]);
if(spot.gotOne()) {
tag.deleteAllByName("HREF");
tag.addAttribute("HREF",spot.whatIsItThen());
return 1;
}
}
}
return 0;
}

void AccessOptions::outputGuessedLink(const char* link,InString &htmlLineInput,bool justHadOne) {
checkPoint();
theEnvironment->outputBodyForNow();
bool needToSetStartAgain=theEnvironment->nothingSinceStartOfRealPage();
theEnvironment->h_fputs(justHadOne?" ":L_JAVASCRIPT_GUESS);
int oldVal=processHrefImg; processHrefImg=1;
HTMLTag tag("A");
tag.addAttribute("HREF",link);
processTag(tag,htmlLineInput);
banner->meaningfulCharacterCounter+=strlen(L_JAVASCRIPT_GUESS);
processTag("/A",htmlLineInput);
processHrefImg=oldVal;
if(needToSetStartAgain) theEnvironment->setStartOfRealPageForFramesetOnly();
theEnvironment->restoreOutputSetting();
}

void AccessOptions::doScriptyStuff(const char* string,InString &htmlLineInput) {
checkPoint();
HtmSpotter spotter; bool justHadOne=false;
for(int i=0; string[i]; i++) {
spotter.processChar(string[i]);
if(spotter.gotOne()) {
outputGuessedLink(spotter.whatIsItThen(),htmlLineInput,justHadOne);
justHadOne=true;
spotter.reset();
}
}
if(justHadOne) outputBR(theEnvironment);
}
