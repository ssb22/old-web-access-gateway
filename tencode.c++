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
#include "tencode.h"
#include "utf.h"
#include "imode.h"
#include "instring.h"
#include "literals.h"
#include "strfunc.h"
#include "!debug.h"
#include "debug.h"

void TableEncoding::load(FILE* dataFile) {
checkPoint();
// Horrible hack: If we're at EOF, then drop into UTF-8
// (Saves all the indirection that would be necessary
// with inheritence - this class is called quite a lot)
if(fread(theName,TE_NAMELEN+1,1,dataFile)) {
// We read something
#ifdef DEBUGGING_TABLES
printf("Debugger: theName=%s, this=0x%p<BR>\n",theName,this); // %s OK (debugging)
#endif
theISODesignator=fgetc(dataFile);
#ifdef DEBUGGING_TABLES
printf("Debugger: theISODesignator=%d (%c)<BR>\n",theISODesignator,theISODesignator);
#endif
setSpecialCaseFromISODesignator();
resetSpecialCaseParams();
theMaxBytesPerChar=fgetc(dataFile);
#ifdef DEBUGGING_TABLES
printf("Debugger: theMaxBytesPerChar=%d<BR>\n",theMaxBytesPerChar);
#endif
switch(theMaxBytesPerChar) {
case 1:
#ifdef COMPILER_USES_LSB_MSB_INTS
//#if (sizeof(short)!=2)
//#error sizeof(short) should be 2 if using COMPILER_USES_LSB_MSB_INTS
//#endif
fread(theMainTable.oneByte,sizeof(short),TE_MAIN_TABLEN,dataFile);
#else
for(int i=0; i<TE_MAIN_TABLEN; i++) {
int lsb=fgetc(dataFile);
int msb=fgetc(dataFile);
theMainTable.oneByte[i]=((msb<<8)|lsb);
#ifdef DEBUGGING_TABLES
printf("Debugger: Native %d maps to %02X %02X<BR>\n",i,msb,lsb);
#endif
}
#endif
break;
case 2: {
theMainTable.twoBytes.mapsAreOnly128Bytes=theMainTable.twoBytes.MAPS256;
theMainTable.twoBytes.single7BitValidButNoMap=0;
// NB the &3 here!
switch((theMainTable.twoBytes.validSecondary[0]=fgetc(dataFile))&3) {
case 0: case 1:
fread(theMainTable.twoBytes.validSecondary+1,sizeof(char),TE_MAIN_TABLEN-1,dataFile);
break;
case 3:
theMainTable.twoBytes.mapsAreOnly128Bytes=theMainTable.twoBytes.MAPS128;
// DON'T break;
case 2:
// Only 128 bytes in main table
theMainTable.twoBytes.single7BitValidButNoMap=1;
fread(theMainTable.twoBytes.validSecondary+128,sizeof(char),TE_HALF_MAIN_TABLEN,dataFile);
break;
default: throw "TableEncoding: Unsupported two-byte mode";
}
theMainTable.twoBytes.ebcdic=(theMainTable.twoBytes.validSecondary[0]&4);
unsigned totalValid=(fgetc(dataFile)<<(theMainTable.twoBytes.mapsAreOnly128Bytes?7:8));
#ifdef DEBUGGING_TABLES
printf("Debugger: totalValid=%d ebcdic=%d mode=%d mapsAreOnly128Bytes=%d single7BitValidButNoMap=%d<BR>\n",totalValid,theMainTable.twoBytes.ebcdic,theMainTable.twoBytes.validSecondary[0]&3,theMainTable.twoBytes.mapsAreOnly128Bytes,theMainTable.twoBytes.single7BitValidButNoMap);
#endif
theMainTable.twoBytes.secondaryMemory=new unsigned short[totalValid];
#ifdef COMPILER_USES_LSB_MSB_INTS
fread(theMainTable.twoBytes.secondaryMemory,sizeof(unsigned short),totalValid,dataFile);
#else
for(unsigned i=0; i<totalValid; i++) {
int lsb=fgetc(dataFile);
int msb=fgetc(dataFile);
theMainTable.twoBytes.secondaryMemory[i]=((msb<<8)|lsb);
}
#endif
break; }
case 4: {
theMainTable.fourBytes.extensionByte=fgetc(dataFile);
theMainTable.fourBytes.firstPlaneSpecifier=fgetc(dataFile);
theMainTable.fourBytes.numSupportedPlanes=fgetc(dataFile);
theMainTable.fourBytes.minFirstByte=fgetc(dataFile);
theMainTable.fourBytes.maxFirstByte=fgetc(dataFile);
theMainTable.fourBytes.minSecondByte=fgetc(dataFile);
theMainTable.fourBytes.maxSecondByte=fgetc(dataFile);
theMainTable.fourBytes.firstByteSize=theMainTable.fourBytes.maxFirstByte-theMainTable.fourBytes.minFirstByte+1;
theMainTable.fourBytes.totalPlaneSize=theMainTable.fourBytes.firstByteSize*(theMainTable.fourBytes.maxSecondByte-theMainTable.fourBytes.minSecondByte+1);
unsigned totalValid=theMainTable.fourBytes.totalPlaneSize*theMainTable.fourBytes.numSupportedPlanes;
theMainTable.fourBytes.data=new unsigned short[totalValid];
#ifdef COMPILER_USES_LSB_MSB_INTS
fread(theMainTable.fourBytes.data,sizeof(unsigned short),totalValid,dataFile);
#else
for(unsigned i=0; i<totalValid; i++) {
int lsb=fgetc(dataFile);
int msb=fgetc(dataFile);
theMainTable.fourBytes.data[i]=((msb<<8)|lsb);
}
#endif
break; }
default: throw "TableEncoding: Unsupported theMaxBytesPerChar";
}
} else {
// UTF-8
strcpy(theName,"UTF-8");
theISODesignator=0;
theSpecialCaseStatus=SC_NONE;
resetSpecialCaseParams();
theMaxBytesPerChar=6;
}
}

