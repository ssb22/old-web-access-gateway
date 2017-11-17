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
#include "cgilib.h"
#include "tagstuff.h"
#include "encoding.h"
#include "webget.h"
#include "!shutdn.h"
#include "extensions.h"
#include "lastupd.h"
#include "images.h"
#include "help.h"
#include "debug.h"
#include "action.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

void setHiddenDefaults(CGIEnvironment* theEnvironment) {
theEnvironment->setName(SHOW_URL_BOX,"1");
}

static int isCheckBox(const char* name) {
// For optionIsRedundant, below.  Should really return 0 only if
// it's a text box.
// This is because the KDE "Konqueror" browser sets check box values
// to the empty string instead of "1", and the previous behaviour of
// deleting all empty strings from the environment, while avoiding
// some problems with text boxes, interfered with the check box entries.
if(!(strcmp(name,URL_ENV)
&& strcmp(name,ISINDEX_ENV)
&& strcmp(name,USER_ENV)
&& strcmp(name,TABLE_TRIM)
&& strcmp(name,ENV_UNICODE_URL)
&& strcmp(name,LINK_FOLLOW))) return(0);
return(1);
}

int optionIsRedundant(const char* name,const char* value) {
// Bit of DeMorgan why not
if(!((strcmp(name,ENV_BGCOLOR)
&& strcmp(name,ENV_TEXT_COLOR)
&& strcmp(name,ENV_LINK_COLOR)
&& strcmp(name,ENV_ALINK_COLOR)
&& strcmp(name,ENV_VLINK_COLOR)
&& strcmp(name,ENV_BOLD_COLOUR)
&& strcmp(name,ENV_HOVER_BG)
&& strcmp(name,ENV_FONTSIZE))
|| strcmp(value,UNSPEC_COLOUR))) return(1);
#ifdef ENCODINGS_AVAILABLE
if(!(strcmp(name,ENV_LANGUAGE_TABLE)
|| strcmp(value,NO_CONVERS_VAL))) return(1);
#endif
if(!((strcmp(name,TABLE_TRIM) && strcmp(name,LINK_FOLLOW)) ||
*value || isCheckBox(name))) return(1);
return(0);
}

int formID=0; // For unique IDs on LABEL in forms

void outputMoreOptionsButton(const char* buttonName,CGIEnvironment* theEnvironment) {
const char* tmp=theEnvironment->getName(ENV_SHOW_OPTIONS,MAY_RETURN_NULL);
if(!tmp || strcmp(tmp,buttonName)) {
formID++; theEnvironment->h_printf("<LABEL FOR=%d><INPUT USESTYLE TYPE=Submit ID=%d NAME=" ENV_SHOW_OPTIONS " VALUE=\"%s\"></LABEL>\n",formID,formID,buttonName); // %s OK here
} else theEnvironment->h_printf("(%s)\n",buttonName); // %s OK here
}

void outputColourOption(const char* inputName,const char* currentValue,const char* value,const char* colourName,CGIEnvironment* theEnvironment,const char* jsName) {
formID++; theEnvironment->h_printf("<LABEL FOR=%d><INPUT USESTYLE TYPE=radio NAME=%s ID=%d VALUE=\"%s\"",formID,inputName,formID,value); // %s OK here
if(jsName) {
// eg. bgColor, fgColor.  Set old%s first  (%s OK here)
// (fgColor works on Mozilla but not on Netscape 4.6)
if(strcmp(value,UNSPEC_COLOUR)) theEnvironment->h_printf(" onClick=\"document.%s='%s'; return true\"",jsName,value); // %s OK here
else theEnvironment->h_printf(" onClick=\"document.%s=old%s; return true\"",jsName,jsName); // %s OK here
}
if(!strcmp(value,currentValue)) theEnvironment->h_fputs(" checked");
theEnvironment->h_putchar('>');
if(strcmp(value,UNSPEC_COLOUR)) theEnvironment->h_printf("<FONT COLOR=\"#%s\">",value); // %s OK here (I'm generating "value", not the user)
theEnvironment->h_printf("%s",colourName); // %s OK here
if(strcmp(value,UNSPEC_COLOUR)) theEnvironment->h_fputs("</FONT>");
theEnvironment->h_puts("</LABEL>");
}

