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
#include "platform.h"
#include "exceptn.h"
#include "subhtml.h"
#include "access.h"
#include "htmlfilt.h"
#include "webget.h"
#include "literals.h"
#include "images.h"
#include "makeform.h"
#include "strfunc.h"
#include "extensions.h"
#include "jscript.h"
#include "debug.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>

/// Returns non-0 if the URL looks like a "main" page
/// (just the domain name) so that banners can be left alone
int isMainPage(const char* url) {
// [protocol:][//]some.domain.org[/] and nothing else
const char* c=strstr(url,"//"); if(c) url=c+2;
c=strchr(url,'/');
return(!c || !c[1]);
}

void checkSecurity(const char* href,CGIEnvironment* theEnvironment,const char* linkOrForm,const char* url) {
checkPoint();
if(href && !strnicmp(href,"https:",6)
// and we're not already in a secure page (otherwise we get far too many warnings):
&& strnicmp(url,"https://",8)) {
#ifdef LYNX_SSL
#ifdef CAN_SWITCH_SSL
#define security_should_be_ok (!(theEnvironment->getName(ENV_AVOID_SWITCH,MAY_RETURN_NULL)))
#else
extern bool UsingSSL;
#define security_should_be_ok UsingSSL
#endif
theEnvironment->h_printf(security_should_be_ok?L_SECURITY_2_SSL:L_SECURITY_2,linkOrForm);
#else
theEnvironment->h_printf(L_SECURITY_WARNING,linkOrForm);
#endif
}
}

#ifdef DEVELOPMENT_COPY
extern int dontProcessLinks;
#endif

void AccessOptions::initFromEnvironment() {
checkPoint();
#ifdef ENCODINGS_AVAILABLE
char userIsASightie=((theEnvironment->getName(ENV_NOT_VI,MAY_RETURN_NULL))!=NULL);
#else
#define userIsASightie 0
#endif
headingPromotionLevel=MAX_HEADING_LEVEL; if(!userIsASightie) {
const char* x=theEnvironment->getName(ENV_PREFIX "m",MAY_RETURN_NULL);
if(x && isdigit(*x)) headingPromotionLevel=(*x)-'0';
else headingPromotionLevel=DFLT_HEADING_PROMOTION;
}
// Note that we are negating the check boxes (so all unchecked gives all options)
// Putting userIsASightie in each one for maintainability
// (Also NB its use for refresh tags in webget.c++)
disableScript=!(/*userIsASightie ||*/ theEnvironment->getName(ENV_PREFIX "0",MAY_RETURN_NULL));
scriptGuess=!(/*userIsASightie ||*/ theEnvironment->getName(ENV_PREFIX "A",MAY_RETURN_NULL));
disableStyle=!(userIsASightie || theEnvironment->getName(ENV_PREFIX "j",MAY_RETURN_NULL));
ignoreBodyTag=!(userIsASightie || theEnvironment->getName(ENV_PREFIX "1",MAY_RETURN_NULL));
ignoreFontTags=!(userIsASightie || theEnvironment->getName(ENV_PREFIX "2",MAY_RETURN_NULL));
processFrames=!(userIsASightie || theEnvironment->getName(ENV_PREFIX "3",MAY_RETURN_NULL));
noApplets=!(userIsASightie || theEnvironment->getName(ENV_PREFIX "4",MAY_RETURN_NULL));
changeBlinkToBold=!(userIsASightie || theEnvironment->getName(ENV_PREFIX "5",MAY_RETURN_NULL));
changeULToBold=!(userIsASightie || theEnvironment->getName(ENV_PREFIX "6",MAY_RETURN_NULL));
changeItalToBold=!(userIsASightie || theEnvironment->getName(ENV_PREFIX "7",MAY_RETURN_NULL));
separateAdjacentLinks=!(userIsASightie || theEnvironment->getName(ENV_PREFIX "8",MAY_RETURN_NULL));
processTables=!(userIsASightie || theEnvironment->getName(ENV_PREFIX "9",MAY_RETURN_NULL));
removeMarquee=!(userIsASightie || theEnvironment->getName(ENV_PREFIX "a",MAY_RETURN_NULL));
processHrefImg=!(userIsASightie || theEnvironment->getName(ENV_PREFIX "c",MAY_RETURN_NULL));
processParams=!(userIsASightie || theEnvironment->getName(ENV_PREFIX "p",MAY_RETURN_NULL));
spaceDetect=!(userIsASightie || theEnvironment->getName(ENV_PREFIX "q",MAY_RETURN_NULL));
removeNobr=!(userIsASightie || theEnvironment->getName(ENV_PREFIX "u",MAY_RETURN_NULL));
zapImageSubmits=!(userIsASightie || theEnvironment->getName(ENV_PREFIX "v",MAY_RETURN_NULL));
removeSpacer=!(userIsASightie || theEnvironment->getName(ENV_PREFIX "F",MAY_RETURN_NULL));
processEmbedTags=!(userIsASightie || theEnvironment->getName(ENV_PREFIX "L",MAY_RETURN_NULL));

// ***NOTE*** the following is negated from the others!
replaceEmbedWithBGSound=((theEnvironment->getName(ENV_PREFIX "d",MAY_RETURN_NULL))!=NULL);
removeDocumentHead=((theEnvironment->getName(ENV_PREFIX "f",MAY_RETURN_NULL))!=NULL);
resetSaysReset=((theEnvironment->getName(ENV_PREFIX "h",MAY_RETURN_NULL))!=NULL);
smileySearch=(!userIsASightie && (theEnvironment->getName(ENV_PREFIX "b",MAY_RETURN_NULL))!=NULL);
dontStripHrefForAlt=(!userIsASightie && (theEnvironment->getName(ENV_PREFIX "J",MAY_RETURN_NULL))!=NULL);
stripTables=(!userIsASightie && (theEnvironment->getName(ENV_PREFIX "g",MAY_RETURN_NULL))!=NULL);
stripFrames=(!userIsASightie && (theEnvironment->getName(ENV_PREFIX "r",MAY_RETURN_NULL))!=NULL);
repeatAltTagsAfterImages=(!userIsASightie && (theEnvironment->getName(ENV_PREFIX "n",MAY_RETURN_NULL))!=NULL);
resetFontAfterLink=((theEnvironment->getName(ENV_PREFIX "i",MAY_RETURN_NULL))!=NULL);
replaceParasWithIndent=((theEnvironment->getName(ENV_PREFIX "o",MAY_RETURN_NULL))!=NULL);
addHardWrap=((theEnvironment->getName(ENV_PREFIX "s",MAY_RETURN_NULL))!=NULL);
removeAltTags=((theEnvironment->getName(ENV_PREFIX "t",MAY_RETURN_NULL))!=NULL);
removeAllImages=(!userIsASightie && (theEnvironment->getName(ENV_PREFIX "z",MAY_RETURN_NULL))!=NULL);

// ENV_PREFIX "D" - don't do Javascript status line:
doLinkStatusLine=(!(theEnvironment->getName(ENV_PREFIX "D",MAY_RETURN_NULL)));

extern char undocEnableIgnore;
undocEnableIgnore=((theEnvironment->getName(ENV_ENABLE_IGNORE,MAY_RETURN_NULL))!=NULL);

processImageMaps=!(userIsASightie || theEnvironment->getName(ENV_PREFIX "e",MAY_RETURN_NULL));
addEndOfPage=!(userIsASightie || theEnvironment->getName(ENV_PREFIX "w",MAY_RETURN_NULL));
doBannerMove=((!(userIsASightie || theEnvironment->getName(ENV_PREFIX "x",MAY_RETURN_NULL))) && !isMainPage(currentPage));
zapHeightsWithPercent=!(userIsASightie || theEnvironment->getName(ENV_PREFIX "y",MAY_RETURN_NULL));
expandAcronyms=!(userIsASightie || theEnvironment->getName(ENV_PREFIX "G",MAY_RETURN_NULL));
showHiddenFormFields=(theEnvironment->getName(ENV_PREFIX "H",MAY_RETURN_NULL)!=NULL);
adobePdfHack=!(userIsASightie || theEnvironment->getName(ENV_PREFIX "I",MAY_RETURN_NULL));
win1252=((theEnvironment->getName(ENV_PREFIX "K",MAY_RETURN_NULL))!=NULL);
doFlash=!(userIsASightie || theEnvironment->getName(ENV_PREFIX "M",MAY_RETURN_NULL));

// Next is "N" (doing upper case)

boldColour=theEnvironment->getName(ENV_BOLD_COLOUR,MAY_RETURN_NULL); if(boldColour && !strcmp(boldColour,UNSPEC_COLOUR)) boldColour=NULL;
headColour=theEnvironment->getName(ENV_HEAD_COLOUR,MAY_RETURN_NULL); if(headColour && !strcmp(headColour,UNSPEC_COLOUR)) headColour=NULL;
normalColour=theEnvironment->getName(ENV_TEXT_COLOR,MAY_RETURN_NULL);

const char* tt=theEnvironment->getName(TABLE_TRIM,MAY_RETURN_NULL);
if(tt) tableTrimmer=new TableTrim(tt);
tt=theEnvironment->getName(LINK_FOLLOW,MAY_RETURN_NULL);
if(tt) {
linkFollower=new LinkFollow(tt);
processFrames=1;
}
if(processTables) stripTables=0;
}

