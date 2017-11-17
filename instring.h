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