void outputColourOptions(const char* inputName,const char* textInputName,CGIEnvironment* theEnvironment,const char* jsName) {
const char* currentValue=theEnvironment->getName(inputName,MAY_RETURN_NULL);
if(!currentValue) currentValue=UNSPEC_COLOUR;
theEnvironment->h_printf("<p>%s:\n",textInputName); // %s OK here
if(jsName) theEnvironment->h_printf("<SCRIPT LANGUAGE=JavaScript><!--\nvar old%s=document.%s;\n// --></SCRIPT>",jsName,jsName); // %s OK here
// NEEDATTENTION Trouble with the above is that it will set oldbgColor to document.bgColor AFTER the BODY tag has changed it to OUR colour (if they're changing existing settings instead of making new ones).  Needs fixing (eg. put the script in the HEAD ??)
outputColourOption(inputName,currentValue,UNSPEC_COLOUR,"Use browser setting",theEnvironment,jsName);
outputColourOption(inputName,currentValue,"FF0000","Red",theEnvironment,jsName);
outputColourOption(inputName,currentValue,"7F0000","Soft red",theEnvironment,jsName);
outputColourOption(inputName,currentValue,"FFFF00","Yellow",theEnvironment,jsName);
outputColourOption(inputName,currentValue,"FFFFFF","White",theEnvironment,jsName);
outputColourOption(inputName,currentValue,"00FF00","Green",theEnvironment,jsName);
outputColourOption(inputName,currentValue,"007F00","Soft green",theEnvironment,jsName);
outputColourOption(inputName,currentValue,"0000FF","Blue",theEnvironment,jsName);
outputColourOption(inputName,currentValue,"00007F","Soft blue",theEnvironment,jsName);
outputColourOption(inputName,currentValue,"000000","Black",theEnvironment,jsName);
}

int outputAllColourOptions(CGIEnvironment* theEnvironment) {
const char* tmp=theEnvironment->getName(ENV_SHOW_OPTIONS,MAY_RETURN_NULL); if(tmp && !strcmp(tmp,L_SET_COLOUR_OPTIONS)) {
theEnvironment->h_fputs("<P>"); theEnvironment->h_puts(L_COLOURS_INTRO);
outputColourOptions(ENV_BGCOLOR,L_BGCOLOUR,theEnvironment,"bgColor");
outputColourOptions(ENV_TEXT_COLOR,L_FGCOLOUR,theEnvironment,"fgColor"); // (fgColor works on Mozilla but not on Netscape 4.6)
outputColourOptions(ENV_LINK_COLOR,L_LINKCOLOUR,theEnvironment,NULL);
outputColourOptions(ENV_ALINK_COLOR,L_ALINK_COLOUR,theEnvironment,NULL);
outputColourOptions(ENV_VLINK_COLOR,L_VLINK_COLOUR,theEnvironment,NULL);
outputColourOptions(ENV_HOVER_BG,L_HOVER_BG,theEnvironment,NULL);
outputColourOptions(ENV_BOLD_COLOUR,L_BOLD_COLOUR,theEnvironment,NULL);
outputColourOptions(ENV_HEAD_COLOUR,L_HEAD_COLOUR,theEnvironment,NULL);
return 1;
} else {
theEnvironment->formDumpCurrentOption(ENV_BGCOLOR);
theEnvironment->formDumpCurrentOption(ENV_TEXT_COLOR);
theEnvironment->formDumpCurrentOption(ENV_LINK_COLOR);
theEnvironment->formDumpCurrentOption(ENV_ALINK_COLOR);
theEnvironment->formDumpCurrentOption(ENV_VLINK_COLOR);
theEnvironment->formDumpCurrentOption(ENV_HOVER_BG);
theEnvironment->formDumpCurrentOption(ENV_BOLD_COLOUR);
theEnvironment->formDumpCurrentOption(ENV_HEAD_COLOUR);
return 0;
}
}

void outputFontSizeOptions(CGIEnvironment* theEnvironment) {
const char* currentValue=theEnvironment->getName(ENV_FONTSIZE,MAY_RETURN_NULL);
if(!currentValue) currentValue=UNSPEC_COLOUR;
formID++; theEnvironment->h_printf("%s<BR><LABEL FOR=%d><INPUT USESTYLE TYPE=radio NAME=" ENV_FONTSIZE " id=%d VALUE=" UNSPEC_COLOUR "%s>%s</LABEL>",L_FONTSIZE_PROMPT,formID,formID,isdigit(currentValue[0])?"":" checked",L_BROWSER_SIZE); // %s OK here
int curVal=atoi(currentValue),size=1;
for(; size<=MAX_FONT_SIZE; size++) {
formID++; theEnvironment->h_printf("<BR><LABEL FOR=%d><INPUT USESTYLE TYPE=radio NAME=" ENV_FONTSIZE " ID=%d VALUE=%d%s><FONT SIZE=%d>%s %d</FONT></LABEL>\n",formID,formID,size,(size==curVal)?" checked":"",size,L_FONT_SIZE,size); // %s OK here
} for(; size<=(MAX_FONT_SIZE+MAX_HEADING_LEVEL); size++) {
formID++; theEnvironment->h_printf("<H%d><LABEL FOR=%d><INPUT USESTYLE TYPE=radio NAME=" ENV_FONTSIZE " ID=%d VALUE=%d%s>%s %d</LABEL></H%d>\n",size-MAX_FONT_SIZE,formID,formID,size,(size==curVal)?" checked":"",L_HEADING_FONT_SIZE,size-MAX_FONT_SIZE,size-MAX_FONT_SIZE); // %s OK here
}
}

