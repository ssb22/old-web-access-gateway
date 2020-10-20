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
#include "allinc.h"
#include "platform.h"
//#ifdef ENCODINGS_AVAILABLE
#include "ampersnd.h"
//#include "tecont.h"
#include "debug.h"
#include <ctype.h>

// This is a special #include; it should be done only once:
#include "_symbols.h"

static char* getStaticUnicodeBuf(unsigned x) {
// from tecont.c++ but cut-down
static char buf[10];
if(!(x&(~0x7F))) { buf[0]=x; buf[1]=0; }
else sprintf(buf,"&#%d;",x);
return(buf);
}

#include <stdio.h> // **********************
unsigned lookupAmpersand(const char* text,int includeExtra=0) {
checkPoint();
// Might as well just do a dictionary-like scan; it's only
// a small table
if(includeExtra) {
// For XML, need to substitute &nbsp; and &copy; also
// (NB this must be before, not after, the main binary
// search, because the main binary search might return
// 0)
if(!strncmp(text,"nbsp;",5)) return 160;
if(!strncmp(text,"copy;",5)) return 169;
}
int minPtr=0,maxPtr=NumAmpersandSymbols-1,offset=0,tmp;
while(*text && *text!=';') {
while((tmp=ampersandSymbols[minPtr][offset])<*text) {
minPtr++; if(minPtr>maxPtr) return 0;
}
if(tmp>*text) return 0;
// ampersandSymbols[minPtr][offset]==*text, and minPtr
// points to the first ampersandSymbols with this
// property.
while(ampersandSymbols[maxPtr][offset]>*text) maxPtr--;
// maxPtr now points to the last with this property.
// The two define the subset of table entries in which
// further letters can be searched for.
if(ampersandSymbols[minPtr][offset+1]==0
&& text[1]==';') {
// We have a match
return ampersandUnicode[minPtr];
}
text++; offset++;
}
// (note that this is not the only place where it can
// return 0)
return 0;
}

int isAmpCode(const char* text) {
// (only roughly, NEEDATTENTION)
if(*text=='#') {
text++;
while(*text && isalnum(*text)) text++;
return(*text==';');
} else return 0;
}

void ampersandFilter(const char* src,InString &dest,int outXml) {
checkPoint();
// NEEDATTENTION Is this OK even in tags, comments etc?
// (Don't want to write more than one full HTML parser in the same program...)
unsigned uVal;
for(int i=0; src[i]; i++) {
if(src[i]=='&' &&
(uVal=lookupAmpersand(src+i+1,outXml))!=0) {
while(src[i]!=';') i++;
dest.addString(getStaticUnicodeBuf(uVal));
} else if(src[i]=='&' && outXml && !isAmpCode(src+i+1)) {
// a lone '&' - change it to &amp; for XML
dest.addString("&amp;");
} else dest.addCharacter(src[i]);
}
}
//#endif
