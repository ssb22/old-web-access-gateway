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
#include "subhtml.h"
#include "webget.h"
#include "literals.h"
#include "images.h"
#include "makeform.h"
#include "strfunc.h"
#include "extensions.h"
#include "htmlfilt.h"
#include "access.h"
#include "instring.h"
#include "debug.h"
#include "encoding.h"

#define HORRIBLE_NBSP_HACK

extern const char* smileys[];
extern const char* smileyFirstChars;

/// Filters the HTML (with ampersands) in sourceLine to the InString dest, writing IMG tags as necessary
// sourceLine is just text, not tags etc
void imageFilter(const char* sourceLine,InString &dest,int doubleIt,const CGIEnvironment* theEnvironment);

inline int checkSpace(int ch) {
return(ch && isspace(ch));
}
inline int checkNotSpace(int ch) {
return(ch && !isspace(ch));
}

void doSpaceDetect(InString & text) {
checkPoint();
// If match "? ? ? ", remove all spaces until the pattern stops
// NEEDATTENTION Webmasters might do it with &nbsp; - Sorted out with HORRIBLE_NBSP_HACK for now, which temporarily replaces &nbsp; with \r
const char* toOutput=text.getString();
for(int i=0; toOutput[i]; i++) {
if(!isspace(toOutput[i])
&& checkSpace(toOutput[i+1])
&& checkNotSpace(toOutput[i+2])
&& checkSpace(toOutput[i+3])
&& checkNotSpace(toOutput[i+4])
&& checkSpace(toOutput[i+5])) {
// Found the pattern
for(;toOutput[i];i++) {
if(isspace(toOutput[i])) {
//if(!isspace(toOutput[0]) && !isspace(toOutput[1])) spaceSuckingMode=0;
text.removeCharAt(i);
if(!toOutput[i]) i--; // because about to ++
// condition added as hack so &nbsp;&nbsp;&nbsp; -> real space (for pages that have three spaces between words - an ordinary space and a "layout" space either side of it)
} else if(checkNotSpace(toOutput[i+1])) {
// The pattern has broken (two consecutive non-space characters), so stop
break;
}
}
}
}
}