static int outBR=1;
/*static*/ int checkOptionJustDumps=0;

void outputCheckOption(const char* option,const char* description,CGIEnvironment* theEnvironment) {
if(checkOptionJustDumps) theEnvironment->formDumpCurrentOption(option);
else {
if(outBR) theEnvironment->h_fputs("<BR>");
formID++; theEnvironment->h_printf("<LABEL FOR=%d><INPUT USESTYLE TYPE=checkbox ID=%d NAME=%s%s>%s</LABEL>\n",formID,formID,option,theEnvironment->getName(option,MAY_RETURN_NULL)?" checked":"",description); // %s OK here
Helper::hackyGlobalHelper()->help(option,theEnvironment);
// Need to call ensureOptionNotInCookie to make sure
// that any cookie sent back to the user does NOT
// include this option.  Otherwise, if the checkbox is
// unchecked and the option is on in the cookie, the
// option will be on and there will be no way for the
// user to turn it off, which is not a good thing.
theEnvironment->ensureOptionNotInCookie(option);
}
outBR=1;
}

void outputIsIndex(CGIEnvironment* theEnvironment) {
theEnvironment->h_puts(MakeRelativeActionURL("<FORM ACTION=\"","\" METHOD=GET TARGET=_top>"));
theEnvironment->formDumpCurrentOptions(ENV_PREFIX);
char* tempURL=strnew(theEnvironment->getName(URL_ENV,MAY_NOT_RETURN_NULL));
char* p=strrchr(tempURL,'?'); if(p) *p=0;
(*theEnvironment)<<"<INPUT USESTYLE TYPE=hidden NAME=" URL_ENV " VALUE=\""<<tempURL<<"\">\n";
delete[] tempURL;
theEnvironment->h_fputs("<HR>");
theEnvironment->h_fputs(L_ISINDEX_START);
theEnvironment->h_fputs("<LABEL FOR=indexbox><INPUT USESTYLE TYPE=text ID=indexbox NAME=" ISINDEX_ENV "></LABEL><HR></FORM>");
}

int outputOptionsLink(CGIEnvironment* theEnvironment,const char* defaultURL) {
checkPoint();
if(preprocessOnly(theEnvironment)) return 0;
int needALine=0,inAForm=0;
if(theEnvironment->getName(SHOW_URL_BOX,MAY_RETURN_NULL)) {
checkPoint();
theEnvironment->h_puts(MakeRelativeActionURL("<FORM ACTION=\"","\" METHOD=GET TARGET=_top>"));
theEnvironment->formDumpCurrentOptions(ENV_PREFIX);
if(!defaultURL) {
defaultURL=theEnvironment->getName(URL_ENV,MAY_RETURN_NULL);
if(!defaultURL) defaultURL="";
}
theEnvironment->h_fputs("<LABEL FOR=urlbox><INPUT USESTYLE TYPE=text ID=urlbox NAME=" URL_ENV " VALUE=\"");
theEnvironment->h_fputs(defaultURL);
theEnvironment->h_putchar('"');
if(theEnvironment->getName(LONG_URL_BOX,MAY_RETURN_NULL)) theEnvironment->h_fputs(" SIZE=100");
theEnvironment->h_puts("></LABEL><LABEL FOR=sub><INPUT USESTYLE TYPE=submit ID=sub VALUE=OK></LABEL>");
needALine=inAForm=1;
}
if(theEnvironment->getName(DONT_SHOW_OPTIONS_LINK,MAY_RETURN_NULL)==NULL) {
checkPoint();
theEnvironment->h_fputs(MakeRelativeActionURL("<A HREF=\"","?"));
if(theEnvironment->urlDumpCurrentOptions("")) theEnvironment->h_putchar('&');
theEnvironment->h_fputs(ENV_SHOW_OPTIONS "=1\" TARGET=_top>");
theEnvironment->h_fputs(L_CHANGE_ACCESS_OPTIONS);
theEnvironment->h_puts("</A>");
needALine=1;
}
if(inAForm) theEnvironment->h_fputs("</FORM>");
if(!needALine) {
// We'd better output a comment at the very least
theEnvironment->h_puts("<!--");
theEnvironment->h_puts(L_COMMENT_STRING); // (don't indent it - it's multiline)
theEnvironment->h_puts("-->");
}
return(needALine);
}