TableEncoding::~TableEncoding() {
if(hackyEucTable==this) hackyEucTable=NULL;
if(theMaxBytesPerChar==2) delete[] theMainTable.twoBytes.secondaryMemory;
else if(theMaxBytesPerChar==4) delete[] theMainTable.fourBytes.data;
}

void TableEncoding::resetSpecialCaseParams() {
theSpecialCaseParameter=SCP_NONE; theSecondParameterIsSet=0;
}

unsigned TableEncoding::lastConvertedCharacter;
char TableEncoding::isAtStartOfLine=0;

int TableEncoding::checkISODesignator(int c) const {
checkPoint();
if(theMaxBytesPerChar==4) {
if(c>=theISODesignator && c<theISODesignator+theMainTable.fourBytes.numSupportedPlanes) {
// Set lastConvertedCharacter to plane prefix
lastConvertedCharacter=((theMainTable.fourBytes.extensionByte<<8)|(theMainTable.fourBytes.firstPlaneSpecifier+c-theISODesignator));
return(1);
} else return(0);
} else { lastConvertedCharacter=0; return(c==theISODesignator || (theISODesignator && c=='@')); }
// This last condition, (theISODesignator && c=='@'),
// was added 2000-08-23 as a hack for some Japanese
// documents; I'm guessing that Esc $ @ means "programmer
// can't remember the ISO designator for this language".
// It seems to work, but this needs sorting out.
// NB the correct ISO designator for Japanese is 'B'
// (AFAIK)
}

ConversionResult TableEncoding::handleISOStuff(const char* data) const {
checkPoint();
// NB Returns CR_FAILURE not CR_UTTER_FAILURE
// because might switch ISO later
// (if another instance claims it, this
// overrides everything)
lastConvertedCharacter=0; // Or could set to the plane prefix
switch(data[1]) {
case 0: return(CR_NEED_MORE_BYTES);
case '$':// two-byte
if(theMaxBytesPerChar==1) return(CR_POSS_ISO_BUT_NOT_MINE);
if(!data[2]) return(CR_NEED_MORE_BYTES);
if(theISODesignator<ISO_HISTORIC_WATERMARK) {
if(checkISODesignator(data[2])) return(CR_WAS_MY_IMMED_ISO_SEQUENCE);
else return(CR_POSS_ISO_BUT_NOT_MINE);
} else if(data[2]>=ISO_MIN_SO && data[2]<=ISO_MAX_SO) {
if(data[3]==0) return(CR_NEED_MORE_BYTES);
else if(checkISODesignator(data[3])) {
switch(data[2]) {
case ISO_MIN_SO: return(CR_WAS_MY_IMMED_ISO_SEQUENCE);
case ISO_MIN_SO+1: return(CR_WAS_MY_SO_ISO_SEQUENCE);
case ISO_MIN_SO+2: return(CR_WAS_MY_SS2_ISO_SEQUENCE);
case ISO_MIN_SO+3: return(CR_WAS_MY_SS3_ISO_SEQUENCE);
default: throw "TableEncoding::tryAndConvert ISO code needs updating: Too many single-shift sequences have been added";
}
} else return(CR_POSS_ISO_BUT_NOT_MINE);
} else return(CR_POSS_ISO_BUT_NOT_MINE);
case '(':// one-byte
if(!data[2]) return(CR_NEED_MORE_BYTES);
if(data[2]=='B') return(CR_WAS_ISO_PURE_ASCII);
else if(theMaxBytesPerChar==1) {
if(data[2]==theISODesignator) return(CR_WAS_MY_IMMED_ISO_SEQUENCE);
else return(CR_POSS_ISO_BUT_NOT_MINE);
} else {
if(theSpecialCaseStatus) return(handleSpecialCaseISOOneByte(data[2]));
else return(CR_POSS_ISO_BUT_NOT_MINE);
}
case 'N': return(CR_ISO_SS2);
case 'O': return(CR_ISO_SS3);
case 14: return(CR_ISO_SO);
case 15: return(CR_ISO_SI);
default: return(CR_FAILURE);
}
}

