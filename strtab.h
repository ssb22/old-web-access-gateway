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
#ifndef STRTAB_H
#define STRTAB_H
#include <stdio.h>
/**
A class for storing a key/value table of strings.
Designed for small numbers of key/value pairs
and must guarantee to store things in the
given order.
*/
class StringTable {
public:
// No point in special hashing functions here
StringTable();
~StringTable();
void addLine(const char* lhs,const char* rhs);
const char* getValueOf(const char* lhs) const;
const char* getValueFromTop(const char* lhs) const;
const char* getCaseSensitiveValueOf(const char* lhs) const;
const char* getLhsByNumber(int n) const { return(theLHSs[n]); }
const char* getRhsByNumber(int n) const { return(theRHSs[n]); }
int getNumberOfValues() const { return(theNextLinePtr); }
void deleteAllByName(const char* lhs);
void deleteAllByCaseSensitiveName(const char* lhs);
void setByCaseSensitiveName(const char* lhs,const char* rhs); // Used by CGILib
void deleteByNumber(int i);
void setRhsDontNewIt(int n,char* rhs); // Used by tag / make URLs absolute
void setRhs(int n,const char* rhs); // again
void saveTo(FILE* fp) const;
void loadFrom(FILE* fp);
void saveTo(const char* filename) const;
void loadFrom(const char* filename);
protected:
char **theLHSs,**theRHSs;
int theNextLinePtr,theNumLines;
};
#endif