int outputPasswordOptions(CGIEnvironment* theEnvironment) {
const char* tmp=theEnvironment->getName(ENV_SHOW_OPTIONS,MAY_RETURN_NULL); if(tmp && !strcmp(tmp,L_SET_PWD_OPTIONS)) {
theEnvironment->h_fputs("<P>"); theEnvironment->h_puts(L_PWD_INTRO);
const char* defaultUser=theEnvironment->getName(USER_ENV,MAY_RETURN_NULL); if(!defaultUser) defaultUser="";
const char* defaultPassword=theEnvironment->getName(PWD_ENV,MAY_RETURN_NULL); if(!defaultPassword) defaultPassword="";
formID++; theEnvironment->h_printf("<BR><LABEL FOR=%d ACCESSKEY=%s>%s: <INPUT USESTYLE TYPE=text ID=%d NAME=" USER_ENV " VALUE=\"",formID,L_PWD_USER_ACCESSKEY,L_PWD_USER,formID); theEnvironment->h_fputs(defaultUser); theEnvironment->h_puts("\"></LABEL>"); // %s OK here
formID++; theEnvironment->h_printf("<BR><LABEL FOR=%d ACCESSKEY=%s>%s: <INPUT USESTYLE TYPE=password ID=%d NAME=" PWD_ENV " VALUE=\"",formID,L_PWD_PWD_ACCESSKEY,L_PWD_PWD,formID); theEnvironment->h_fputs(defaultPassword); theEnvironment->h_puts("\"></LABEL>"); // %s OK here
return 1;
} else {
theEnvironment->formDumpCurrentOption(USER_ENV);
theEnvironment->formDumpCurrentOption(PWD_ENV);
return 0;
}
}

inline void outputAccessHeading(const char* heading,CGIEnvironment* theEnvironment) {
theEnvironment->h_fputs("<H3>");
theEnvironment->h_fputs(heading);
theEnvironment->h_puts("</H3>");
outBR=0;
}