void TableEncoding::setSpecialCaseFromISODesignator() {
if(theISODesignator=='A') theSpecialCaseStatus=SC_CHINESE_GB;
else if(theISODesignator=='B') {
theSpecialCaseStatus=SC_JAPANESE_EUCJIS;
hackyEucTable=this;
}
else theSpecialCaseStatus=SC_NONE;
}

int TableEncoding::setSpecialCaseFromCharsetIfAppropriateToLanguage(const char* charset) {
if(!stricmp(charset,theName)) return SC_NONE;
// (This will work for UTF-8, but not for others, which have aliases etc NEEDATTENTION) (and what about ISO stuff?)
return -1;
}

int TableEncoding::getNoOfSpecialCaseInstances() const {
checkPoint();
switch(theSpecialCaseStatus) {
case SC_CHINESE_GB: return(3);
case SC_JAPANESE_EUCJIS: return(2);
case SC_NONE: ;
} return(1);
}

void TableEncoding::setSpecialCaseInstanceNumber(int n) {
checkPoint();
if(!n) { setSpecialCaseParameter(SCP_NONE); return; }
switch(theSpecialCaseStatus) {
case SC_CHINESE_GB:
if(n==1) setSpecialCaseParameter(SCP_HZ);
else setSpecialCaseParameter(SCP_ZW);
return;
case SC_JAPANESE_EUCJIS:
if(n==1) { setSpecialCaseParameter(SCP_SHIFT_JIS); return; }
break;
case SC_NONE: ; // Do nothing
} throw "TableEncoding::setSpecialCaseInstanceNumber got invalid parameter";
}

const char* TableEncoding::getDescription(int specialCaseInstanceNumber) const {
if(specialCaseInstanceNumber) switch(theSpecialCaseStatus) {
case SC_CHINESE_GB:
if(specialCaseInstanceNumber==1) return(L_SPECIAL_CASE_HZ);
else return(L_SPECIAL_CASE_ZW);
break;
case SC_JAPANESE_EUCJIS: return(L_SPECIAL_CASE_SJIS);
case SC_NONE: ; // Nothing
} return(theName);
}

#ifdef MULTIPLE_STYLES_SUPPORTED
const char* TableEncoding::getRecommendedStyle() const {
if(theSpecialCaseStatus==SC_CHINESE_GB) return "s";
else return NULL; // don't know at this level
// (later looks at language level - Chinese="t" (all other
// Chinese encodings apart from the above are traditional
// (?)), korean="k", etc
}
#endif

ConversionResult TableEncoding::handleSpecialCaseISOOneByte(char c) const {
checkPoint();
lastConvertedCharacter=0; // special case parameter
switch(theSpecialCaseStatus) {
case SC_JAPANESE_EUCJIS:
if(c=='I') {
lastConvertedCharacter=SCP_HALF_WIDTH_KATAKANA;
return(CR_ONE_BYTE_SPECIAL_CASE);
} else if(c=='J') return(CR_ONE_BYTE_SPECIAL_CASE);
else return(CR_POSS_ISO_BUT_NOT_MINE);
case SC_CHINESE_GB:
if(c=='T') return(CR_ONE_BYTE_SPECIAL_CASE);
// else don't break;
case SC_NONE: ;
} return(CR_POSS_ISO_BUT_NOT_MINE);
}

