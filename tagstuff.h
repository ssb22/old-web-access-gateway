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