int outputAccessOptions(CGIEnvironment* theEnvironment) {
const char* tmp=theEnvironment->getName(ENV_SHOW_OPTIONS,MAY_RETURN_NULL);
if(!tmp || strcmp(tmp,L_SET_ACCESS_OPTIONS)) checkOptionJustDumps=1;
else {
Helper::outputHelpSwitch(theEnvironment);
theEnvironment->h_fputs("<BR>");
theEnvironment->h_puts(L_ACCESS_INTRO);
}

// Scripts and applets
if(!checkOptionJustDumps) outputAccessHeading(L_A_SCRIPT_HEADING,theEnvironment);
outputCheckOption(ENV_PREFIX "0",L_A_SCRIPTS,theEnvironment);
outputCheckOption(ENV_PREFIX "A",L_A_NO_SCRIPT_GUESS,theEnvironment);
outputCheckOption(ENV_PREFIX "J",L_A_NO_ABBR_URLS,theEnvironment);
outputCheckOption(ENV_PREFIX "4",L_A_APPLETS,theEnvironment);
outputCheckOption(ENV_PREFIX "p",L_A_PARAMS,theEnvironment);
outputCheckOption(ENV_PREFIX "L",L_A_EMBED,theEnvironment);
outputCheckOption(ENV_PREFIX "M",L_A_FLASH,theEnvironment);

// Frames and tables
if(!checkOptionJustDumps) outputAccessHeading(L_A_FRAMES_HEADING,theEnvironment);
outputCheckOption(ENV_PREFIX "3",L_A_FRAMES,theEnvironment);
outputCheckOption(ENV_PREFIX "r",L_A_GENFRAMES,theEnvironment);
outputCheckOption(ENV_PREFIX "9",L_A_TABLES,theEnvironment);
outputCheckOption(ENV_PREFIX "g",L_A_GENTAB,theEnvironment);

// Images
if(!checkOptionJustDumps) outputAccessHeading(L_A_IMG_HEADING,theEnvironment);
outputCheckOption(ENV_PREFIX "c",L_A_LINKIMG,theEnvironment);
outputCheckOption(ENV_PREFIX "e",L_A_IMGMAP,theEnvironment);
outputCheckOption(ENV_PREFIX "n",L_A_ALTREPEAT,theEnvironment);
outputCheckOption(ENV_PREFIX "v",L_A_NOSUBMIT,theEnvironment);
outputCheckOption(ENV_PREFIX "z",L_A_REMOVEIMGS,theEnvironment);

// Style
if(!checkOptionJustDumps) outputAccessHeading(L_A_STYLE_HEADING,theEnvironment);
outputCheckOption(ENV_PREFIX "j",L_A_STYLE,theEnvironment);
outputCheckOption(ENV_PREFIX "1",L_A_BODY,theEnvironment);
outputCheckOption(ENV_PREFIX "2",L_A_FONT,theEnvironment);
outputCheckOption(ENV_PREFIX "5",L_A_BLINK,theEnvironment);
outputCheckOption(ENV_PREFIX "6",L_A_UL,theEnvironment);
outputCheckOption(ENV_PREFIX "7",L_A_ITAL,theEnvironment);
outputCheckOption(ENV_PREFIX "a",L_A_MARQUEE,theEnvironment);

// Screenreader
if(!checkOptionJustDumps) outputAccessHeading(L_A_SPEECH_HEADING,theEnvironment);
outputCheckOption(ENV_PREFIX "q",L_A_SPACEDETECT,theEnvironment);
outputCheckOption(ENV_PREFIX "x",L_A_BANNERMOVE,theEnvironment);
outputCheckOption(ENV_PREFIX "b",L_A_SMILEY,theEnvironment);
outputCheckOption(ENV_PREFIX "8",L_A_LINKS,theEnvironment);
outputCheckOption(ENV_PREFIX "w",L_A_ENDPAGE,theEnvironment);
outputCheckOption(ENV_PREFIX "k",L_A_REFRESH,theEnvironment);

// Miscellaneous
if(!checkOptionJustDumps) outputAccessHeading(L_MISC_HEADING,theEnvironment);
outputCheckOption(ENV_PREFIX "I",L_A_ADOBE,theEnvironment);
outputCheckOption(ENV_PREFIX "u",L_A_NOBR,theEnvironment);
outputCheckOption(ENV_PREFIX "y",L_A_HEIGHTSTRIP,theEnvironment);
outputCheckOption(ENV_PREFIX "F",L_A_SPACERSTRIP,theEnvironment);
outputCheckOption(ENV_PREFIX "G",L_A_ACRONYM,theEnvironment);
if(checkOptionJustDumps) theEnvironment->formDumpCurrentOption(ENV_PREFIX "m");
else {
const char* currentValue=theEnvironment->getName(ENV_PREFIX "m",MAY_RETURN_NULL);
if(!currentValue) currentValue=DEFAULT_HEADING_PROMOTION;
theEnvironment->h_fputs("<BR>");
theEnvironment->h_puts(L_HEADING_PROMPT);
for(int size=1; size<=MAX_HEADING_LEVEL; size++) {
formID++; theEnvironment->h_printf("<H%d><LABEL FOR=%d><INPUT USESTYLE TYPE=radio NAME=" ENV_PREFIX "m ID=%d VALUE=%d%s>%s %d</LABEL></H%d>\n",size,formID,formID,size,(size==atoi(currentValue))?" checked":"",L_HEADING_TYPE,size,size); // %s OK here
}
Helper::hackyGlobalHelper()->help(ENV_PREFIX "m",theEnvironment);
}
int retVal=!checkOptionJustDumps;
checkOptionJustDumps=0;
return retVal;
}
int outputFontOptions(CGIEnvironment* theEnvironment) {
const char* tmp=theEnvironment->getName(ENV_SHOW_OPTIONS,MAY_RETURN_NULL); if(tmp && !strcmp(tmp,L_SET_SIZE_OPTIONS)) {
outputFontSizeOptions(theEnvironment);
return 1;
} else {
theEnvironment->formDumpCurrentOption(ENV_FONTSIZE);
return 0;
}
}
int outputMiscOptions(CGIEnvironment* theEnvironment) {
const char* tmp=theEnvironment->getName(ENV_SHOW_OPTIONS,MAY_RETURN_NULL);
if(!tmp || strcmp(tmp,L_SET_MISC_OPTIONS)) {
checkOptionJustDumps=1;
theEnvironment->formDumpCurrentOption(TABLE_TRIM);
theEnvironment->formDumpCurrentOption(LINK_FOLLOW);
} else {
// Search (includes the intro)
Helper::outputHelpSwitch(theEnvironment);
outputAccessHeading(L_SEARCH_HEADING,theEnvironment);
theEnvironment->h_puts(L_MISCOPTS_INTRO);
const char* defaultURL=theEnvironment->getName(TABLE_TRIM,MAY_RETURN_NULL); if(!defaultURL) defaultURL="";
theEnvironment->h_fputs("<LABEL FOR=tabtrim>");
theEnvironment->h_fputs(L_TABLE_TRIM);
theEnvironment->h_fputs(": <INPUT USESTYLE TYPE=text ID=tabtrim NAME=" TABLE_TRIM " VALUE=\"");
theEnvironment->h_fputs(defaultURL); theEnvironment->h_puts("\"></LABEL>");
Helper::hackyGlobalHelper()->help(TABLE_TRIM,theEnvironment);

defaultURL=theEnvironment->getName(LINK_FOLLOW,MAY_RETURN_NULL); if(!defaultURL) defaultURL="";
theEnvironment->h_fputs("<BR><LABEL FOR=linkfol>");
theEnvironment->h_fputs(L_LINK_FOLLOW);
theEnvironment->h_fputs(": <INPUT USESTYLE TYPE=text ID=linkfol NAME=" LINK_FOLLOW " VALUE=\"");
theEnvironment->h_fputs(defaultURL); theEnvironment->h_puts("\"></LABEL>");
Helper::hackyGlobalHelper()->help(LINK_FOLLOW,theEnvironment);
}
// Navigation
if(!checkOptionJustDumps) outputAccessHeading(L_NAV_HEADING,theEnvironment);
outputCheckOption(SHOW_URL_BOX,L_OPTS_URLBOX,theEnvironment);
outputCheckOption(LONG_URL_BOX,L_OPTS_LONGBOX,theEnvironment);
outputCheckOption(DONT_SHOW_OPTIONS_LINK,L_OPTS_ACCESSLINK,theEnvironment);
outputCheckOption(DONT_SHOW_CHARSET,L_OPTS_CHARSET,theEnvironment);
outputCheckOption(ENV_PREFIX "l",L_OPTS_DATESTAMP,theEnvironment);
// ENV_PREFIX "D" - don't do Javascript status line
// If Javascript is not enabled then we want it anyway
// (save bandwidth on mobile phones etc)
// So propagate it anyway if it's set, or if it's unset,
// set it if noscript
if(checkOptionJustDumps && !theEnvironment->getName(ENV_PREFIX "D",MAY_RETURN_NULL)) theEnvironment->h_puts("<NOSCRIPT><INPUT TYPE=hidden NAME=" ENV_PREFIX "D VALUE=1></NOSCRIPT>"); else outputCheckOption(ENV_PREFIX "D",L_OPTS_STATUSLINE,theEnvironment);

// Browser quirks
if(!checkOptionJustDumps) outputAccessHeading(L_BROWSER_HEADING,theEnvironment);
#ifdef CAN_SWITCH_SSL
outputCheckOption(ENV_AVOID_SWITCH,L_OPTS_NOSSL,theEnvironment);
#endif
outputCheckOption(ENV_PREFIX "d",L_OPTS_EMBED,theEnvironment);
outputCheckOption(ENV_PREFIX "i",L_OPTS_NETFONT,theEnvironment);
outputCheckOption(ENV_PREFIX "f",L_OPTS_HEAD,theEnvironment);
// Cookies
if(!checkOptionJustDumps) outputAccessHeading(L_COOKIES_HEADING,theEnvironment);
outputCheckOption(ENV_PREFIX "C",L_OPTS_COOKIEOPTS,theEnvironment);
#ifdef LYNX_USER_AGENT
outputCheckOption(ENV_PREFIX "B",L_OPTS_DONTSTORE,theEnvironment);
theEnvironment->formDumpCurrentOption(ENV_COOKIES);
// **** NEEDATTENTION - Message about not supporting
// **** cookies if not LYNX_USER_AGENT (although does
// **** support some if can USE_GNU_SOCKETS)?  If no
// **** LYNX_SSL then only USE_GNU_SOCKETS so not to
// **** worry?  What about DSA-210-1?
#endif
// Miscellaneous
if(!checkOptionJustDumps) outputAccessHeading(L_MISC_HEADING,theEnvironment);
outputCheckOption(ENV_PREFIX "o",L_OPTS_PARASINDENT,theEnvironment);
outputCheckOption(ENV_PREFIX "s",L_OPTS_HARDWRAP,theEnvironment);
outputCheckOption(ENV_PREFIX "h",L_OPTS_RESET,theEnvironment);
outputCheckOption(ENV_PREFIX "H",L_OPTS_HIDDEN,theEnvironment);
outputCheckOption(ENV_PREFIX "t",L_OPTS_NOALT,theEnvironment);
outputCheckOption(ENV_PREFIX "K",L_OPTS_1252,theEnvironment);
int retVal=!checkOptionJustDumps;
checkOptionJustDumps=0;
return retVal;
}