ConversionResult TableEncoding::specialCaseConversionHook(const char* data) const {
checkPoint();
switch(theSpecialCaseStatus) {
case SC_JAPANESE_EUCJIS:
if(data[0]&0x80) {
if(theSpecialCaseParameter==SCP_SHIFT_JIS) {
unsigned char c1=(unsigned char)(data[0]); if(data[1]==0) {
// Little patch for Macintosh SJIS
if(c1==0x80) lastConvertedCharacter='\\';
else if(c1==0xFD) lastConvertedCharacter=0x87; // Copyright
else if(c1==0xFE) lastConvertedCharacter=0x99; // TM
else if(c1==0xFF) lastConvertedCharacter=0x85; // ...
else return(CR_NO_RESULT_YET);
return(CR_SUCCESS);
} else { // Two-byte stuff
unsigned char c2=(unsigned char)(data[1]);
// Here we go, reverse engineering...
int c1Mod2=0,jisc1,jisc2;
if(c2>=0x9F && c2<=0xFC) jisc2=c2-126;
else if(c2&0x80) { jisc2=c2-32; c1Mod2=1; }
else if(c2&0x40) { jisc2=c2-31; c1Mod2=1; }
else return(CR_UTTER_FAILURE);
if(c1>=0xE0 && c1<=0xEF) jisc1=((c1-0xB0)<<1)-c1Mod2;
else if(c1>=0xF0 && c1<=0xFC) {
// This is the user-defined SJIS area
// Return failure, but not utter failure
return(CR_FAILURE);
} else if(c1>=0x81 && c1<=0x9F) jisc1=((c1-0x70)<<1)-c1Mod2;
else return(CR_UTTER_FAILURE);
char theWorkspace[3]; theWorkspace[2]=0;
theWorkspace[0]=(jisc1|0x80); theWorkspace[1]=(jisc2|0x80);
return(rawTryAndConvert(theWorkspace));
}
} // End of Shift-JIS code
else if(data[0]==(char)0x8F) {
if(!(data[1]&&data[2])) return(CR_NEED_MORE_BYTES);
// We have a problem: It's JIS X 0212-1990
// and we only know JIS X 0208
// NEEDATTENTION
return(CR_FAILURE);
}
} else { // data[0]<0x80
if(theSpecialCaseParameter==SCP_HALF_WIDTH_KATAKANA) {
// NEEDATTENTION
// WARNING!  This code is total guesswork!
char d2[3]; d2[0]=0x8E; d2[1]=(data[0]&0x80); d2[2]=0;
return(rawTryAndConvert(d2));
} else if(data[0]==92) {
// Patch for Japanese Yen
lastConvertedCharacter=165; return(CR_SUCCESS);
}
} break;
case SC_CHINESE_GB:
if(data[0]=='$' && !theSecondParameterIsSet) {
// Patch for Chinese Yuan
return(rawTryAndConvert("\xD4\xB2")); // or \xD4\xAA
} else if(theSpecialCaseParameter==SCP_HZ) {
if(data[0]=='~') switch(data[1]) {
case 0: return(CR_NEED_MORE_BYTES);
case '\n': return(CR_DITCH_THIS_CHARACTER);
case '~': lastConvertedCharacter='~'; return(CR_SUCCESS);
case '{': return(CR_SET_MY_SECOND_PARAMETER);
case '}': return(CR_CLEAR_MY_SECOND_PARAMETER);
default: return(CR_UTTER_FAILURE); // Invalid Hz escape sequence
}
} else if(theSpecialCaseParameter==SCP_ZW) {
if(isAtStartOfLine && data[0]=='z') {
if(data[1]==0) return(CR_NEED_MORE_BYTES);
else if(data[1]=='W') return(CR_SET_SECOND_PARAMETER_TO_EOL);
else {
lastConvertedCharacter='z';
return(CR_SUCCESS_WITH_ONE_LESS_BYTE);
}
}
} else return(CR_NO_RESULT_YET);
// If get here, is either Hz or zW
if(theSecondParameterIsSet) {
if(data[1]==0) return(CR_NEED_MORE_BYTES);
if((data[0]|data[1])&0x80) return(CR_UTTER_FAILURE);
char d2[3]; d2[0]=(data[0]&0x80); d2[1]=(data[1]&0x80); d2[2]=0;
return(rawTryAndConvert(d2));
} break;
case SC_NONE: ;
// default is to just run on
} return(CR_NO_RESULT_YET);
}

