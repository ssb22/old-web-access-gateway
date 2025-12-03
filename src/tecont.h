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