#if (defined(Internal_Extensions1) || defined(External_Extensions) || defined(Script_Extensions))
#define EXTENSIONS_INCLUDED
#endif
#ifdef EXTENSIONS_INCLUDED
int outputExtensionOptions(CGIEnvironment* theEnvironment) {
const char* tmp=theEnvironment->getName(ENV_SHOW_OPTIONS,MAY_RETURN_NULL);
if(!tmp || strcmp(tmp,L_SET_EXTENSION_OPTIONS)) checkOptionJustDumps=1;
else theEnvironment->h_puts(L_EXTENSION_INFO);
#ifdef Internal_Extensions1
callExtensions(NULL,theEnvironment,1);
#endif
#ifdef External_Extensions
callFilters(NULL,NULL,theEnvironment);
#endif
#ifdef Script_Extensions
appendScriptExtensionsTo(theEnvironment,1);
#endif
int retVal=!checkOptionJustDumps;
checkOptionJustDumps=0;
return retVal;
}
#endif

void outputAllOptions(CGIEnvironment* theEnvironment) {
if(theEnvironment->isEmpty()) setHiddenDefaults(theEnvironment);
theEnvironment->h_puts(MakeRelativeActionURL("<FORM ACTION=\"","\" METHOD=GET>"));
// The following is undocumented:
theEnvironment->formDumpCurrentOption(ENV_ENABLE_IGNORE);
theEnvironment->formDumpCurrentOption(ENV_NONLOCAL_PASSWORD);
if(
// Special options first:
// NOTE: Do NOT use ||.  Use |.
// We want everything to be evaluated!
outputPasswordOptions(theEnvironment)
| outputAllColourOptions(theEnvironment)
#ifdef ENCODINGS_AVAILABLE
| outputEncodingOptions(theEnvironment)
#endif
| outputAccessOptions(theEnvironment)
| outputFontOptions(theEnvironment)
| outputMiscOptions(theEnvironment)
#ifdef EXTENSIONS_INCLUDED
| outputExtensionOptions(theEnvironment)
#endif
// Put an HR if ANY of those options were actually
// output
) theEnvironment->h_puts("<HR>");
theEnvironment->formDumpCurrentOption(ENV_INLINE_HELP); // Propagate that setting
const char* defaultURL=theEnvironment->getName(URL_ENV,MAY_RETURN_NULL); if(!defaultURL) defaultURL="";
formID++; theEnvironment->h_printf("<LABEL FOR=%d ACCESSKEY=%s>%s: <INPUT USESTYLE TYPE=text ID=%d NAME=" URL_ENV " VALUE=\"",formID,L_URL_ACCESSKEY,L_URL_PROMPT,formID); // %s OK here
theEnvironment->h_fputs(defaultURL);
theEnvironment->h_putchar('"');
if(theEnvironment->getName(LONG_URL_BOX,MAY_RETURN_NULL)) theEnvironment->h_fputs(" SIZE=100");
theEnvironment->h_puts("></LABEL>");
theEnvironment->h_puts("<LABEL FOR=sub2><INPUT USESTYLE TYPE=Submit ID=sub2 VALUE=\"Get page\"></LABEL>");
outputMoreOptionsButton(L_SET_PWD_OPTIONS,theEnvironment);
#ifdef ENCODINGS_VISIBLE
outputMoreOptionsButton(L_SET_LANGUAGE_OPTIONS,theEnvironment);
#endif
outputMoreOptionsButton(L_SET_ACCESS_OPTIONS,theEnvironment);
outputMoreOptionsButton(L_SET_COLOUR_OPTIONS,theEnvironment);
outputMoreOptionsButton(L_SET_SIZE_OPTIONS,theEnvironment);
outputMoreOptionsButton(L_SET_MISC_OPTIONS,theEnvironment);
#ifdef EXTENSIONS_INCLUDED
outputMoreOptionsButton(L_SET_EXTENSION_OPTIONS,theEnvironment);
#endif
#ifdef ENCODINGS_VISIBLE
outputCheckOption(ENV_NOT_VI,L_DISABLE_SIGHT_OPTIONS,theEnvironment);
#else
theEnvironment->h_fputs("<BR>");
theEnvironment->h_puts(L_NO_ENCODINGS);
#endif
// 2002-01-10: The help button used to be first, but that
// needs changing because some browsers (e.g. some
// versions of Mozilla) automatically add the first button
// on the form when the user presses Return.  It should be
// Get Page, not Help.
theEnvironment->h_fputs("<BR>");
theEnvironment->h_fputs(L_HELP_PROMPT);
theEnvironment->h_fputs(": <LABEL FOR=hlp ACCESSKEY=");
theEnvironment->h_fputs(L_HLP_ACCESSKEY);
theEnvironment->h_fputs("><INPUT USESTYLE TYPE=Submit ID=hlp NAME=" HELP_ENV " VALUE=\"");
theEnvironment->h_fputs(L_HELP);
theEnvironment->h_puts("\"></LABEL>");
theEnvironment->h_puts("<HR>"); // Don't change to h_fputs (see comment in access.c++ about Mozilla bug)
outputLastUpdate(theEnvironment);
formID++; theEnvironment->h_printf("<LABEL FOR=%d><INPUT USESTYLE TYPE=Submit ID=%d NAME=" ENV_SHOW_OPTIONS " VALUE=\"%s\"></LABEL></FORM>",formID,formID,L_EXIT); // %s OK here
}