void innerHtmlFilter(CGIEnvironment* theEnvironment,FILE* in,const char* currentPage,int isPlainText,AccessOptions& options) {
InString textBetweenTags; // Buffers the "ordinary" text between the HTML tags
#ifdef CAN_SHOW_IMAGES
InString intermediate;
#endif
int ch=0;
char getNext=1,inPre=0; // For space detection stuff
char inAmpersandSequence=0; // For nobr stuff (don't put the extra spaces in the middle of HTML entities)
if(isPlainText && !options.removeNobr) {
textBetweenTags.addString("<PRE>"); inPre=1;
}
InString htmlLineInput;
while(1) {
// (Did say while(ch!=EOF), but now the EOF check is as
// a "break", because of the "getNext" thing)
if(getNext) ch=subHtmlFgetc(in,htmlLineInput,options.lineInputPtr);
else getNext=1;
if(ch=='<' && isPlainText) {
// Hack
textBetweenTags.addString("&lt"); ch=';';
}
if(ch=='<' || ch==EOF) {
int needToRestoreSetting=(options.inHead && !(options.canWriteInHead));
if(needToRestoreSetting) theEnvironment->outputBodyForNow();
if(options.spaceDetect)doSpaceDetect(textBetweenTags);
const char* toOutput=textBetweenTags.getString();
#ifdef CAN_SHOW_IMAGES
if(RealUsingImages && !options.inHead) {
imageFilter(textBetweenTags.getString(),intermediate,(subHtmlShowCharsAs==CharsDbl),theEnvironment);
toOutput=intermediate.getString();
}
#endif
if(options.smileySearch) {
// Hacky "smiley substitution" code
// This code is not fast enough!
while(*toOutput) {
while(*toOutput && strchr(smileyFirstChars,*toOutput)) {
/*register*/ int j; // here for optimisation (don't say register - it tends to make things awkward for compiler optimisers)
int found=0;
for(int i=0; /*strcmp(smileys[i],"END")*/ smileys[i][0]!='E' || smileys[i][1]!='N' || smileys[i][2]!='D' || smileys[i][3] ; i+=2) {
const char* optTemp=smileys[i]; // Just to save a lookup
// And just to be really nasty, let's even use
// a ... wait for it... a (you're not going to
// believe I'm doing this)... a...
for(j=0; optTemp[j]; j++) if(toOutput[j]!=optTemp[j]) goto lpCont;
// Now we're all right
theEnvironment->h_fputs(smileys[i|1]);
// j=strlen(optTemp); // should be already
toOutput+=j; found=1;
break;
lpCont: continue; // because label must be followed by statement
// (compiler should do jump optimisation
// anyway)
} // End of looping i through the smileys
if(!found) break;
} // End of while(first character is possibly the start of a smiley)
if(*toOutput) {
#ifdef HORRIBLE_NBSP_HACK
if(*toOutput=='\r') {
theEnvironment->h_fputs("&nbsp;");
if(options.removeNobr && toOutput[1] && !isspace(toOutput[1])) theEnvironment->h_putchar(' ');
} else
#endif
theEnvironment->h_putchar(*toOutput);
toOutput++;
} // end of if *toOutput
} // end of while *toOutput
} else { // Not smiley search - just dump it
if(*toOutput) {
#ifdef HORRIBLE_NBSP_HACK
char* c;
if(/*options.spaceDetect &&*/ (c=(char*)strchr(toOutput,'\r'))!=0) {
const char* d=toOutput; while(c) {
*c=0; theEnvironment->h_fputs(d);
d=c+1; c=(char*)strchr(d,'\r');
theEnvironment->h_fputs("&nbsp;");
if(options.removeNobr && *d && !isspace(*d)) theEnvironment->h_putchar(' ');
} theEnvironment->h_fputs(d);
} else
#endif
theEnvironment->h_fputs(toOutput);
}
}
if(needToRestoreSetting) theEnvironment->restoreOutputSetting();
textBetweenTags.clear();
#ifdef CAN_SHOW_IMAGES
intermediate.clear();
#endif
}
if(ch=='<') {
HTMLTag theTag(in,htmlLineInput,options.lineInputPtr);
if(!theTag.wasOnlyAComment()) {
#ifdef Internal_Extensions1
callExtensions(&theTag,theEnvironment,0);
#endif
if(options.processTag(theTag,htmlLineInput)) break;
if(!stricmp(theTag.getName(),"PRE")) inPre=1;
else if(!stricmp(theTag.getName(),"/PRE")) inPre=0;
}
} else if(ch==EOF) break;
else {
if(options.win1252 && inLatin1(theEnvironment)) {
const char* w1252data[]={
"#131;","f", // &fnof;, <i>f</i> - careful might already be in <i>
"#133;","...",
"#134;","+",
"#135;","++",
"#137;","/1000", // &permil;
"#145;","`",
"#146;","'",
"#147;","``",
"#148;","''",
"#149;","&bull;",
"#150;"," - ",
"#151;"," -- ",
"#153;","(TM)",
"#155;",">",
"#156;","oe",
// NEEDATTENTION Below we list the real Unicode
// versions - probably shouldn't be here.  These
// Unicode versions are displayed by modern
// Mozilla & Lynx but not by older browsers
// e.g. stable Dillo (2003-04-07)
"#8211;"," - ",
"#8212;"," -- ",
"#8213;","---",
"#9472;","---", // box drawing - sometimes used
"#8216;","`",
"#8217;","'",
"#8220;","``",
"#8221;","''",
"#8226;","&bull;",
"#8229;","..",
"#8230;","...",
0};
int need_continue=0;
for(int i=0; w1252data[i]; i+=2) {
int val=atoi(w1252data[i]+1);
if(ch=='&' && subHtmlTryGetString(w1252data[i],strlen(w1252data[i]),htmlLineInput,options.lineInputPtr)) ch=val;
if(ch==val) {
textBetweenTags.addString(w1252data[i|1]);
need_continue=1; break;
}
} // end for
if(need_continue) continue; // don't worry about isMeaningful etc - they're probably not
} // end if w1252
#ifdef HORRIBLE_NBSP_HACK
if(ch=='&' && options.spaceDetect && subHtmlTryGetString("nbsp;",5,htmlLineInput,options.lineInputPtr)) textBetweenTags.addCharacter('\r');
// else { if(ch=='\r') ch='\n'; // ch is never '\r'
else {
#endif
textBetweenTags.addCharacter(ch);
if(ch=='&') inAmpersandSequence=1;
else if(inAmpersandSequence && (ch==';' || (!isalnum(ch) && ch!='#'))) inAmpersandSequence=0;
if(isspace(ch)) options.charsSinceLastSpace=0;
else {
options.alreadyHadLinkBrackets=(ch=='[');
if(!inAmpersandSequence && options.removeNobr) {
options.charsSinceLastSpace++;
if((options.charsSinceLastSpace>WrapThreshold1 && !isalnum(ch)) || (options.charsSinceLastSpace>WrapThreshold2)) { textBetweenTags.addCharacter(' '); options.charsSinceLastSpace=0; }
}
}
if(/*!inAmpersandSequence &&*/ isMeaningful(ch)) {
if(options.needLinkImgNoAlt) { delete[] options.urlForLinkImgNoAlt; options.needLinkImgNoAlt=0; }
if(!options.inSelect && theEnvironment->outputtingToMainPage()) {
options.banner->meaningfulCharacterCounter++;
options.banner->hadMeaningfulThisRow++;
options.banner->hadMeaningfulThisCell++;
}
} else if(isspace(ch) && !inPre) {
getNext=0; int hadNYet=0;
do {
// Concatenate multiple spaces (for space
// detection routine), but make sure new lines
// result in a \n (browser compatibility etc)
if(ch=='\n') {
textBetweenTags.replaceLastCharWith(ch);
if(isPlainText) {
if(hadNYet) {
// Multiple newlines in plain text - have
// a paragraph marker
textBetweenTags.addString("<P>");
// NEEDATTENTION Really need to handle
// that as a tag properly - someone might
// want to replace the paragraphs with
// indentation or something - but need to
// split textBetweenTags etc
} else hadNYet=1;
} // isPlainText
} // ch=='\n'
ch=subHtmlFgetc(in,htmlLineInput,options.lineInputPtr);
} while(isascii(ch) && isspace(ch));
}
#ifdef HORRIBLE_NBSP_HACK
}
#endif
} // End of else (i.e. ch != EOF and != '<')
} // End of while !eof
}

