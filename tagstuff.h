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
#ifndef TAGSTUFF_H
#define TAGSTUFF_H
#include "platform.h"
#include "cgilib.h"
#include "instring.h"
#include "strtab.h"
#include "strfunc.h"
#include <stdlib.h>

#define MAX_HEADING_LEVEL 6 // Need to re-implement some things if >9
#define DEFAULT_HEADING_PROMOTION "3"
#define DFLT_HEADING_PROMOTION 3
#define MAX_FONT_SIZE 7

/**
Resolves relativeHTML in the context of currentPage, and returns the result.
Need to delete[] the return value of sortDirectories
*/
char* sortDirectories(const char* relativeHTML,const char* currentPage);

/// @return non-0 if url is not likely to be an HTML document
int notAnHtmlDocument(const char* url,const CGIEnvironment* e);

/// A class for representing an HTML tag
class HTMLTag {
public:
HTMLTag(FILE* in,InString &htmlLineInput,int &lineInputPtr) { construct(1,in,htmlLineInput,lineInputPtr); }
HTMLTag(FILE* in);
HTMLTag(const char* name) { theCommentStatus=0; setNameTo(name); }
~HTMLTag() {};
const char* getName() const { return(theTagName.getString()); }
const char* getAttribute(const char* attribute) const { return(theAttributes.getValueOf(attribute)); }
const char* getAttributeFromLeft(const char* attribute) const { return(theAttributes.getValueFromTop(attribute)); }
const char* getAttributeName(int n) const { return (theAttributes.getLhsByNumber(n)); }
const char* getAttributeValue(int n) const { return (theAttributes.getRhsByNumber(n)); }
int getNumAttributes() const { return theAttributes.getNumberOfValues(); }
void addAttribute(const char* lhs,const char* rhs) { theAttributes.addLine(lhs,rhs); }
void setNameTo(const char* newName);
void removeJavascript();
void deleteAllByName(const char* name) { theAttributes.deleteAllByName(name); }
int wasOnlyAComment() const { return(theCommentStatus); }
void outputWithModifiedURLs(const char* currentPage,CGIEnvironment* theEnvironment,int canUseJavascript=1,int usePdfHack=0);
void outputWithModifiedContent(const char* currentPage,CGIEnvironment* theEnvironment);
void debugDump(CGIEnvironment* e) const;
void outputTo(CGIEnvironment* e) const;
void outputTo(FILE* out) const; // for the XHTML stuff
void outputTo(CGIEnvironment* e,const char* currentPage) { makeURLsAbsolute(currentPage); outputTo(e); }
char* getStrippedHrefForAlt(); // Need to delete[] it
protected:
void construct(int usingSubHtml,FILE* in,InString &htmlLineInput,int &lineInputPtr);
void makeURLsAbsolute(const char* currentPage);
int outputIfNotReallyATag(CGIEnvironment* e) const;
InString theTagName;
StringTable theAttributes;
int theCommentStatus;
static int useOldCommentStyle;
};

void adobePDFHackMessage(CGIEnvironment* e);

#endif