void outputBodyAttr(const char* inputName,const char* htmlAttrib,CGIEnvironment* theEnvironment) {
const char* currentValue=theEnvironment->getName(inputName,MAY_RETURN_NULL);
if(currentValue && strcmp(currentValue,UNSPEC_COLOUR)) theEnvironment->h_printf(" %s=\"#%.9s\"",htmlAttrib,currentValue); // First %s OK
}

static int wasColour = 0; // NEEDATTENTION hacky - see below
// (for "always use heading" sizes & quirky browsers)
void initTheFont(CGIEnvironment* theEnvironment,int unconditional,const char* colour) {
const char* x=theEnvironment->getName(ENV_FONTSIZE,MAY_RETURN_NULL);
if(!x && unconditional) x="3"; // NEEDATTENTION Awful hack!
int i;
if(x && (i=atoi(x))!=0) {
if(!colour) colour=theEnvironment->getName(ENV_TEXT_COLOR,MAY_RETURN_NULL);
if(colour && !strcmp(colour,UNSPEC_COLOUR)) colour=NULL;
InString colourString;
if(colour) {
wasColour = 1;
colourString.addString(" COLOR=\"#");
colourString.addString(colour);
colourString.addCharacter('\"');
} else wasColour = 0;
if(i>MAX_FONT_SIZE) {
static int alreadyHad = 0; // NEEDATTENTION hack
if(unconditional==2) alreadyHad=0; // part of above hack (see access.c++ - called when </H> in page - & doesn't matter that above 'unconditional' will also be activated because this hack only applies for "heading" font sizes)
if(!alreadyHad) theEnvironment->h_printf("<H%d>",i-MAX_FONT_SIZE);
alreadyHad = 1; // don't want spurious newlines etc
if(colour) theEnvironment->h_printf("<FONT%.99s>",colourString.getString());
} else {
theEnvironment->h_printf("<FONT SIZE=%d%.99s>",i,colourString.getString());
}
} // NB no \n in either of the above printf's
}

