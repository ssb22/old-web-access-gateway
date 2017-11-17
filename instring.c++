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
#include "instring.h"
#include "platform.h"
#include "debug.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#ifdef MEMCPY_IS_IN_MEM_DOT_H
#include <mem.h>
#endif

#define TEMP_BUFLEN 80 // for addLineFromFile

InString::InString() {
checkPoint();
theMaxLength=theCurrentLength=0;
theData=NULL;
ensureLength(1);
*theData=0;
}

InString::~InString() { if(theData) delete[] theData; }

InString::InString(const InString&x) {
checkPoint();
theMaxLength=theCurrentLength=0;
theData=NULL;
ensureLength(1);
*theData=0;
addString(x.getString());
}

void InString::addCharacter(int c) {
checkPoint();
// To add speed, we'll copy some of the member pointers
// into what would hopefully be registers
// (careful with theData though because ensureLength may change it)
int curlen=theCurrentLength;
int newlen=curlen+1;
if(newlen>=theMaxLength) ensureLength(newLength(theMaxLength));
theData[curlen]=c;
theData[newlen]=0;
theCurrentLength=newlen;
}

void InString::replaceLastCharWith(int c) {
checkPoint();
if(theCurrentLength) theData[theCurrentLength-1]=c;
else throw "Instring::replaceLastCharWith on empty string";
}

void InString::addBytes(const char* bytes,int len) {
checkPoint();
addLength(len);
memcpy(theData+theCurrentLength,bytes,len);
theCurrentLength+=len; theData[theCurrentLength]=0;
}

void InString::prepend(const char* string) {
checkPoint();
int len=strlen(string);
addLength(len);
memmove(theData+len,theData,theCurrentLength+1); // +1 so don't have to say theData[theCurrentLength]=0 at end
memcpy(theData,string,len);
theCurrentLength+=len;
}

void InString::addString(int number) {
checkPoint();
char buf[50]; sprintf(buf,"%d",number);
addString(buf);
}

char* text_fgets(char*buf,size_t len,FILE* f);
int InString::addLineFromFile(FILE* f) {
checkPoint();
if(!f) throw "InString::addLineFromFile called with null parameter";
char temp[TEMP_BUFLEN+1];
// We're going to turn a while into an if followed
// by a do-while, and at the same time put the initial
// check for EOF there
if(!text_fgets(temp,TEMP_BUFLEN,f)) return(EOF);
do {
int i=strlen(temp)-1;
if(i<0) return '\n';
if(temp[i]=='\n') {
// (don't need to deal with '\r' etc here because using text_fgets)
// (NEEDATTENTION fgets is not suitable if the file is likely to have null characters in it)
temp[i]=0;
if(i) addString(temp);
return('\n');
} else addString(temp);
} while(text_fgets(temp,TEMP_BUFLEN,f));
return('\n');
}

void InString::addStringWithCommandPrecautions(const char* url) {
checkPoint();
for(int i=0; url[i]; i++) {
if(url[i]>=' ') {
if(!isalnum(url[i])) addCharacter(COMMAND_SPECIAL_CHAR_PREFIX);
addCharacter(url[i]);
} // Else ignore (someone put a URL with CRs in it, for example - it has been done)
// 2000-07-15: Don't ignore now
// (cookies etc)

// #ifdef DEVELOPMENT_COPY
// else if(url[i]=='\n') addString("$'\\n'");
// #endif

// Trouble is, not all command processor will accept $''
// It was OK in the development copy, but now we have a
// problem.
// NT has "^\n\n"
// Have put in new platform.h (don't need to tell
// existing hosts since none are NT) 2000-07-15
#ifndef COMMAND_CR_STRING
#define COMMAND_CR_STRING "\"\n\""
#endif
else if(url[i]=='\n') addString(COMMAND_CR_STRING);
}
}

void InString::addStringWithQuoteEscape(const char* string) {
checkPoint();
for(int i=0; string[i]; i++) {
if(string[i]=='\\') addString("\\\\");
else if (string[i]=='\"') addString("\\q");
// (This is for cookies in hidden form fields.  We can't directly put the " character, and we can't %-encode it because the browser will %-encode the % character)
else addCharacter(string[i]);
// if(string[i]!='\"') addCharacter(string[i]);
}
}

void InString::ensureLength(long length) {
checkPoint();
char* oldData=theData;
theData=(length<=theCurrentLength)?NULL:(new char[length]);
// (The NULL etc is a patch for overflow)
if(!theData) throw "InString::ensureLength: Probably long integer overflow";
if(oldData) {
memcpy(theData,oldData,theMaxLength);
delete[] oldData;
}
theMaxLength=length;
}

long InString::newLength(long curLength) {
checkPoint();
if(curLength<20) return(20); // Nasty
else return(curLength<<1);
}

void InString::removeIsspace() {
checkPoint();
for(int i=0; i<theCurrentLength; i++)
while(isspace(theData[i]) && theData[i])
memmove(theData+i,theData+i+1,(theCurrentLength--)-i);
}

void InString::replace(int oldChar,int newChar) {
checkPoint();
for(int i=0; i<theCurrentLength; i++)
if(theData[i]==oldChar) theData[i]=newChar;
}

void InString::removeCharAt(int pos) {
checkPoint();
if(theCurrentLength>pos) memmove(theData+pos,theData+pos+1,(theCurrentLength--)-pos);
else throw "InString::removeCharAt out of range";
}

void InString::moveAChunkToTheEnd(int chunkStart,int chunkEnd) {
checkPoint();
int chunkLength=chunkEnd-chunkStart;
if(chunkLength<0) throw "InString::moveAChunkToTheEnd: Silly chunk length";
else if(!chunkLength) return;
if(chunkEnd>theCurrentLength) throw "InString::moveAChunkToTheEnd: Chunk exceeds length of string";
char* tempBuf=new char[chunkLength];
memcpy(tempBuf,theData+chunkStart,chunkLength);
memmove(theData+chunkStart,theData+chunkEnd,theCurrentLength-chunkEnd);
memcpy(theData+theCurrentLength-chunkLength,tempBuf,chunkLength);
delete[] tempBuf;
}

LenType InString::lengthMinusWhitespace() const {
LenType l=length();
while(l && isspace(theData[l-1])) l--;
return l;
}