ConversionResult TableEncoding::tryAndConvert(const char* data) const {
if(data[0]==ESCAPE && theISODesignator) return(handleISOStuff(data));
else if(theSpecialCaseStatus) {
ConversionResult r=specialCaseConversionHook(data);
if(r!=CR_NO_RESULT_YET) return(r);
} return(rawTryAndConvert(data));
}

ConversionResult TableEncoding::rawTryAndConvert(const char* data) const {
checkPoint();
switch(theMaxBytesPerChar) {
case 1:
return(((lastConvertedCharacter=theMainTable.oneByte[(unsigned char)(data[0])])==0xFFFF)?CR_FAILURE:CR_SUCCESS);
case 2: {
int byte1=((unsigned char*)data)[0];
if(theMainTable.twoBytes.single7BitValidButNoMap && !(byte1&0x80)) {
lastConvertedCharacter=byte1;
return(CR_SUCCESS);
}
int byte2=((unsigned char*)data)[1];
if(!byte2) return(CR_NEED_MORE_BYTES);
byte1=theMainTable.twoBytes.validSecondary[byte1];
if(byte1) {
if(theMainTable.twoBytes.mapsAreOnly128Bytes && !(byte2&0x80))
return(CR_UTTER_FAILURE);
else return(((lastConvertedCharacter=theMainTable.twoBytes.secondaryMemory[((byte1-1)<<(theMainTable.twoBytes.mapsAreOnly128Bytes?7:8))|(byte2-theMainTable.twoBytes.mapsAreOnly128Bytes)])==0xFFFF)?CR_FAILURE:CR_SUCCESS);
} else return(CR_FAILURE); }
case 4: {
int byte1=((unsigned char*)data)[0];
if(byte1&0x80) {
int plane=0,byte2;
if(byte1==theMainTable.fourBytes.extensionByte) {
if(!(data[0] && data[1] && data[2] && data[3])) return(CR_NEED_MORE_BYTES);
plane=((unsigned char*)data)[1]-theMainTable.fourBytes.firstPlaneSpecifier;
if(plane<0) return(CR_UTTER_FAILURE);
if(plane>theMainTable.fourBytes.numSupportedPlanes) return(CR_FAILURE);
byte1=((unsigned char*)data)[2];
byte2=((unsigned char*)data)[3];
} else {
byte2=((unsigned char*)data)[1];
if(!byte2) return(CR_NEED_MORE_BYTES);
}
if(byte1<theMainTable.fourBytes.minFirstByte || byte1>theMainTable.fourBytes.maxFirstByte
|| byte2<theMainTable.fourBytes.minSecondByte || byte2>theMainTable.fourBytes.maxSecondByte) return(CR_UTTER_FAILURE);
else return(((lastConvertedCharacter=theMainTable.fourBytes.data[plane*theMainTable.fourBytes.totalPlaneSize+(byte1-theMainTable.fourBytes.minFirstByte)*theMainTable.fourBytes.firstByteSize+byte2-theMainTable.fourBytes.minSecondByte])==0xFFFF)?CR_FAILURE:CR_SUCCESS);
} else {
lastConvertedCharacter=byte1;
return(CR_SUCCESS);
} }
case 6:
return utf8Convert(lastConvertedCharacter,(const unsigned char*)data);
default:
#ifdef DEBUGGING_TABLES
printf("Debugger: theMaxBytesPerChar=%d, this=0x%p\n",theMaxBytesPerChar,this);
#endif
throw "TableEncoding::rawTryAndConvert: Unsupported theMaxBytesPerChar";
}
}

ConversionResult TableEncoding::tryConvertAndSetState(const char* data) {
checkPoint();
ConversionResult r=tryAndConvert(data);
switch(r) {
case CR_ONE_BYTE_SPECIAL_CASE: setSpecialCaseParameter((SpecialCaseParameter)lastConvertedCharacter); return(CR_DITCH_THIS_CHARACTER);
case CR_SET_MY_SECOND_PARAMETER: setSpecialCaseSecondParameter(); return(CR_DITCH_THIS_CHARACTER);
case CR_CLEAR_MY_SECOND_PARAMETER: clearSpecialCaseSecondParameter(); return(CR_DITCH_THIS_CHARACTER);
default: return(r);
}
}
#endif
