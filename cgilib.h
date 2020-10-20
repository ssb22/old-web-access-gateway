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
// -*- C++ -*-
#ifndef CGILIB_H
#define CGILIB_H

#include <stdio.h>
#include "platform.h"
#include "cookie.h"
#include "subset.h"
#include "options.h"

#include "literals.h"

#define HO_BOLD "\x1"
#define HO_NOBOLD "\x2"
#define HO_ITAL "\x3"
#define HO_NOITAL "\x4"
#define HO_RULE "\x5"
#define HO_H2 "\x6"
#define HO_NOH2 "\x8"
#define HO_H3 "\x9"
#define HO_NOH3 "\xa"

/// Returns non-0 if an option is redundant, ie. can be discarded before processing begins
int optionIsRedundant(const char* name,const char* value);
// (If using this code elsewhere, you need to provide this function,
// or make it an inline that always returns 0)

/// An enumeration for catching errors
enum MayReturnNull { MAY_RETURN_NULL, MAY_NOT_RETURN_NULL };
enum CookieStuff { NoCookies, RelyOnCookies, DoingCookies,
NoCookiesOrCompression };
/// A class for the CGI environment (input query and output HTML)
enum CgiExceptionDealtWith {};
class CGIEnvironment { // Constructor also inits rest of CGI, destructor exits, etc
public:
CGIEnvironment(const char* cookiePrefix="",const char* usrWantsCookiesVar=""); // Any exceptions that occur in the constructor will be printed and re-thrown; new should return NULL
// (cookiePrefix and usrWantsCookiesVar must be literals)
~CGIEnvironment();
const char* getName(const char* name,MayReturnNull mayReturnNull) const;
// setName is only used by the encoding routines; it replaces the old value
void setName(const char* name,const char* value) { theData.setByCaseSensitiveName(name,value); }
void deleteName(const char* name) { theData.deleteAllByCaseSensitiveName(name); }
void outHTMLParagraph(const char* paragraph) { h_fputs("<p> "); outHTMLLine(paragraph); h_puts("</p>"); }
void outHTMLLine(const char* line);
void exceptionHandle(const char* string);
void h_puts(const char* string);
void h_fputs(const char* string) { whereToOutput->addString(string); }
void h_fputs(const char* string,int len) { whereToOutput->addBytes(string,len); }
void h_printf(const char* format,...);
CGIEnvironment& operator<<(const char* s) { h_fputs(s); return(*this); }
int h_putchar(int c) { whereToOutput->addCharacter(c); return c; }

void outputToMainPage() { whereToOutput=&theOutputData; }
void outputToHeader() { whereToOutput=&theHeaderData; }
void outputToNothing() { whereToOutput=&theDevNull; }
int outputtingToMainPage() { return (whereToOutput==&theOutputData); }
void outputBodyForNow() { oldWhereToOutput=whereToOutput; whereToOutput=&theOutputData; }
void restoreOutputSetting() { whereToOutput=oldWhereToOutput; }
void dontAddServiceTitle() { addServiceTitle=0; }
void setCharset(const char* charset) { theCharset=charset; }
/// @return non-0 if it actually wrote anything
int lynxDumpExcludePrefix(const char* prefix,FILE* fp) const;
int queryDumpExcludePrefix(const char* prefix,InString &url,int addQuestionMark) const;
// formDumpCurrentOptions assumes inside a form;
// urlDumpCurrentOptions outputs eg. x=y&a=b&s=t
// (returns 1 on success)
// Both include only options begining with 'prefix'
// formDumpCurrentOption (singular) needs exact match
// and assumes at most one of them
void formDumpCurrentOptions(const char* prefix) { formButOneDumpCurrentOptions(prefix,NULL); }
void formButOneDumpCurrentOptions(const char* prefix,const char* except);
void formDumpCurrentOption(const char* exact);
int urlDumpCurrentOptions(const char* prefix);
void cookieDumpCurrentOptions(InString &s) {
urlDumpCurrentOptions(s,theCookiePrefix,DoingCookies);
}
void urlDumpOptionsForRedirect() {
// For redirection to another gateway.  Should not
// rely on cookies at all, since these probably won't
// be sent to the other gateway.
// Should not use URL compression either, since it
// might expand to a different set of options at the
// other gateway.
InString s; urlDumpOptionsForRedirect(s);
h_fputs(s.getString());
}
void urlDumpOptionsForRedirect(InString &s) {
urlDumpCurrentOptions(s,"",NoCookiesOrCompression);
}
int isEmpty() const { return(!theData.getNumberOfValues()); }
// All of the following must use theOutputData, not
// whereToOutput
// (2001-04-14 theRPSforFramesetsOnly: Hack because
// theRealPageStart is also being used for banner moving.
// If guessed JavaScript links occurr first in a page,
// theRPSforFramesetsOnly is advanced so that they can be
// deleted if it turns out to be a frameset;
// theRealPageStart is not advanced so they can be moved
// to the bottom if it's a banner that needs moving.
void setStartOfRealPage() { theRPSforFramesetsOnly=theRealPageStart=theOutputData.length(); }
void setStartOfRealPageForFramesetOnly() { theRPSforFramesetsOnly=theOutputData.length(); }
bool nothingSinceStartOfRealPage() const { return (theRPSforFramesetsOnly>=theOutputData.lengthMinusWhitespace()); }
void moveAChunkToTheEnd(int chunkEnd) { if(chunkEnd>theRealPageStart) theOutputData.moveAChunkToTheEnd(theRealPageStart,chunkEnd); } // NEEDATTENTION condition shouldn't really be there (bug workaround)
void deleteTheSpeilIfNotAlreadyDone() { if(theRPSforFramesetsOnly) { theOutputData.removeFirstFewCharacters(theRPSforFramesetsOnly); theRPSforFramesetsOnly=theRealPageStart=0; addArtificialNoframes=1; } }
long getPos() const { return(theOutputData.length()); }
const char* dataFromPos(long pos) const { return(theOutputData.getString()+pos); }
void truncateAtPos(long pos) { theOutputData.truncateAt((int)pos); }
// NEEDATTENTION Sort these stupid 'long's out
int hasAdvancedSince(long p) const { return(theOutputData.length()>p); }
// Putting a CookieTable here for convenience
// (hack 2000-07-15)
CookieTable theCookies;
// User cookie support is, of course, a completely different thing...
int userCookieSupportConfirmed() const { return theUserSupportsCookies; }
CookieStuff relyOnCookies() const { return((userCookieSupportConfirmed() && userWantsCookies())?RelyOnCookies:NoCookies); }
int userWantsCookies() const { return((getName(theUsrWantsCookiesVar,MAY_RETURN_NULL))!=NULL); }
void ensureOptionNotInCookie(const char* option) {
cookieAvoidThese.addCharacter('&');
cookieAvoidThese.addString(option);
cookieAvoidThese.addCharacter('&');
}
#ifdef MULTIPLE_STYLES_SUPPORTED
void setStyle(const char* style) { theStyle=style; }
const char* getStyle() const {
const char* c=getName(ENV_PREFERRED_STYLE,MAY_RETURN_NULL);
return (c?((*c)?c:NULL):theStyle);
// (If empty, returns NULL, since no subdirectory)
}
#endif
// initOutputData was called internally but is now also
// called by the rating exception handler
void initOutputData() { theOutputData.clear(); }
protected:
Subset* selectOptionsToDump(const char* prefix,CookieStuff doRelyOnCookies,const char* except=NULL) const;
// compressOptions returns a suitable value for
// CGI_COMPRESSION (or 0).  If successful, it de-selects
// the elements of s that are specified in the compressed
// preset.
char compressOptions(Subset* s) const;
// decompressOptions calls addName(name,value) as
// appropriate (does nothing if c==0, and doesn't replace
// existing names)
void decompressOptions(char c);
int urlDumpCurrentOptions(InString &str,const char* prefix,CookieStuff doRelyOnCookies) const;
void outputHiddenOption(const char* lhs,const char* rhs) {
// NB This doesn't check the cookie stuff any more
// selectOptionsToDump does, but if you're calling it
// in other contexts, be careful
// Note: Hidden options cannot contain the " character here
(*this)<<"<INPUT TYPE=hidden NAME=\""<<lhs<<"\" VALUE=\""<<rhs<<"\">\n";
}
InString theOutputData,theHeaderData,theDevNull;
InString *whereToOutput, *oldWhereToOutput;
int theRealPageStart,theRPSforFramesetsOnly,addArtificialNoframes;
const char* theCharset; char addServiceTitle;
void outputHeader(
#ifndef BRIEF_RESPONSE
long contentLength=-1
#endif
);
void addName(const char* name,const char* value); // Copies them
void cleanup();
public: // hack for some (buggy?) versions of gcc
class CgiInput {
public:
CgiInput() {};
virtual ~CgiInput() {};
virtual int getCharacter()=0; // Returns EOF if read past end
};
void saveTo(const char* filename) const { theData.saveTo(filename); }
void loadFrom(const char* filename) { theData.loadFrom(filename); }
protected:
void buildCGIInput(CgiInput &i,int dontReplaceExisting=0);
int tryDecodingMultipart();
void outHTMLCharacter(int c);
StringTable theData; // This is NOT theOutputData !
// (in case you like reading files from the bottom up)
int theUserSupportsCookies;
const char *theCookiePrefix,*theUsrWantsCookiesVar;
void htmlEnvDump(const char* varName);
InString theOriginalCookie; // for comparing with the
// one we generate (don't send if unchanged)
InString cookieAvoidThese;
int avoidSendingCookie(const char* option) const {
if(strstr(cookieAvoidThese.getString(),option)) {
// (guard condition to avoid wasting too much time)
InString t;
t.addCharacter('&');
t.addString(option);
t.addCharacter('&');
return (strstr(cookieAvoidThese.getString(),t.getString())!=0);
} else return 0;
}
#ifdef MULTIPLE_STYLES_SUPPORTED
const char* theStyle;
#endif
};

void addUnicodeURL(InString &url,const CGIEnvironment* c);

#endif
