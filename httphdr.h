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
#ifndef HTTPHDR_H
#define HTTPHDR_H
#include <stdio.h>
#include "strtab.h"
#include "cookie.h"
class CGIEnvironment;
/// A class for parsing an HTTP header
class HttpHeader {
public:
/// Parses the HTTP header from a given file
HttpHeader(FILE* &fp,int isFTP,CGIEnvironment* theEnvironment,const char* alternativeFilenameForDecompression,const char* oldFilename,bool canThrow=false); // Leaves pointer after header
// isFTP==2 for forced interpretation as HTML
// theEnvironment is for extensions
~HttpHeader() {};
/// @return non-0 if the parse was successful
int wasItSuccessful() const { return(theSuccessStatus); }
/**
If more than one value is present, getValueOf
returns the LAST one, so values in the document
itself override those in the header
@return May return NULL
*/
const char* getValueOf(const char* lhs) const { return(theHeaders.getValueOf(lhs)); }
int getNumberOfValues() const { return(theHeaders.getNumberOfValues()); }
const char* getKeyByNumber(int n) const { return(theHeaders.getLhsByNumber(n)); }
const char* getValueByNumber(int n) const { return(theHeaders.getRhsByNumber(n)); }
int handleSetCookies(CookieTable &c);
// (returns 1 if there were any)
private:
void decompress(FILE* &fp,const char* alternativeFilenameForDecompression,const char* oldFilename);
void readMimeHeader(FILE* fp,bool canThrow=false);
int readHttpEquivs(FILE* fp,CGIEnvironment* theEnvironment);
void addHeaderLine(const char* lhs,const char* rhs) { theHeaders.addLine(lhs,rhs); }
int theSuccessStatus;
StringTable theHeaders;
static int useOldCommentStyle;
};
#endif