void htmlFilter(CGIEnvironment* theEnvironment,FILE* in,const char* currentPage,int isPlainText,int frameNesting,BannerVariables* in_banner) {
checkPoint();
if(frameNesting>MAX_FRAME_NESTING) throw "Too many recursive frames";
LinkFollow::addURL(currentPage); // Just in case
AccessOptions options; memset(&options,0,sizeof(options));
options.banner = (in_banner?in_banner:new BannerVariables);
options.theEnvironment=theEnvironment; options.in=in; options.currentPage=currentPage; options.frameNesting=frameNesting;
options.initFromEnvironment();
#ifdef Internal_Extensions1
callExtensions(NULL,theEnvironment,0); // Indicate start of pass 2
#endif
// ------------------
innerHtmlFilter(theEnvironment,in,currentPage,isPlainText,options);
// ------------------
// theEnvironment->h_printf("<!-- Debugger: Considering banner move, frameNesting=%d bannerEndLocked=%d banner->meaningfulCharacterCounter=%d bannerDetectionThreshold=%d banner->meaningfulCtr2=%d bannerMinLength=%d bannerMaxLength=%d bannerDocMinLength=%d -->\n",frameNesting,options.banner->bannerEndLocked,options.banner->meaningfulCharacterCounter,bannerDetectionThreshold,options.banner->meaningfulCtr2,bannerMinLength,bannerMaxLength,bannerDocMinLength);
if(options.doBannerMove && !frameNesting &&
(options.banner->bannerEndLocked || options.banner->meaningfulCharacterCounter>bannerDetectionThreshold) &&
options.banner->meaningfulCtr2>bannerMinLength &&
options.banner->meaningfulCtr2<bannerMaxLength &&
(options.banner->meaningfulCharacterCounter+options.banner->meaningfulCtr2)>bannerDocMinLength &&
options.banner->bannerEnd) {
if(!options.banner->bannerEndLocked) options.banner->lockedBannerEnd = options.banner->bannerEnd;
// (Assuming that bannerMinLength is > 0, otherwise we might
// be in a frameset or something!)
theEnvironment->moveAChunkToTheEnd(options.banner->lockedBannerEnd);
// theEnvironment->h_printf("<!-- Debugger: Moved chunk up to %d -->\n",options.banner->lockedBannerEnd);
}
if(options.tableTrimmer) delete options.tableTrimmer;
if(options.linkFollower) delete options.linkFollower;
if(options.needToDeleteCurpage) delete[] options.currentPage;
if(!in_banner) delete options.banner;
if(options.addEndOfPage && !frameNesting) {
theEnvironment->outputToMainPage();
theEnvironment->h_fputs("<P>");
theEnvironment->h_puts(L_END_OF_PAGE);
}
}
