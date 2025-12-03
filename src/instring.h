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
#ifndef INSTRING_H
#define INSTRING_H
#include "platform.h"
#include <stdio.h>
#include <string.h>
#ifdef MEMCPY_IS_IN_MEM_DOT_H
#include <mem.h>
#endif
typedef int LenType;
/// A class for building up large and small strings dynamically ('string' is not always implemented, and this gives extra functionality related to Web pages)
class InString {
public:
InString();
~InString();
InString(const InString&x);
const char* getString() const { return(theData?theData:""); }
LenType length() const { return(theCurrentLength); }
LenType lengthMinusWhitespace() const;
void addCharacter(int c);
void replaceLastCharWith(int c);
void addString(const char* string) { addBytes(string,strlen(string)); }
void prepend(const char* string);
void addBytes(const char* bytes,int len);
void addString(int number);
int addLineFromFile(FILE* f); // Will set EOF if reading last
// Returns EOF or \n
void addStringWithCommandPrecautions(const char* url);
void addStringWithQuoteEscape(const char* string);
void clear() { if(theCurrentLength) { theCurrentLength=0; *theData=0; } }
int isEmpty() const { return(!theCurrentLength); }
void truncateAt(LenType position) { theData[position]=0; theCurrentLength=position; }
void removeFirstFewCharacters(int n) { memmove(theData,theData+n,(theCurrentLength-=n)+1); }
void removeIsspace();
void replace(int oldChar,int newChar);
void removeCharAt(int pos);
void moveAChunkToTheEnd(int chunkStart,int chunkEnd);
#ifdef ENCODINGS_AVAILABLE
void eucjToSjis();
#endif
protected:
void addLength(long length) { while(theCurrentLength+length>=theMaxLength) ensureLength(newLength(theMaxLength)); }
void ensureLength(long length);
long newLength(long length);
long theMaxLength,theCurrentLength;
char* theData;
};
#endif