void closeTheFontTag(CGIEnvironment* theEnvironment,int unconditional) {
// Note when this is called!  So don't close a heading tag
// (don't want surplus new lines)
const char* x=theEnvironment->getName(ENV_FONTSIZE,MAY_RETURN_NULL);
if(!x && unconditional) x="3"; // NEEDATTENTION Awful hack! (as above)
int i;
if(x && (i=atoi(x))!=0) {
if(i>MAX_FONT_SIZE) {
if(wasColour) theEnvironment->h_fputs("</FONT>"); // NEEDATTENTION hacky (not proper stack; globals; ...)
// theEnvironment->h_printf("</H%d>",i-MAX_FONT_SIZE);
} else theEnvironment->h_fputs("</FONT>");
} // NB no \n
}

void outputBodyTag(CGIEnvironment* theEnvironment) {
#ifdef ENCODINGS_AVAILABLE
theEnvironment->setCharset(NeedCharset);
#endif
theEnvironment->h_fputs("<BODY");
outputBodyAttr(ENV_BGCOLOR,"BGCOLOR",theEnvironment);
outputBodyAttr(ENV_TEXT_COLOR,"TEXT",theEnvironment);
outputBodyAttr(ENV_LINK_COLOR,"LINK",theEnvironment);
outputBodyAttr(ENV_ALINK_COLOR,"ALINK",theEnvironment);
outputBodyAttr(ENV_VLINK_COLOR,"VLINK",theEnvironment);
theEnvironment->h_puts(">");
{
// Slightly modified version of outputBodyAttr :
const char* currentValue=theEnvironment->getName(ENV_HOVER_BG,MAY_RETURN_NULL);
if(currentValue && strcmp(currentValue,UNSPEC_COLOUR)) theEnvironment->h_printf("<style><!--\nA:hover { background: #%.9s; } --></style>",currentValue);
}
// colours: also syntax highlight the text? NEEDATTENTION
initTheFont(theEnvironment,0,NULL);

/*  time_t tmp=time(NULL); struct tm* t=gmtime(&tmp);
if (!(t->tm_year<EXPIRY_YEAR || (t->tm_year==EXPIRY_YEAR && t->tm_mon<EXPIRY_MONTH))) {
theEnvironment->h_puts(EXPIRY_MESSAGE);
theEnvironment->h_puts("<HR>"); // Don't change to h_fputs (see comment in access.c++ about Mozilla bug)
// DON'T output details here (top of every page!) - you
// don't know what context it'll be in
} */
#ifdef SHUTDOWN_MESSAGE
theEnvironment->h_puts(SHUTDOWN_MESSAGE); theEnvironment->h_fputs("<P>");
#endif
#ifdef MSG_FOR_DOMAIN_CONTAINING
const char* x=getenv("HTTP_REMOTE_HOST");
if(x && stristr(x,MSG_FOR_DOMAIN_CONTAINING)) theEnvironment->h_puts(MSG_FOR_DOMAIN); theEnvironment->h_fputs("<P>");
#endif
}
