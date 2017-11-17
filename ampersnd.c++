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
