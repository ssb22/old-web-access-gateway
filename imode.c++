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
