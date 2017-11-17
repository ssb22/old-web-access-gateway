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
#ifndef TE_CONTAINER_H
#define TE_CONTAINER_H
#include "platform.h"
#ifdef ENCODINGS_AVAILABLE
#include "tencode.h"
#include "instring.h"
#include "!debug.h"
#define PUSHBACK_STACKLEN 20
char* getStaticUnicodeBuf(unsigned x);
enum TEC_Constants { TEC_InvalidMethod = -5 };
/**
Table encoding container.  Contains and manages a set of encoding tables that are related to a language.
@see TableEncoding
*/
class TEContainer {
#ifdef DEVELOPMENT_COPY
friend void processArguments(int argc,char* argv[]);
#endif
public:
TEContainer(FILE* dataFile);
~TEContainer();
/// Automatically called before/after autoDetect
void resetStates();
int setAutoDetectMimeCharsetIfCharsetIsAppropriateToLanguage(const char* mimeDesignator);
/**
autoDetectFrom resets fp's pointer afterwards
@return >=0 for most likely encoding; -1 = Definately uses ISO or other sequences
*/
int autoDetectFrom(FILE* fp
#ifdef DEBUGGING_SCORES
,CGIEnvironment* theEnvironment
#endif
);
int getMethodByDescription(const char* desc) const;
/// setMethod's parameter is return value of autoDetectFrom
void setMethod(int method);
#ifdef MULTIPLE_STYLES_SUPPORTED
const char* getRecommendedStyle(const char* langName) const; // once method has been set
#endif
/// convertLine returns 1 if reached EOF (but may still fill in dest)
int convertLine(FILE* source,InString &dest);
const char* getDescription(int specialCaseNo) const;
int getNumSpecialCases() const { return(theNumSpecialCases); }
void omitSubEncoding() { theOmitSubEncoding=1; }
int hasGotFreqTable() const { return(theScoreLen); }
private:
void resetShiftStuff();
int theNumberOfEncodings;
TableEncoding* theEncodings;
TableEncoding* makeTE(FILE* dataFile); // See c++ for reasoning behind this
int theScoreLen; unsigned short * theScoreTable;
int getScoreOf(unsigned c) const;
int theMethod;
int ss2EncodingInUse,ss3EncodingInUse,shiftOutRequired,isoNeedsHighBit,isoEncodingInUse;
#define DS_BUFLEN 10
struct DetectionStruct {
char buf[DS_BUFLEN]; int bPtr,inUse,needToClearAtEOL;
int specCaseSecondParam;
long score,blackMarks;
void addChar(int b);
DetectionStruct();
};
void doAutoDetectStuffWith(DetectionStruct* d,int i,int mappedI);
int theNumSpecialCases;
int* theSpecialCaseMap;
int prepareSpecialCase(int i);
void saveSpecialCaseParam(DetectionStruct* d,int i,int mappedI);
void pushBack(int c) { thePushbackStack[thePushbackPtr++]=c; }
char thePushbackStack[PUSHBACK_STACKLEN]; int thePushbackPtr;
int subEncodingFgetc(FILE* fp,int useLtHack=0);
int theOmitSubEncoding,wasAmpersand;
};
#endif
#endif