/// Writes a table row separator if necessary
void AccessOptions::separateTableRow(CGIEnvironment* theEnvironment,const char* comment) {
checkPoint();
// Using h_puts not h_fputs, because we do want a
// newline after the <HR> (see comment on Mozilla bug
// below) - 2000-08-21
if(banner->hadMeaningfulThisRow) {
if(banner->hadMeaningfulThisRow>horizontalLineThreshold) {
theEnvironment->h_fputs("<HR TITLE=\"");
theEnvironment->h_fputs(comment);
theEnvironment->h_puts("\">");
} else theEnvironment->h_puts("<BR>");
banner->hadMeaningfulThisRow=banner->hadMeaningfulThisCell=0;
if(banner->meaningfulCtr2<bannerMinLength) banner->meaningfulCtr2+=charCountTableRowPenalty;
} else {
theEnvironment->h_puts("" /*"<!-- meaningless row/frame? -->"*/);
}
}

/**
Processes a tag.
@param theTag The tag to process
@return 1 if further processing is to be aborted (usually a serious error)
*/
int AccessOptions::processTag(HTMLTag &theTag,InString &htmlLineInput) {
checkPoint();
if(disableScript) {
if(scriptGuess) {
// Pick out any URLs from the "on..." tags (because some pages have nothing but "onload='location=...'")
for(int i=0; i<theTag.getNumAttributes(); i++) {
if(!strnicmp(theTag.getAttributeName(i),"ON",2)) doScriptyStuff(theTag.getAttributeValue(i),htmlLineInput);
}
}
theTag.removeJavascript();
}
if(disableStyle) {
theTag.deleteAllByName("STYLE");
theTag.deleteAllByName("CLASS");
}
if(processFrames) theTag.deleteAllByName("TARGET");
if(removeAltTags) theTag.deleteAllByName("ALT");

if(!stricmp(theTag.getName(),"SELECT")) inSelect=1;
else if(!stricmp(theTag.getName(),"/SELECT")) inSelect=0;

if(!stricmp(theTag.getName(),"INPUT") ||
!stricmp(theTag.getName(),"SELECT") ||
!stricmp(theTag.getName(),"TEXTAREA")) {
// Let's just check nobody's using my user ID
// (Now updated to 'escape' it, so we can use a shorter
// ID, but need to watch out for the old one also)
// The un-escaping is done under CGILib's
// lynxDumpExcludePrefix
const char* x=theTag.getAttribute("NAME"); if(x && !(strnicmp(x,SSB22_PREFIX,strlen(SSB22_PREFIX)) && strnicmp(x,OLD_PREFIX,strlen(OLD_PREFIX)))) {
// Oh dear, we'll have to 'double-escape' it
char* y=new char[strlen(x)+(SSB22_ESCAPE_LEN+1)];
strcpy(y,SSB22_ESCAPE);	strcat(y,x);
theTag.deleteAllByName("NAME");
theTag.addAttribute("NAME",y);
delete[] y;
// throw "Someone else is using '" SSB22_PREFIX "' in their forms";
}
}

if(!stricmp(theTag.getName(),"BODY")) {
if(!frameNesting) banner->meaningfulCharacterCounter=0;
if(inHead) processTag("/HEAD",htmlLineInput);
// 2000-08-29: The above was added because some
// documents don't have a </HEAD> (and we might be
// expecting to do some processing)
}
// 2000-10-06: The initTheFont at the beginning of TT and PRE was causing Netscape 4.7ish to keep the TT active throughout the entire page, so at this point we output </FONT> before the close of these tags
if(!stricmp(theTag.getName(),"/TT") || !stricmp(theTag.getName(),"/PRE")) closeTheFontTag(theEnvironment);
if(!stricmp(theTag.getName(),"/PRE")) inPre=0;
if(
!(
!stricmp(theTag.getName(),"SCRIPT") &&
(doScriptSrcStuff(theTag,disableScript,in,htmlLineInput,scriptGuess,currentPage,theEnvironment) ||
doScriptyStuff(theTag,"SCRIPT",disableScript,in,htmlLineInput,scriptGuess))
)
&& !(
!stricmp(theTag.getName(),"STYLE") &&
doScriptyStuff(theTag,"STYLE",disableStyle,in,htmlLineInput,0)
))
// If true, then didn't deal with it, so continue
if(disableStyle && !(stricmp(theTag.getName(),"LINK") && stricmp(theTag.getName(),"DIV") && stricmp(theTag.getName(),"/DIV") && stricmp(theTag.getName(),"SPAN") && stricmp(theTag.getName(),"/SPAN"))) return 0; // Ignore
else if(!stricmp(theTag.getName(),"HEAD")) {
if(removeDocumentHead || frameNesting) theEnvironment->outputToNothing(); else theEnvironment->outputToHeader();
// (Hack: Don't add to the header if frameNesting - it
// will interfere with the banner moving code, which
// may have already recorded the position of the end
// of the banner (this position will be invalidated by
// adding to the document header))
// (NO - theEnvironment handles data and header in
// separate strings.  Probably reduces clutter though)
inHead=1;
} else if(!stricmp(theTag.getName(),"/HEAD")) {
theEnvironment->outputToMainPage();
inHead=0;
} else if(ignoreBody && !stricmp(theTag.getName(),"BODY")) theEnvironment->outputToNothing();
else if(!stricmp(theTag.getName(),"/BODY")) theEnvironment->outputToMainPage(); // Don't check ignoreBody, because we don't want to repeat the /BODY anyway (we'll add our own later).  Do do the outputToMainPage(), because there might be more frames to follow.
else if(ignoreBodyTag && !stricmp(theTag.getName(),"BODY")) {
/*if(ignoreBodyTag)*/ return 0; // Ignore
/*else ..;*/
// NEEDATTENTION this could be better - instead of re-writing the BODY tag, could copy its attributes into the BODY tag that we're going to write (overriding what's already there)
// (This has effect if you choose colours and also ask for the document's colours)
}
else if(processFrames && !stricmp(theTag.getName(),"NOFRAMES")) theEnvironment->outputToNothing();
else if(processFrames && !stricmp(theTag.getName(),"/NOFRAMES")) theEnvironment->outputToMainPage(); // Might have NOFRAMES before the frameset
else if(!strnicmp(theTag.getName(),"HTM",3) || !strnicmp(theTag.getName(),"/HTM",4)) return 0; // Ignore: HTML, /HTML would be already done (HTM added because some webmasters have unusual ideas)
else if(theTag.getName()[0]=='!') return 0; // Ignore - it's !DOCTYPE or something, which we don't want at this point
else if(ignoreFontTags && (!strnicmp(theTag.getName(),"FONT",4) || !strnicmp(theTag.getName(),"/FONT",5) || !stricmp(theTag.getName(),"SMALL") || !stricmp(theTag.getName(),"/SMALL") || !stricmp(theTag.getName(),"BASEFONT"))) return 0; // Ignore again
else if(processFrames && (!stricmp(theTag.getName(),"FRAMESET") || !stricmp(theTag.getName(),"/FRAMESET"))) {
// Ignore, but -
if(inHead) processTag("/HEAD",htmlLineInput);
// 2000-10-02: The above was added because some
// documents don't have a </HEAD> (and we might be
// expecting to do some processing)
} else if(processImageMaps && !stricmp(theTag.getName(),"MAP")) hadAreaInMapYet=0; // but otherwise ignore
else if(!stricmp(theTag.getName(),"MAP")) theTag.outputWithModifiedURLs(currentPage,theEnvironment);
else if(processImageMaps && !stricmp(theTag.getName(),"/MAP")) {
if(!hadAreaInMapYet) theEnvironment->h_puts(L_IMG_MAP_WITHOUT_AREA); // otherwise ignore
} else if(processImageMaps && !stricmp(theTag.getName(),"AREA")) {
const char* href=theTag.getAttribute("HREF"); if(href && *href) {
theEnvironment->outputBodyForNow();
hadAreaInMapYet=1;
HTMLTag tag2("A");
tag2.addAttribute("HREF",href);
processTag(tag2,htmlLineInput);
const char* textToOutput=theTag.getAttribute("ALT");
if(textToOutput && isMeaningful(textToOutput))
theEnvironment->h_puts(textToOutput);
else if(dontStripHrefForAlt)
theEnvironment->h_fputs(href);
else {
char* x=theTag.getStrippedHrefForAlt();
theEnvironment->h_fputs(x); delete[] x;
}
if(needLinkImgNoAlt) { delete[] urlForLinkImgNoAlt; needLinkImgNoAlt=0; }
// NEEDATTENTION Perhaps the above few lines can be deleted?
// (careful, they may need to set another option to enable the urlForLinkImgNoAlt thing)
processTag("/A",htmlLineInput);
theEnvironment->restoreOutputSetting();
}
} else if(!strnicmp(theTag.getName(),"AREA",4)) theTag.outputWithModifiedURLs(currentPage,theEnvironment);
else if(processFrames && (!stricmp(theTag.getName(),"/LAYER") || !stricmp(theTag.getName(),"/ILAYER"))) ; // Ignore
else if(processFrames && (!stricmp(theTag.getName(),"FRAME") || !stricmp(theTag.getName(),"IFRAME") || !stricmp(theTag.getName(),"LAYER") || !stricmp(theTag.getName(),"ILAYER"))) {
const char* x=theTag.getAttribute("SRC");
if(x && *x) { // 2001-06-04: Need *x because some
// sites set SRC to the empty string
separateTableRow(theEnvironment,L_FRAME_SEP_START);
//theEnvironment->h_fputs("<HR TITLE=\"");
//theEnvironment->h_fputs(L_FRAME_SEP_START);
//theEnvironment->h_puts("\">"); // Don't change that to h_fputs (see comment below on Mozilla bug)
char* y=sortDirectories(x,currentPage);
InString actualPage;
#ifdef Internal_Extensions1
ExtensionContainer* c=makeNewReturnOldExtensions();
#endif
FILE* frameFile=NULL;
try {
if(frameNesting==MAX_FRAME_NESTING) {
// don't bother
theEnvironment->h_fputs(L_UNGETTABLE_FRAME_1); theEnvironment->h_puts(y);
theEnvironment->h_fputs("<BR>"); theEnvironment->h_fputs(L_UNGETTABLE_FRAME_2); theEnvironment->h_fputs("Too many recursive frames");
} else frameFile=getWebPage(y,NULL,MAY_NOT_RETURN_NULL,actualPage,theEnvironment,0);
} catch(CatchLiteral x) {
theEnvironment->h_fputs(L_UNGETTABLE_FRAME_1); theEnvironment->h_puts(y);
theEnvironment->h_fputs("<BR>"); theEnvironment->h_fputs(L_UNGETTABLE_FRAME_2); theEnvironment->h_fputs(x);
delete[] y; // don't need fclose(frameFile) or frameFile=NULL
} catch(...) {
delete[] y; throw; // don't need fclose(frameFile)
}
if(frameFile) {
try {
banner->bannerEndLocked = 0; // hack for sites
// that have (title frame, links frame, main
// frame).  Really need proper frame
// re-ordering.
htmlFilter(theEnvironment,frameFile,actualPage.getString(),0,frameNesting+1,banner);
} catch(...) {
fclose(frameFile); delete[] y; throw;
}
fclose(frameFile); delete[] y;
}
#ifdef Internal_Extensions1
resetOldExtensions(c);
#endif
separateTableRow(theEnvironment,L_FRAME_SEP_END);
//theEnvironment->h_fputs("<HR TITLE=\"");
//theEnvironment->h_fputs(L_FRAME_SEP_END);
//theEnvironment->h_puts("\">"); // Don't change that to h_fputs (see comment below on Mozilla bug)
if(toupper(theTag.getName()[0])!='I') ignoreBody=1;
// 2001-06-27: Condition added to fix cases where
// there's an IFRAME followed by a surplus BODY tag
}
} else if(!stricmp(theTag.getName(),"FRAME") || !stricmp(theTag.getName(),"IFRAME") || !stricmp(theTag.getName(),"ILAYER") || !stricmp(theTag.getName(),"LAYER")) {
if(!stricmp(theTag.getName(),"FRAME")) theEnvironment->deleteTheSpeilIfNotAlreadyDone(); // Not with IFRAME, because that seems to be "inline frame" (and I'm guessing that ILAYER is similar)
if(stripFrames) {
const char* x=theTag.getAttribute("SRC");
if(x && *x) {
HTMLTag tag2(/*theTag.getName()*/ "FRAME");
// NEEDATTENTION Should that really be "FRAME" even with "LAYER" etc?  Also what of removing the / tags if so?
tag2.addAttribute("SRC",x);
tag2.outputWithModifiedURLs(currentPage,theEnvironment);
} else theTag.outputWithModifiedURLs(currentPage,theEnvironment);  // Compiler, please optimise that!
} else theTag.outputWithModifiedURLs(currentPage,theEnvironment);
} else if(noApplets && (!stricmp(theTag.getName(),"APPLET") || !stricmp(theTag.getName(),"/APPLET") || !stricmp(theTag.getName(),"OBJECT") || !stricmp(theTag.getName(),"/OBJECT"))) return 0; // Ignore
else if(noApplets && processParams && !stricmp(theTag.getName(),"PARAM")) {
const char* v=theTag.getAttribute("VALUE"); if(v) {
if((!strnicmp(v,"http://",7) || (strchr(v,'.') && !strcmp(v+strlen(v)-2,"tm") || !strcmp(v+strlen(v)-3,"tml")))
&& !strchr(v,' ') && !strchr(v,'<')) {
// It's probably a link
// (Don't really need to bother with other protocols; this is only an applet parameter guesser)
HTMLTag tag2("A");
tag2.addAttribute("HREF",v);
processTag(tag2,htmlLineInput);
theEnvironment->h_fputs(v);
if(needLinkImgNoAlt) { delete[] urlForLinkImgNoAlt; needLinkImgNoAlt=0; }
processTag("/A",htmlLineInput);
theEnvironment->h_puts("<BR>");
} else if(strchr(v,' ')) {
// It's probably text
// 2000-07-20: No!  Should check the "words"
// are not rediculously long
// Count the spaces and compare with len
int spaces=0;
const char* spac=strchr(v,' '); while(spac) {
spaces++; spac=strchr(spac+1,' ');
}
if(strlen(v)<=(size_t)(spaces*BigWordLen)) {
// Higher probability that it's text
// (NB This all assumes a writing system with
// spaces in it)
theEnvironment->h_fputs(v);
theEnvironment->h_puts("<BR>");
}
}
}
} else if(!stricmp(theTag.getName(),"APPLET")) {
// Make sure it has a CODEBASE
if(!theTag.getAttribute("CODEBASE")) {
char* y=sortDirectories(".",currentPage);
theTag.addAttribute("CODEBASE",y);
delete[] y;
}
theTag.outputWithModifiedURLs(currentPage,theEnvironment);
} else if(!stricmp(theTag.getName(),"META")
#ifdef DEVELOPMENT_COPY
&& !dontProcessLinks
#endif
) return 0; // Ignore
// (We just ditch META tags, including
// HTTP-EQUIV / Refresh ones, because these
// have already been dealt with)
// 2003-02-21: AND it's better for the character set thing (see ucam.comp-serv.suggest post)
else if(!stricmp(theTag.getName(),"B")
|| !stricmp(theTag.getName(),"STRONG")) {
if(boldColour) {
if(!inHref) theEnvironment->h_fputs("</FONT>");
// Not within "A", because the /FONT will revert to
// a small size (alternatively could use another
// initTheFont with link colour, but then visited
// links are a problem).  There are various bugs in
// some versions of the Konqueror browser that cause
// spurious newlines when FONT is used improperly,
// so careful.
initTheFont(theEnvironment,1,boldColour);
}
else theTag.outputTo(theEnvironment,currentPage);
} else if(!stricmp(theTag.getName(),"/B")
|| !stricmp(theTag.getName(),"/STRONG")) {
if(boldColour) {
/*if(inHref)*/ theEnvironment->h_fputs("</FONT>");
if(!inHref) initTheFont(theEnvironment,1,(inHeading && headColour)?headColour:normalColour);
// 2001-06-05: otherwise the </FONT> might be moved
// by the banner move and you lose the large font.
// Really need a better way of doing this.
}
else theTag.outputTo(theEnvironment,currentPage);
} else if((changeBlinkToBold && !stricmp(theTag.getName(),"BLINK"))
|| (changeULToBold && !stricmp(theTag.getName(),"U"))
|| (changeItalToBold && (!stricmp(theTag.getName(),"I") || !stricmp(theTag.getName(),"EM") || !stricmp(theTag.getName(),"ADDRESS") || !stricmp(theTag.getName(),"CITE") || !stricmp(theTag.getName(),"DFN")))
) processTag("b",htmlLineInput);
else if((changeBlinkToBold && !stricmp(theTag.getName(),"/BLINK"))
|| (changeULToBold && !stricmp(theTag.getName(),"/U"))
|| (changeItalToBold && (!stricmp(theTag.getName(),"/I") || !stricmp(theTag.getName(),"/EM") || !stricmp(theTag.getName(),"/ADDRESS") || !stricmp(theTag.getName(),"/CITE") || !stricmp(theTag.getName(),"/DFN")))
) processTag("/b",htmlLineInput);
else if(!stricmp(theTag.getName(),"EMBED")) {
const char* c=theTag.getAttribute("TYPE");
if(replaceEmbedWithBGSound && (!c || !strnicmp(c,"audio/",6))) {
// Audio (and we're converting to BGSOUND)
theTag.setNameTo("BGSOUND");
theTag.addAttribute("VOLUME","100");
theTag.outputTo(theEnvironment,currentPage);
} else {
// Not audio, or audio but not converting.
if(processEmbedTags) {
c=theTag.getAttribute("SRC");
if(c && *c) {
// There is a SRC.  Is it Flash?
if(!(doFlash && strlen(c)>4 && (!strcmp(c+strlen(c)-4,".swf") || strstr(c,".swf?")) && doSwfStuff(c,htmlLineInput,currentPage,theEnvironment))) {
// Not Flash
theEnvironment->h_fputs("<P>");
theEnvironment->h_fputs(L_EMBEDDED_FILE);
theEnvironment->h_fputs(": ");
HTMLTag tag2("A");
tag2.addAttribute("HREF",c);
processTag(tag2,htmlLineInput);
processTag("/A",htmlLineInput);
theEnvironment->h_fputs("<P>");
}
} // else no SRC (not much that we can do)
} else {
// We do need to modify the URLs, in case there
// are pointers to plugin pages etc.
theTag.outputWithModifiedURLs(currentPage,theEnvironment);
}
}
} else if(!stricmp(theTag.getName(),"FORM")) {
if(inHref) processTag("/A",htmlLineInput);
if(!theTag.getAttribute("ACTION")) {
// No ACTION tag, so default to the URL that we got the page from
// (2001-02-01)
// (However, we won't *output* implicit ACTION tags, because
// some browsers might not support them, and the bandwidth saved
// is usually minimal)
char* y=strnew(currentPage);
if(strchr(y,'?')) *(strchr(y,'?'))=0;
if(strchr(y,'#')) *(strchr(y,'#'))=0;
theTag.addAttribute("ACTION",y);
delete[] y;
}
checkSecurity(theTag.getAttribute("ACTION"),theEnvironment,L_SECURITY_FORM,currentPage);
theTag.outputWithModifiedURLs(currentPage,theEnvironment);
#ifdef ENCODINGS_AVAILABLE
static char needFormWarning=1; // Static because we only want one per run
if(UsingImages && needFormWarning) {
theEnvironment->h_fputs("<BR>");
theEnvironment->h_fputs(L_IMAGE_FORM_WARNING);
theEnvironment->h_puts("<BR>");
needFormWarning=0;
}
#endif
const char* x=theTag.getAttribute("ENCTYPE");
if(x && !stricmp(x,"multipart/form-data")) {
theEnvironment->h_fputs("<BR>");
theEnvironment->h_fputs(L_ENCTYPE_WARNING);
theEnvironment->h_puts("<BR>");
}
x=theTag.getAttribute("METHOD");
if(x && !stricmp(x,"POST")) theEnvironment->h_puts("<INPUT TYPE=hidden NAME=" FORM_NEEDS_POSTING " VALUE=1>");
} else if(!stricmp(theTag.getName(),"A")) { // Hyperlink
if(inHref) processTag("/A",htmlLineInput);
const char* href=theTag.getAttribute("HREF");
lastLinkWasHref=0; if(href && *href) {
if(separateAdjacentLinks && !alreadyHadLinkBrackets) { theEnvironment->h_fputs(" ["); lastLinkWasHref=1; }
if(!banner->bannerEndLocked && (banner->meaningfulCharacterCounter>bannerDetectionThreshold || *href=='#') && theEnvironment->outputtingToMainPage()) {
// outputtingToMainPage condition added so that
// headings in "noframes" sections have no effect
// when they're not going to appear
banner->bannerEndLocked=1;
banner->lockedBannerEnd = banner->bannerEnd;
// theEnvironment->h_printf("<!-- Debugger: Locking banner end at %d; banner->meaningfulCharacterCounter=%d (bannerDetectionThreshold=%d), banner->meaningfulCtr2=%d (bannerMinLength=%d bannerMaxLength=%d bannerDocMinLength=%d) -->\n",banner->lockedBannerEnd,banner->meaningfulCharacterCounter,bannerDetectionThreshold,banner->meaningfulCtr2,bannerMinLength,bannerMaxLength,bannerDocMinLength);
}
// 2000-10-13: The condition on *href=='#' was added so that pages with contents links do not get the contents moved to the bottom (since these ARE relevant to the page)
if(/*disableScript &&*/ !strnicmp(href,"Javascript:",11)) {
if(scriptGuess && javascriptToHref(theTag)) theEnvironment->h_fputs(L_JAVASCRIPT_LINK_GUESS);
else 
#ifdef DEVELOPMENT_COPY
if(!dontProcessLinks)
#endif
theEnvironment->h_fputs(L_JAVASCRIPT_WARNING);
}
checkSecurity(href,theEnvironment,L_SECURITY_LINK,currentPage);
if(processHrefImg) {
needLinkImgNoAlt=1; // 1 = had no text yet, 2 = had images and no text, 0 = had text
urlForLinkImgNoAlt=
(dontStripHrefForAlt ?
strnew(href) :
theTag.getStrippedHrefForAlt());
}
theTag.outputWithModifiedURLs(currentPage,theEnvironment,doLinkStatusLine,
adobePdfHack && strlen(href)>4 && !stricmp(href+strlen(href)-4,".pdf"));
#ifdef ENCODINGS_AVAILABLE
if(!separateAdjacentLinks && LinksNeedBrackettingForImages) theEnvironment->h_putchar('[');
#endif
inHref=1;
if(linkFollower) linkFollower->setStartOfLink(theEnvironment,theTag.getAttribute("HREF"));
// (Because theTag.getAttribute("HREF") will now be a complete URL)
} else if(theTag.getAttribute("NAME")) {
theTag.outputTo(theEnvironment); // NAME tag: Might as well output the lot
} // else ignore (accepts NAME and HREF only)
} else if(!stricmp(theTag.getName(),"/A")) {
// End of hyperlink
// if(needLinkImgNoAlt==2) theEnvironment->h_fputs(urlForLinkImgNoAlt);
// if(needLinkImgNoAlt) { delete[] urlForLinkImgNoAlt; needLinkImgNoAlt=0; }
// The following means it also puts the URL if there's a link with no image AND no text
// (eg. www.freebsd.cz/publish.html)
if(inHref) {
if(needLinkImgNoAlt) {
theEnvironment->h_fputs(urlForLinkImgNoAlt);
// 2000-12-16: Need to add to banner->meaningfulCharacterCounter here also:
int toAdd=strlen(urlForLinkImgNoAlt);
banner->meaningfulCharacterCounter+=toAdd; banner->hadMeaningfulThisRow+=toAdd; banner->hadMeaningfulThisCell+=toAdd;
// (+ there may be others that are missed; more elegant code would be nice...)
delete[] urlForLinkImgNoAlt; needLinkImgNoAlt=0;
}
// NEEDATTENTION May need more than three </FONT>s, depending on the NUMBER of style changes in the link
#ifdef ENCODINGS_AVAILABLE
if(!separateAdjacentLinks && LinksNeedBrackettingForImages) theEnvironment->h_putchar(']');
#endif
if(resetFontAfterLink) theEnvironment->h_fputs("</FONT></FONT></FONT>");
}
theEnvironment->h_fputs("</A>");
if(inHref) {
inHref=0;
if(resetFontAfterLink) initTheFont(theEnvironment,1);
if(separateAdjacentLinks && lastLinkWasHref) theEnvironment->h_fputs("] ");
if(linkFollower) linkFollower->setEndOfLink(theEnvironment,this);
banner->bannerEnd=theEnvironment->getPos();
banner->meaningfulCtr2+=banner->meaningfulCharacterCounter; banner->meaningfulCharacterCounter=0;
}
} else if(processTables &&
(
!stricmp(theTag.getName(),"TABLE") ||
!stricmp(theTag.getName(),"/TABLE") ||
!stricmp(theTag.getName(),"/TR") ||
!stricmp(theTag.getName(),"THEAD") ||
!stricmp(theTag.getName(),"/THEAD") ||
!stricmp(theTag.getName(),"TBODY") ||
!stricmp(theTag.getName(),"/TBODY"))) {
if(inHref) processTag("/A",htmlLineInput);
if(!stricmp(theTag.getName(),"/TABLE")) separateTableRow(theEnvironment,L_TAB_ROW_SEP);
else if(tableTrimmer && !stricmp(theTag.getName(),"/TR")) tableTrimmer->setCloseTR(theEnvironment);
} else if(processTables && (!stricmp(theTag.getName(),"/TD") || !stricmp(theTag.getName(),"/TH"))) {
if(inHref) processTag("/A",htmlLineInput);
if(inPre) processTag("/PRE",htmlLineInput);
} else if(!stricmp(theTag.getName(),"TR")) {
if(tableTrimmer) tableTrimmer->setOpenTR(theEnvironment);
if(processTables) {
if(inHref) processTag("/A",htmlLineInput);
separateTableRow(theEnvironment,L_TAB_ROW_SEP);
} else if(stripTables) {
if(inHref) processTag("/A",htmlLineInput);
theEnvironment->h_fputs("<TR>");
} else theTag.outputTo(theEnvironment,currentPage);
} else if(processTables && (!stricmp(theTag.getName(),"TD") || !stricmp(theTag.getName(),"TH"))) {
if(inHref) processTag("/A",htmlLineInput);
if(banner->hadMeaningfulThisCell) theEnvironment->h_fputs(" : ");
banner->hadMeaningfulThisCell=0;
} else if(stripTables && !stricmp(theTag.getName(),"TABLE")) {
if(inHref) processTag("/A",htmlLineInput);
theEnvironment->h_fputs("<TABLE BORDER WIDTH=\"100%\">");
} else if(stripTables && !stricmp(theTag.getName(),"/TABLE")) {
if(inHref) processTag("/A",htmlLineInput);
theEnvironment->h_fputs("</TABLE>");
} else if(stripTables && (!stricmp(theTag.getName(),"TD") || !stricmp(theTag.getName(),"TH"))) {
if(inHref) processTag("/A",htmlLineInput);
(*theEnvironment) << "<TD VALIGN=\"top\"";
const char* x=theTag.getAttribute("COLSPAN");
if(x) (*theEnvironment) << " COLSPAN=\"" << x << "\"";
x=theTag.getAttribute("ROWSPAN"); if(x) (*theEnvironment) << " ROWSPAN=\"" << x << "\"";
theEnvironment->h_putchar('>');
} else if(!stricmp(theTag.getName(),"/TR")) {
if(stripTables && inHref) processTag("/A",htmlLineInput);
theEnvironment->h_fputs("</TR>");
if(tableTrimmer) tableTrimmer->setCloseTR(theEnvironment);
} else if(stripTables && !stricmp(theTag.getName(),"/TD")) {
if(inHref) processTag("/A",htmlLineInput);
theEnvironment->h_fputs("</TD>");
} else if(stripTables && !stricmp(theTag.getName(),"/TH")) {
if(inHref) processTag("/A",htmlLineInput);
theEnvironment->h_fputs("</TH>");
} else if(removeMarquee && (!stricmp(theTag.getName(),"MARQUEE") || !stricmp(theTag.getName(),"/MARQUEE"))) return 0; // Ignore
else if(!stricmp(theTag.getName(),"ISINDEX")) outputIsIndex(theEnvironment);

else if(!stricmp(theTag.getName(),"INPUT")) {
const char* x=theTag.getAttribute("TYPE");
if(zapImageSubmits && x && !stricmp(x,"image")) {
theTag.deleteAllByName("TYPE");
theTag.addAttribute("TYPE","SUBMIT");
if(!theTag.getAttribute("VALUE")) {
// No VALUE, but it might have an ALT tag
const char* y=theTag.getAttribute("ALT"); if(y) {
theTag.addAttribute("VALUE",y);
theTag.deleteAllByName("ALT");
// 13 December 1999: It seems that Netscape uses the NAME
// tag as the ALT tag, and some websites have NAME instead
// of ALT.  So....
} else if((y=theTag.getAttribute("NAME"))!=0) {
theTag.addAttribute("VALUE",y);
}
}
} else if(showHiddenFormFields && x && !stricmp(x,"hidden")) {
theTag.deleteAllByName("TYPE");
theTag.addAttribute("TYPE","TEXT");
const char* y=theTag.getAttribute("NAME");
theEnvironment->h_putchar('\n');
if(y) {
theEnvironment->h_fputs(y);
theEnvironment->h_putchar(':');
} // (else the page is wrong, but not to worry here)
y=theTag.getAttribute("VALUE"); if(y && strlen(y)<15) {
// Make the box smaller (in case there are many of them)
char buf[10]; sprintf(buf,"%d",strlen(y)+1);
theTag.addAttribute("SIZE",buf);
}
}
if(resetSaysReset && x && !stricmp(x,"reset")) theEnvironment->h_fputs("<INPUT TYPE=reset VALUE=Reset USESTYLE>");
else theTag.outputTo(theEnvironment,currentPage);
} else if(processHrefImg && !stricmp(theTag.getName(),"IMG") && inHref) {
const char* x=theTag.getAttribute("ALT"); if(x && isMeaningful(x)) {
theEnvironment->h_fputs(x);
if(needLinkImgNoAlt) { delete[] urlForLinkImgNoAlt; needLinkImgNoAlt=0; }
} else if(needLinkImgNoAlt) needLinkImgNoAlt=2;
} else if(!stricmp(theTag.getName(),"IMG")) {
const char* alt=theTag.getAttribute("ALT");
int altStatus=((alt && isMeaningful(alt))?1:0); // 0 = can't, 1 = haven't had yet, 2 = had
if(processImageMaps && theTag.getAttribute("ISMAP")) {
if(altStatus) {
theEnvironment->h_puts(theTag.getAttribute("ALT"));
altStatus=2;
if(needLinkImgNoAlt) { delete[] urlForLinkImgNoAlt; needLinkImgNoAlt=0; }
} else if(needLinkImgNoAlt) needLinkImgNoAlt=2;
if(!(theTag.getAttribute("USEMAP"))) {
if(inHref) theEnvironment->h_puts(L_SERVER_SIDE_MAP);
if(needLinkImgNoAlt) { delete[] urlForLinkImgNoAlt; needLinkImgNoAlt=0; }
if(!alt && !removeAltTags) theTag.addAttribute("ALT",""); // Little else we can do
if(!removeAllImages) theTag.outputTo(theEnvironment,currentPage);
}
} else {
if(needLinkImgNoAlt) { delete[] urlForLinkImgNoAlt; needLinkImgNoAlt=0; }
if(!alt && !removeAltTags) theTag.addAttribute("ALT",""); // Little else we can do
if(zapHeightsWithPercent) {
const char* height=theTag.getAttribute("HEIGHT");
if(height && strrchr(height,'%')) theTag.deleteAllByName("HEIGHT");
}
if(!removeAllImages) theTag.outputTo(theEnvironment,currentPage);
}
if(repeatAltTagsAfterImages && altStatus==1) {
theEnvironment->h_puts(alt);
if(needLinkImgNoAlt) { delete[] urlForLinkImgNoAlt; needLinkImgNoAlt=0; }
}
} else if(!stricmp(theTag.getName(),"TITLE")) {
theTag.outputTo(theEnvironment);
theEnvironment->dontAddServiceTitle();
canWriteInHead=1;
} else if(!stricmp(theTag.getName(),"/TITLE")) {
theTag.outputTo(theEnvironment);
canWriteInHead=0;
} else if(toupper(theTag.getName()[0])=='H' && isdigit(theTag.getName()[1])) {
if(!banner->hadHeadingYet) {
// If this is the first heading on the page, then
// it's probably a good place to end the "banner",
// if the link density heuristic had already ended
// it (2001-02-24)
if(banner->bannerEndLocked  && theEnvironment->outputtingToMainPage()) {
// outputtingToMainPage condition added so that
// headings in "noframes" sections have no effect
// when they're not going to appear
banner->lockedBannerEnd=theEnvironment->getPos();
// theEnvironment->h_printf("<!-- Debugger: Updating locked banner end; tag=%s -->\n",theTag.getName()); // %s OK (debugging)
}
banner->hadHeadingYet=1;
// NEEDATTENTION Do we also want to END the banner
// here (even if the link density heuristic hasn't
// done it)?  (Some academic people have a heading
// followed by text with a link quite early on in
// the running text, e.g. "I'm a researcher at [XYZ
// university]") (but some pages DO have banners
// after headings)
}
int headingLevel=theTag.getName()[1]-'0',numberOfDashes=0;
if(headingLevel>headingPromotionLevel) {
numberOfDashes=headingLevel-headingPromotionLevel;
headingLevel=headingPromotionLevel;
}
theEnvironment->h_printf("<H%c>",headingLevel+'0');
if(headColour) theEnvironment->h_printf("<FONT COLOR=\"#%.9s\">",headColour); // DON'T use initTheFont within a heading
for(int i=0; i<numberOfDashes; i++) theEnvironment->h_fputs("- ");
inHeading = 1;
} else if(theTag.getName()[0]=='/' && toupper(theTag.getName()[1])=='H' && isdigit(theTag.getName()[2])) {
inHeading = 0;
if(headColour) theEnvironment->h_fputs("</FONT>");
if(theTag.getName()[2]-'0'>headingPromotionLevel)
theEnvironment->h_printf("</H%c>",headingPromotionLevel+'0');
else theTag.outputTo(theEnvironment,currentPage);
// Check if need to re-instate another heading level
const char* x=theEnvironment->getName(ENV_FONTSIZE,MAY_RETURN_NULL);
if(x && atoi(x)>MAX_FONT_SIZE) initTheFont(theEnvironment,2); // 2 - see hack
} else if(!stricmp(theTag.getName(),"BASE")) {
// If HREF, need to change currentPage
// (since it's the base of links and images)
const char* href=theTag.getAttribute("HREF"); if(href && *href
#ifdef DEVELOPMENT_COPY
&& !dontProcessLinks
#endif
) {
if(needToDeleteCurpage) delete[] currentPage;
currentPage=sortDirectories(href,currentPage); needToDeleteCurpage=1;
} else theTag.outputWithModifiedURLs(currentPage,theEnvironment);
} else if((replaceParasWithIndent || (processTables && !banner->hadMeaningfulThisCell)) && !stricmp(theTag.getName(),"P")) {
if(replaceParasWithIndent) theEnvironment->h_puts("<BR>&nbsp;&nbsp;&nbsp;"); // else ignore (<P> within a meaningless table row)
} else if((replaceParasWithIndent || processTables) && !stricmp(theTag.getName(),"/P")) return 0; // Ignore (ignoring with processTables as well because some table generators abuse /P)
else if(processTables && !stricmp(theTag.getName(),"BR")) {
if(!banner->hadMeaningfulThisCell) return 0; // Ignore
banner->hadMeaningfulThisCell = 0; // attempt to remove multiple BR's
theTag.outputTo(theEnvironment,currentPage);
} else if(addHardWrap && !stricmp(theTag.getName(),"TEXTAREA")) {
theTag.deleteAllByName("WRAP");
theTag.addAttribute("WRAP","HARD");
theTag.outputTo(theEnvironment,currentPage);
} else if(removeNobr && !(stricmp(theTag.getName(),"NOBR") && stricmp(theTag.getName(),"/NOBR"))) return 0; // Ignore
else if(disableScript && !(stricmp(theTag.getName(),"NOSCRIPT") && stricmp(theTag.getName(),"/NOSCRIPT"))) return 0; // Ignore
else if(!stricmp(theTag.getName(),"HR")) {
// 2000-08-21
// This is a patch for some alpha versions of Mozilla
// under Linux.  They ignore the user's font size for
// a while if you don't put a newline after an <HR>,
// so we'll make sure that one goes in.
// It's not really worth making this a switchable
// option (for 1-2 bytes per HR)

// 2002-04-18: Added code to reset "hadMeaningful"
// stuff also (if necessary)

// NEEDATTENTION Is "processTables" the right
// condition?  Really need another option here (but if
// processTables then need to reset the meaningful
// counters anyway even if not re-writing the HR)
if(processTables) separateTableRow(theEnvironment,"");
else {
theTag.outputTo(theEnvironment,currentPage);
theEnvironment->h_putchar('\n');
}
} else if(removeSpacer && !stricmp(theTag.getName(),"SPACER")) return 0; // Ignore
else if(expandAcronyms && !stricmp(theTag.getName(),"ACRONYM")) {
const char* c=theTag.getAttribute("TITLE");
if(c) theEnvironment->h_puts(c);
// else what?  (bad HTML)  Not to worry, you'd just
// get an acronym in parentheses
theEnvironment->h_fputs(" (");
} else if(expandAcronyms && !stricmp(theTag.getName(),"/ACRONYM")) theEnvironment->h_putchar(')');
//#ifdef DEVELOPMENT_COPY
#define wapToHtml 1  // temp for debugging
//#endif
// The following WAP to HTML code is horrible - needs
// sorting out
else if(wapToHtml && !stricmp(theTag.getName(),"CARD")) {
processTag("FORM",htmlLineInput);
const char* id=theTag.getAttribute("id");
if(id) {
theEnvironment->h_fputs("<A NAME=\"");
theEnvironment->h_fputs(id);
theEnvironment->h_fputs("\"></A>");
}
const char* onTimer=theTag.getAttribute("ontimer");
// NEEDATTENTION "ontimer" has a special exception from the "on..." scripty removal in tagstuff.c++; it shouldn't really be there.  (The HtmSpotter itself won't recognise a raw URL, only a quoted one in a script.)
if(onTimer && *onTimer!='#') {
// ('#' is an exception because it's going to be another card within the same page and it's probably already on the screen, so no point outputting more clutter about it)
theEnvironment->h_fputs("<BR>");
theEnvironment->h_fputs(L_PAGE_REFRESH);
HTMLTag myTag("A");
myTag.addAttribute("HREF",onTimer);
myTag.outputWithModifiedURLs(currentPage,theEnvironment);
theEnvironment->h_fputs(onTimer); theEnvironment->h_puts("</A>.");
}
} else if(wapToHtml && !stricmp(theTag.getName(),"/CARD")) processTag("/FORM",htmlLineInput);
else if(wapToHtml && !stricmp(theTag.getName(),"POSTFIELD")) {
// NEEDATTENTION we're assuming that all WAP <input>
// stuff is copied directly to <postfield> - not
// necessarily the case!
const char* val=theTag.getAttribute("VALUE");
if(val && *val!='$') {
theTag.setNameTo("INPUT");
theTag.addAttribute("TYPE","hidden");
processTag(theTag,htmlLineInput);
}
} else if(wapToHtml && !stricmp(theTag.getName(),"ANCHOR")) {
const char* title=theTag.getAttribute("TITLE");
if(title) {
if(wapLastTitle) delete[] wapLastTitle;
wapLastTitle=strnew(title); // NEEDATTENTION This can memory-leak (AccessOptions needs a destructor)
}
} else if(wapToHtml && !stricmp(theTag.getName(),"GO")) {
const char* href=theTag.getAttribute("HREF");
if(href && wapLastTitle) {
// NEEDATTENTION really && wapLastTitle?
// (NB if not, need to revise the statement below)
InString temp;
char* y=sortDirectories(href,currentPage);
temp.addString(URL_ENV); temp.addString(y);
delete[] y;
theTag.addAttribute("NAME",temp.getString());
theTag.addAttribute("VALUE",wapLastTitle);
theTag.addAttribute("TYPE","submit");
theTag.deleteAllByName("HREF");
theTag.setNameTo("INPUT");
processTag(theTag,htmlLineInput);
}
} else if(wapToHtml && (!stricmp(theTag.getName(),"/ANCHOR") || !stricmp(theTag.getName(),"/GO"))) ; // ignore
else theTag.outputTo(theEnvironment,currentPage);
// One last thing: Certain tags require initTheFont to be called again
if(!stricmp(theTag.getName(),"PRE")) inPre=1;
if(!(stricmp(theTag.getName(),"TT")
&& (stricmp(theTag.getName(),"TD") || processTables)
&& (stricmp(theTag.getName(),"/TABLE") || processTables)
&& stricmp(theTag.getName(),"PRE")
)) initTheFont(theEnvironment);
// (2000-10-06: See also "/TT" and "/PRE" above)
return(0);
}

int isMeaningful(const char* ch) {
for(int i=0; ch[i]; i++) if(isMeaningful(ch[i])) return(1);
return(0);
}
