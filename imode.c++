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
#ifdef ENCODINGS_AVAILABLE
#include "imode.h"
#include "tecont.h"
#include "instring.h"
#include "debug.h"
#include <string.h>

// This is some hacky code to do stuff with Shift-JIS
// output.  It might be better to precompute the inverse
// mapping table and load it from a file.

TableEncoding* hackyEucTable=NULL;

#define SECONDARY_MAX_PAGES 93
#define SECONDARY_SIZE (SECONDARY_MAX_PAGES<<8)

static struct ReverseJISTable {
int validSecondary[0x100]; // points into secondary
unsigned secondary[SECONDARY_SIZE];
int nextSecPointer;
}* revTable=NULL;

inline unsigned jisLookup(unsigned unicode) {
int i=revTable->validSecondary[unicode>>8];
if(i)
return revTable->secondary[((i-1)<<8)|(unicode&0xFF)];
else return 0;
}

inline void addUnicodePoint(unsigned in,unsigned out) {
checkPoint();
int i=revTable->validSecondary[in>>8];
// First write: addUnicodePoint, unicode=ff61 out=8ea1
// 8ea1 = 36513 = i of x611b
if(!i) {
i=revTable->validSecondary[in>>8]=
++(revTable->nextSecPointer); // ++ as prefix (not suffix), because 0 is not valid
if(revTable->nextSecPointer>SECONDARY_MAX_PAGES) throw "addUnicodePoint: SECONDARY_MAX_PAGES is too small";
}
revTable->secondary[((i-1)<<8)|(in&0xFF)]=out;
}

void makeJISTable() {
checkPoint();
// If the Japanese table hasn't already been loaded,
// load it
if(!hackyEucTable) {
InString s;
#ifdef TBL_SUBDIR
s.addString(TBL_SUBDIR);
#endif
s.addString("Japanese.tbl");
FILE* eTbl=fopen(s.getString(),"rb");
if(!eTbl) throw "makeJISTable: Could not open Japanese.tbl";
new TEContainer(eTbl); // NEEDATTENTION Dangling new
fclose(eTbl);
if(!hackyEucTable) throw "makeJISTable: Failed to load EUC-JP table";
}
// hackyEucTable now points to a TableEncoding containing
// an EUC-JP to Unicode mapping table
// We want to do the reverse
revTable=new ReverseJISTable;
memset(revTable,0,sizeof(*revTable));
for(int b1=(hackyEucTable->theMainTable.twoBytes.single7BitValidButNoMap?0x80:0); b1<0x100; b1++) {
int base=hackyEucTable->theMainTable.twoBytes.validSecondary[b1];
if(base) {
base=((base-1)<<(hackyEucTable->theMainTable.twoBytes.mapsAreOnly128Bytes?7:8));
for(int b2=hackyEucTable->theMainTable.twoBytes.mapsAreOnly128Bytes; b2<0x100; b2++) {
int u=hackyEucTable->theMainTable.twoBytes.secondaryMemory[base|(b2-hackyEucTable->theMainTable.twoBytes.mapsAreOnly128Bytes)];
if(u!=0xFFFF) addUnicodePoint(u,(b1<<8)|b2);
}
}
}

// NEEDATTENTION Need to do the Yen patch.
// Patch for Japanese Yen: Unicode 165 maps to 92
// (Shouldn't need to do this as it's probably duplicated
// elsewhere in Shift-JIS)
// But what does a Unicode 92 (\) map to?

}

char* getStaticEUCJBufOrNull(unsigned x) {
checkPoint();
if(!revTable) makeJISTable();
unsigned r=jisLookup(x);
if(r) {
static char buf[3]="\x0\x0"; // so 3rd byte is NULL
buf[0]=(r>>8); buf[1]=(r&0xFF);
return buf;
} else return NULL;
}

unsigned eucToSjis(unsigned euc) {
// Don't need much error checking here
euc &= 0x7F7F; // make it JIS
int j1=(euc>>8), j2=(euc&0xFF); // the JIS input
// s1, s2: the two bytes of Shift-JIS output
// Perhaps the compiler will optimise better if the
// expressions are completely inlined
#define s1 (((j1 + 1) >> 1) + ((j1>=95)?0xB0:0x70))
#define s2 (j2 + ((j1 & 1) ? \
(j2>=96 ? 32 : 31) \
: 126))
return ((s1<<8)|s2);
#undef s1
#undef s2
}

void InString::eucjToSjis() {
if(theData) {
unsigned char c1,c2;
for(LenType i=0; theData[i]; i++) {
if(
((c1=((unsigned char)(theData[i])))&0x80) &&
((c2=((unsigned char)(theData[i+1])))&0x80)) {
unsigned sjis=eucToSjis((((int)c1)<<8)|c2);
theData[i++]=(sjis>>8); // ++ so i advanced twice
theData[i]=(sjis & 0xFF);
}
}
}
}

#endif
