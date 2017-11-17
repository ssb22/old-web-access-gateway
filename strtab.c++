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
#include "strtab.h"
#include "strfunc.h"
#include "debug.h"

StringTable::StringTable() {
theNextLinePtr=theNumLines=0;
}

StringTable::~StringTable() {
checkPoint();
for(int i=0; i<theNextLinePtr; i++) {
delete[] theLHSs[i];
delete[] theRHSs[i];
} if(theNextLinePtr) {
// NOT delete[] !
delete theLHSs;
delete theRHSs;
}
}

void StringTable::addLine(const char* lhs,const char* rhs) {
checkPoint();
char* l=new char[strlen(lhs)+1]; strcpy(l,lhs);
char* r=new char[strlen(rhs)+1]; strcpy(r,rhs);
if(theNextLinePtr==theNumLines) {
int newNumLines=((theNumLines+1)<<1);
char **newLHSs=new char*[newNumLines];
char **newRHSs=new char*[newNumLines];
for(int i=0; i<theNextLinePtr; i++) {
newLHSs[i]=theLHSs[i];
newRHSs[i]=theRHSs[i];
} if(theNextLinePtr) {
delete theLHSs; // NOT delete[] !
delete theRHSs;
}
theLHSs=newLHSs; theRHSs=newRHSs;
theNumLines=newNumLines;
}
theLHSs[theNextLinePtr]=l;
theRHSs[theNextLinePtr]=r;
theNextLinePtr++;
}

const char* StringTable::getValueOf(const char* lhs) const {
checkPoint();
// NB Need to count DOWN (see header)
for(int i=theNextLinePtr-1; i>=0; i--) {
if(!stricmp(theLHSs[i],lhs)) return(theRHSs[i]);
} return(NULL);
}

const char* StringTable::getValueFromTop(const char* lhs) const {
checkPoint();
for(int i=0; i<theNextLinePtr; i++) {
if(!stricmp(theLHSs[i],lhs)) return(theRHSs[i]);
} return(NULL);
}

const char* StringTable::getCaseSensitiveValueOf(const char* lhs) const {
checkPoint();
// NB Need to count DOWN (see header)
for(int i=theNextLinePtr-1; i>=0; i--) {
if(!strcmp(theLHSs[i],lhs)) return(theRHSs[i]);
} return(NULL);
}

void StringTable::setByCaseSensitiveName(const char* lhs,const char* rhs) {
checkPoint();
for(int i=theNextLinePtr-1; i>=0; i--) {
if(!strcmp(theLHSs[i],lhs)) {
delete[] theRHSs[i];
theRHSs[i]=new char[strlen(rhs)+1];
strcpy(theRHSs[i],rhs);
return;
}
} addLine(lhs,rhs); // If not yet found
}

void StringTable::deleteAllByName(const char* lhs) {
checkPoint();
// NEEDATTENTION Duplicate code with the case sensitive version
for(int i=theNextLinePtr-1; i>=0; i--) {
if(!stricmp(theLHSs[i],lhs)) {
delete[] theLHSs[i];
delete[] theRHSs[i];
theNextLinePtr--;
for(int j=i; j<theNextLinePtr; j++) {
theLHSs[j]=theLHSs[j+1];
theRHSs[j]=theRHSs[j+1];
}
}
} 
}

void StringTable::deleteAllByCaseSensitiveName(const char* lhs) {
checkPoint();
// NEEDATTENTION Duplicate code with the case insensitive version
for(int i=theNextLinePtr-1; i>=0; i--) {
if(!strcmp(theLHSs[i],lhs)) {
delete[] theLHSs[i];
delete[] theRHSs[i];
theNextLinePtr--;
for(int j=i; j<theNextLinePtr; j++) {
theLHSs[j]=theLHSs[j+1];
theRHSs[j]=theRHSs[j+1];
}
}
} 
}

void StringTable::deleteByNumber(int i) {
checkPoint();
delete[] theLHSs[i];
delete[] theRHSs[i];
theNextLinePtr--;
for(int j=i; j<theNextLinePtr; j++) {
theLHSs[j]=theLHSs[j+1];
theRHSs[j]=theRHSs[j+1];
}
}

void StringTable::setRhsDontNewIt(int n,char* rhs) {
checkPoint();
delete[] theRHSs[n];
theRHSs[n]=rhs;
}

void StringTable::setRhs(int n,const char* rhs) {
checkPoint();
delete[] theRHSs[n];
theRHSs[n]=new char[strlen(rhs)+1];
strcpy(theRHSs[n],rhs);
}

// Saving and loading stuff
// Straight platform-specific binary should be sufficient
void saveString(const char* s,FILE* fp) {
int l=strlen(s)+1; // include the 0 (makes coding simpler)
fwrite(&l,sizeof(l),1,fp);
fwrite(s,l,1,fp);
}
char* loadString(FILE* fp) {
int l;
fread(&l,sizeof(l),1,fp);
char* s=new char[l];
fread(s,l,1,fp);
return s;
}
void StringTable::saveTo(FILE* fp) const {
checkPoint();
fwrite(&theNextLinePtr,sizeof(theNextLinePtr),1,fp);
for(int i=0; i<theNextLinePtr; i++) {
saveString(theLHSs[i],fp);
saveString(theRHSs[i],fp);
}
}
void StringTable::loadFrom(FILE* fp) {
int numToAdd;
fread(&numToAdd,sizeof(numToAdd),1,fp);
for(int i=0; i<numToAdd; i++) {
char* lhs=loadString(fp);
char* rhs=loadString(fp);
if(!this->getValueOf(lhs)) this->addLine(lhs,rhs);
// (what's already in the table overrides what's in the file)
delete[] rhs; delete[] lhs;
}
}

void StringTable::saveTo(const char* filename) const {
FILE* fp=fopen(filename,"wb");
this->saveTo(fp);
fclose(fp);
}
void StringTable::loadFrom(const char* filename) {
FILE* fp=fopen(filename,"rb");
if(fp) {
this->loadFrom(fp);
fclose(fp);
} // if file not found, starting with new one
}
