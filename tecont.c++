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
#include "tecont.h"
#include "literals.h"
#include "!debug.h"
#include "images.h"
#include "utf.h"
#include "imode.h"
#include "debug.h"
#include "cgilib.h"
#include <ctype.h>
#include <string.h>

int text_fgetc(FILE* stream);

// NOTE: The value passed to this function may be
// a normal ASCII character and/or part of HTML,
// so don't just convert everything!
char* getStaticUnicodeBuf(unsigned x) {
if(subHtmlShowCharsAs==CharsUtf) return getStaticUtf8Buf(x);
else if(subHtmlShowCharsAs==CharsSJIS) {
char* c=getStaticEUCJBufOrNull(x); // converted to Shift-JIS later (so that the HTML parsing routines don't stumble over the '<' characters that sometimes crop up in Shift-JIS)
if(c) return c;
}

// Decimal is always <= the number of bytes
// that it takes with hex, if 'x' is added
// to the latter
// (within this range anyway)
// (becomes better to use hex at 100 000)
static char buf[10];
if(!(x&(~0x7F))) { buf[0]=x; buf[1]=0; }
else sprintf(buf,"&#%d;",x);
return(buf);
}

TableEncoding* TEContainer::makeTE(FILE* dataFile) {
checkPoint();
TableEncoding* a=new TableEncoding[theNumberOfEncodings];
for(int i=0; i<theNumberOfEncodings; i++) a[i].load(dataFile);
return a;
}

TEContainer::TEContainer(FILE* dataFile) {
checkPoint();
theMethod=thePushbackPtr=theOmitSubEncoding=0; ss2EncodingInUse=ss3EncodingInUse=isoEncodingInUse=-1;
shiftOutRequired=0; isoNeedsHighBit=1;
if(fgetc(dataFile)!=0) { // There is a frequency table
theScoreLen=fgetc(dataFile);       // LSB
theScoreLen|=(fgetc(dataFile)<<8); // MSB
theScoreTable=new unsigned short[theScoreLen];
#ifdef COMPILER_USES_LSB_MSB_INTS
//#if (sizeof(short)!=2)
//#error sizeof(short) should be 2 if using COMPILER_USES_LSB_MSB_INTS
//#endif
fread(theScoreTable,sizeof(unsigned short),theScoreLen,dataFile);
#else
for(int i=0; i<theScoreLen; i++) {
int lsb=fgetc(dataFile);
int msb=fgetc(dataFile);
theScoreTable[i]=((msb<<8)|lsb);
}
#endif
} else theScoreLen=0;
// Hack: Adding 1 to theNumberOfEncodings, so the last
// one can do UTF-8
theNumberOfEncodings=fgetc(dataFile)+1;
theEncodings=makeTE(dataFile);
if(strcmp(theEncodings[theNumberOfEncodings-1].getDescription(0),"UTF-8")) throw "TEContainer: Last encoding is not UTF-8; wrong length marker in table?";
#ifdef DEBUGGING_TABLES
puts("Debugger: Finished creating encodings array<BR>");
#endif
theNumSpecialCases=0;
for(int i=0; i<theNumberOfEncodings; i++) theNumSpecialCases+=theEncodings[i].getNoOfSpecialCaseInstances();
#ifdef DEBUGGING_TABLES
printf("Debugger: theNumSpecialCases=%d theNumberOfEncodings=%d<BR>\n",theNumSpecialCases,theNumberOfEncodings);
#endif
if(theNumSpecialCases>theNumberOfEncodings) {
theSpecialCaseMap=new int[theNumSpecialCases];
int j=0; for(int i=0; i<theNumberOfEncodings; i++) {
for(int k=0; k<theEncodings[i].getNoOfSpecialCaseInstances(); k++)
theSpecialCaseMap[j++]=i;
}
} else theSpecialCaseMap=NULL;
}

TEContainer::~TEContainer() {
checkPoint();
if(theSpecialCaseMap) delete[] theSpecialCaseMap;
delete[] theEncodings;
if(theScoreLen) delete[] theScoreTable;
}

int TEContainer::prepareSpecialCase(int i) {
checkPoint();
if(i<0 || i>theNumSpecialCases) throw "TEContainer::prepareSpecialCase got invalid parameter";
if(theSpecialCaseMap) {
int j=i,oj=i;
i=theSpecialCaseMap[i];
while(j && theSpecialCaseMap[j-1]==theSpecialCaseMap[j]) j--;
if(oj>j) theEncodings[i].setSpecialCaseInstanceNumber(oj-j);
else theEncodings[i].setSpecialCaseInstanceNumber(0);
}
return(i);
}

int TEContainer::getMethodByDescription(const char* desc) const {
checkPoint();
for(int i=-1; i<theNumSpecialCases; i++) {
if(!strcmp(desc,getDescription(i))) return(i);
} return(-2);
}

const char* TEContainer::getDescription(int specialCaseNo) const {
checkPoint();
int i=specialCaseNo,param=0;
if(theSpecialCaseMap && i>=0) {
int j=i,oj=i;
i=theSpecialCaseMap[i];
while(j && theSpecialCaseMap[j-1]==theSpecialCaseMap[j]) j--;
if(oj>j) param=oj-j;
}
if(i>=0) return(theEncodings[i].getDescription(param));
else if(i==-1) return(L_ISO_CODES);
else if(i==-2) return(L_AUTO_DETECT);
throw "TEContainer::getDescription: This point should never be reached";
}

void TEContainer::resetStates() {
for(int i=0; i<theNumberOfEncodings; i++) theEncodings[i].resetSpecialCaseParams();
}

int TEContainer::setAutoDetectMimeCharsetIfCharsetIsAppropriateToLanguage(const char* mimeDesignator) {
for(int i=0; i<theNumberOfEncodings; i++) {
int detectedSpecialCase=theEncodings[i].setSpecialCaseFromCharsetIfAppropriateToLanguage(mimeDesignator);
if(detectedSpecialCase>=0) {
if(theSpecialCaseMap) {
for(int j=0; j<theNumSpecialCases; j++) {
if(theSpecialCaseMap[j]==i) {
detectedSpecialCase+=j; break;
}
}
theMethod=detectedSpecialCase;
} else theMethod=i;
return theMethod;
}
}
return TEC_InvalidMethod;
}

int TEContainer::getScoreOf(unsigned c) const {
for(int i=0; i<theScoreLen; i++) if(theScoreTable[i]==c) return(i);
return(0);
}

void TEContainer::DetectionStruct::addChar(int b) {
checkPoint();
buf[bPtr++]=b; if(bPtr==DS_BUFLEN)
#ifdef DEBUGGING_TABLES
{
static char tmp[200];
sprintf(tmp,"TEContainer / TEContainer::DetectionStruct: Need to increase DS_BUFLEN - DetectionStruct::addChar b=%d buf=%s",b,buf); // %s OK - debugging
throw(tmp);
}
#else
throw "TEContainer / TEContainer::DetectionStruct: Need to increase DS_BUFLEN";
#endif
buf[bPtr]=0;
} TEContainer::DetectionStruct::DetectionStruct() {
bPtr=needToClearAtEOL=0; inUse=1; score=blackMarks=0L;
specCaseSecondParam=0;
} struct ReturnException {
// because some compilers are a bit vague about this
ReturnException(int val) { theVal=val; }
int theVal;
};
void TEContainer::saveSpecialCaseParam(TEContainer::DetectionStruct* d,int i,int mappedI) {
d[i].specCaseSecondParam=theEncodings[mappedI].getSpecialCaseSecondParameter();
}
void TEContainer::doAutoDetectStuffWith(TEContainer::DetectionStruct* d,int i,int mappedI) {
checkPoint();
theEncodings[mappedI].setSpecialCaseSecondParameterTo(d[i].specCaseSecondParam);
ConversionResult c=theEncodings[mappedI].tryConvertAndSetState(d[i].buf);
int needRecursiveCall=0;
if(c!=CR_NEED_MORE_BYTES) {
int ob=d[i].bPtr; d[i].bPtr=0;
if(c==CR_WAS_MY_IMMED_ISO_SEQUENCE
|| c==CR_WAS_MY_SO_ISO_SEQUENCE
|| c==CR_WAS_MY_SS2_ISO_SEQUENCE
|| c==CR_WAS_MY_SS3_ISO_SEQUENCE
|| c==CR_WAS_ISO_PURE_ASCII) {
throw(new ReturnException(-1));
}
switch(c) {
case CR_SET_SECOND_PARAMETER_TO_EOL:
theEncodings[mappedI].setSpecialCaseSecondParameter();
d[i].needToClearAtEOL=1;
break;
case CR_SUCCESS_WITH_ONE_LESS_BYTE:
d[i].buf[0]=d[i].buf[ob-1]; d[i].buf[1]=0;
d[i].bPtr=1;
needRecursiveCall=1;
// Don't break;
case CR_SUCCESS:
if(TableEncoding::getLastConvertedCharacter()=='\n') {
TableEncoding::setStartOfLineStatus();
if(d[i].needToClearAtEOL) {
theEncodings[mappedI].clearSpecialCaseSecondParameter();
d[i].needToClearAtEOL=0;
}
} else TableEncoding::clearStartOfLineStatus();
if(theScoreLen) {
d[i].score+=getScoreOf(TableEncoding::getLastConvertedCharacter());
if(d[i].score<0) throw "Auto detect frequency table: Score overflow";
} if(needRecursiveCall) { saveSpecialCaseParam(d,i,mappedI); doAutoDetectStuffWith(d,i,mappedI); needRecursiveCall=0; }
case CR_DITCH_THIS_CHARACTER:
case CR_ISO_SS2:
case CR_ISO_SS3:
case CR_ISO_SO:
case CR_ISO_SI: break;
case CR_UTTER_FAILURE: d[i].inUse=0; break;
default: // Failure etc
d[i].blackMarks++;
if(d[i].blackMarks<0) throw "Auto detect: Overflow of counter of invalid characters";
}
}
saveSpecialCaseParam(d,i,mappedI);
}
void blankLineSeekPatch(FILE* fp,long pos);
int TEContainer::autoDetectFrom(FILE* fp
#ifdef DEBUGGING_SCORES
,CGIEnvironment* theEnvironment
#endif
) {
checkPoint();
long oldPos=ftell(fp);
resetStates();
TEContainer::DetectionStruct* d=new TEContainer::DetectionStruct[theNumSpecialCases]();
try {
while(1) {
// 2002-04-08: subEncodingFgetc now depends on
// theMethod (since it does a different thing with
// UTF-8) so 
// **********************************************
wasAmpersand = 0;
int b=subEncodingFgetc(fp); if(b==EOF) break;
for(int i=0; i<theNumSpecialCases; i++) {
if(d[i].inUse && (!wasAmpersand || i!=theNumSpecialCases-1)) {
// see note on wasAmpersand in subEncodingFgetc
// for an explanation of the second half of that
// hacky condition
d[i].addChar(b);
doAutoDetectStuffWith(d,i,prepareSpecialCase(i));
}
}
}
} catch(ReturnException* r) {
// catch(ReturnException r) has a problem with some compilers
// (the parsers think it's a declaration)
delete[] d; resetStates();
blankLineSeekPatch(fp,oldPos);
int x=r->theVal; delete r; return(x);
} catch(...) {
delete[] d; /*resetStates();*/ throw; // 'finally' not supported
}
#ifdef DEBUGGING_SCORES
theEnvironment->h_puts("<!-- Character set detection scores");
#endif
int retVal=0; for(int i=1; i<theNumSpecialCases; i++) if(d[i].inUse) {
#ifdef DEBUGGING_SCORES
theEnvironment->h_printf("retVal=%d (blackMarks=%d score=%d), testing i=%d (%s) (blackMarks=%d score=%d)\n",retVal,d[retVal].blackMarks,d[retVal].score,i,getDescription(i),d[i].blackMarks,d[i].score); // %s OK
#endif
if(d[i].blackMarks<d[retVal].blackMarks
|| d[i].score>d[retVal].score) retVal=i;
}
#ifdef DEBUGGING_SCORES
else theEnvironment->h_printf("skipping %d (%s)\n",i,getDescription(i)); // %s OK
theEnvironment->h_puts("-->");
#endif
delete[] d; resetStates();
blankLineSeekPatch(fp,oldPos); return(retVal);
}

void TEContainer::setMethod(int method) {
if(method<-1 || method>=theNumSpecialCases)
throw "TEContainer::setMethod got invalid parameter";
else theMethod=method;
}

#ifdef MULTIPLE_STYLES_SUPPORTED
const char* TEContainer::getRecommendedStyle(const char* langName) const {
checkPoint();
// Call the TableEncoding::getRecommendedStyle(); if that
// doesn't know, make a judgement based on the language
// name
int method=theMethod;
if(method>=0) {
if(method>theNumSpecialCases) throw "TEContainer::getRecommendedStyle got invalid parameter";
if(theSpecialCaseMap) method=theSpecialCaseMap[method]; // No need to actually tell it which special case we're using (doesn't affect the style)
const char* rec=theEncodings[method].getRecommendedStyle();
if(rec) return rec;
} else {
// We're using ISO codes.  Oops, which method do we ask?
// For now, the following hack will do, since the only
// difference is in the Chinese (simplified vs
// traditional), and the only Chinese mapping that we
// have an ISO designator for is the simplified one.  So
// if we've detected ISO codes then we're in simplified.
// It's slightly less of a hack to ask all the methods
// that *have* ISO codes and take the first answer (at
// least that won't be far wrong if new mapping tables
// are introduced without fixing this)
for(int i=0; i<theNumberOfEncodings; i++) {
if(theEncodings[i].hasISODesignator()) {
const char* rec=theEncodings[i].getRecommendedStyle();
if(rec) return rec;
}
}
}
// No decision yet; check the language
if(!strcmp(langName,"Chinese")) {
// The special cases would have returned Simplified if
// necessary, apart from UTF-8 (which should probably be
// Simplified, 2002-04-08).  Otherwise say Traditional.
if(theMethod==theNumSpecialCases-1) return "s";
return "t";
} else if(!strcmp(langName,"Korean")) return "k";
else return NULL;
// Those literals ("Chinese", "Korean") are a problem if the .tbl names are changed NEEDATTENTION (has been documented in encoding.c++)
// (Really need to put this stuff in the .tbl itself)
}
#endif

int TEContainer::subEncodingFgetc(FILE* fp,int useLtHack) {
checkPoint();
if(theOmitSubEncoding) return(text_fgetc(fp));
// (This is the get-out for "Don't decode HTML escapes
// before interpreting characters" on the "Characters"
// button)
if(thePushbackPtr) return(thePushbackStack[--thePushbackPtr]);
int c=text_fgetc(fp);
if(c=='&') {
c=text_fgetc(fp); if(c=='#') {
// Need to get rid of the not-so-magic numbers here
int isHex=0,result=0; char tmpArray[20]; int tp=0;
tmpArray[tp++]=c=text_fgetc(fp); if(c=='x' || c=='X') { isHex=1; tmpArray[tp++]=c=text_fgetc(fp); }
while(isHex?isxdigit(c):isdigit(c) && tp<15) {
if(isHex) result=((result<<4)|(isalpha(c)?(toupper(c)-'A'+10):(c-'0')));
else result=result*10+c-'0';
tmpArray[tp++]=c=text_fgetc(fp);
} if(c==';') {
// 2000-11-22: NEEDATTENTION
// Word's HTML export in Chinese adds large
// codepoint values here, but this method can only
// return an int, and it leaves the &#... code here.
// OK if imageFilter is called (as long as it isn't
// hex, since imageFilter can't deal with that), and
// passable in UTF-8 output, but awful in Shift-JIS
// output.  Maybe need another
// dest.addString(getStaticUnicodeBuf(x)) somewhere
// in convertLine or somewhere.

// 2002-04-08: Added a hack to never do this thing
// in UTF-8 encoding (to cope with yahoo.com.cn).
// It should be deprecated anyway - older HTML
// composing programs are on the decline and these
// days many people put entities and really MEAN
// entities (not half an EUC character).

// Nasty hack for auto-detect stuff: theMethod won't
// be set correctly, so also set wasAmpersand (which
// causes the character to be ignored in UTF-8 - on
// the yahoo.com.cn site they're just bullet points
// and shouldn't affect the auto-detect score).
// This hackery is horrible.

// UTF-8 if theMethod == theNumSpecialCases-1
if(theMethod!=theNumSpecialCases-1 && (result<' ' || (result>=127 && result<=255))) {
wasAmpersand = 1;
return(result);
}
// else pushBack(';'); // Don't - it will be in tp
} // else pushBack(c); // Instead of the ; (Don't - it will be in tp)
while(tp) pushBack(tmpArray[--tp]);
pushBack('#');
} else if(c=='l' && useLtHack) {
// 2001-01-22: This is a hack for kw10004.  Some sites
// (e.g. Queens college email displaying system)
// have "convert all < characters into &lt;" logic,
// which breaks JIS.  This converts them back (if
// within a multibyte ISO coding).  The hack is, of
// course, disabled by the "Don't decode HTML
// escapes..." option under Characters.
#define HackyStuff(theChar,theCode) c=text_fgetc(fp); if(c==(theChar)) { theCode; pushBack(theChar); } else pushBack(c);
// (horrible, horrible, horrible)
HackyStuff('t',HackyStuff(';',return '<'));
pushBack('l');
} else if(c=='a' && useLtHack) {
// 2001-01-23: We want an &amp; hack as well.  If this
// gets any more complicated, write a proper FSM or
// whatever.
HackyStuff('m',HackyStuff('p',HackyStuff(';',return '&')));
pushBack('a');
} else if(c=='g' && useLtHack) {
// 2001-01-24 - want &gt; too...
HackyStuff('t',HackyStuff(';',return '>'));
pushBack('g');
} else pushBack(c);
return('&');
} else return(c);
}

int TEContainer::convertLine(FILE* source,InString &dest) {
checkPoint();
// Use method theMethod (-1 == ISO) (Map it!)
#define ISO_UNDECIDED (-2)
// isoEncodingInUse is -1 for pure ASCII
// NEEDATTENTION Check the following!
if(theMethod>=0) isoEncodingInUse=prepareSpecialCase(theMethod);
else if(!shiftOutRequired) isoEncodingInUse=-1; // So resets immediate ones at end of line
int needToClearAtEOL=0,hadEbcdicYet=0;
// hadEbcdicYet is a flag to make sure that
// shiftOut is set to 1 for EBCDIC (as this is
// EBCDIC's default)
int getCharNextTime=1,shiftedOut=0,isoToUseNext=isoEncodingInUse;
unsigned isoPlanePrefix=0,planePrefixToUseNext=0,
ss2PlanePrefix=0,ss3PlanePrefix=0;
char buf[DS_BUFLEN]; int bPtr=0;
TableEncoding::setStartOfLineStatus();
LenType oldLength=dest.length();
while(1) { // Main loop (for each character, byte etc)
if(getCharNextTime) {
// Need to get more bytes in
int b=subEncodingFgetc(source,isoNeedsHighBit && theMethod==-1); if(b==EOF) {
// This did say return(1), but that's not necessarily so because there might be something on the last line
return(dest.length()==oldLength);
}
// (Doesn't matter about resetting EOL etc if EOF)
if(theMethod==-1) {
// Using ISO codes, so check if any switching needs to be done
isoToUseNext=isoEncodingInUse;
planePrefixToUseNext=isoPlanePrefix;
if(b==ESCAPE) {
bPtr=1; buf[0]=b; resetStates();
isoNeedsHighBit=1; // unless specified otherwise
isoEncodingInUse=ISO_UNDECIDED;
while(isoEncodingInUse==ISO_UNDECIDED) {
buf[bPtr]=0;
for(int i=0; i<theNumberOfEncodings; i++) {
ConversionResult c=theEncodings[i].tryAndConvert(buf);
switch(c) {
case CR_ONE_BYTE_SPECIAL_CASE: // Eg. Esc ( J
theEncodings[i].setSpecialCaseParameter((SpecialCaseParameter)TableEncoding::getLastConvertedCharacter());
isoNeedsHighBit=0;
isoEncodingInUse=i;
break;
case CR_WAS_MY_IMMED_ISO_SEQUENCE:
shiftOutRequired=0; isoEncodingInUse=i; isoPlanePrefix=TableEncoding::getLastConvertedCharacter();
break; // so shiftoutrequired is 0
case CR_WAS_MY_SO_ISO_SEQUENCE: shiftOutRequired=1; isoEncodingInUse=i; break;
case CR_WAS_MY_SS2_ISO_SEQUENCE: ss2EncodingInUse=i; ss2PlanePrefix=TableEncoding::getLastConvertedCharacter(); break;
case CR_WAS_MY_SS3_ISO_SEQUENCE: ss3EncodingInUse=i; ss3PlanePrefix=TableEncoding::getLastConvertedCharacter(); break;
case CR_WAS_ISO_PURE_ASCII: isoNeedsHighBit=0; isoEncodingInUse=-1; shiftOutRequired=0; break;
case CR_ISO_SS2:
isoEncodingInUse=isoToUseNext;
if(ss2EncodingInUse>=0) {
planePrefixToUseNext=ss2PlanePrefix;
isoToUseNext=ss2EncodingInUse;
} break;
case CR_ISO_SS3:
isoEncodingInUse=isoToUseNext;
if(ss3EncodingInUse>=0) {
planePrefixToUseNext=ss3PlanePrefix;
isoToUseNext=ss3EncodingInUse;
} break;
default: ; // nothing
}
if(isoEncodingInUse!=ISO_UNDECIDED) break;
}
if(isoEncodingInUse!=ISO_UNDECIDED) break;
b=subEncodingFgetc(source,isoNeedsHighBit && theMethod==-1); buf[bPtr++]=b;
if(bPtr==DS_BUFLEN)
#ifdef DEBUGGING_TABLES
{
static char tmp[200];
sprintf(tmp,"TEContainer::convertLine: Need to increase DS_BUFLEN (1) - DetectionStruct::addChar b=%d buf=%d %d %d %d %d %d %d %d %d %d ",b,buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7],buf[8],buf[9]);
throw(tmp);
}
#else
throw "TEContainer::convertLine: Need to increase DS_BUFLEN (unrecognised ISO designator?)";
// This is in a loop that says "while ISO
// undecided, rake in more bytes".  So if
// someone forces ISO codes and they're
// unrecognised, you should get that exception.
// NEEDATTENTION What SHOULD we do here?
// Also, why does it sometimes say "Server
// error", although attempts to reproduce this
// from the command line give valid HTTP
// responses with exceptions?
// eg. QUERY_STRING="Aeld=ISO+codes&Ac=%40&Aelt=Japanese&Aela=3&Aem=3&Au=http%3A%2F%2Flocalhost%2F%7Essb22%2Ftest%2Fsilly.html"
#endif
} // End of while ISO undecided
bPtr=0; continue;
} else if(isoEncodingInUse==-1) {
if(b=='\n') break;
dest.addCharacter(b); continue;
} else {
// b is not escape, isoEncodingInUse != -1
if(isoNeedsHighBit && (!shiftOutRequired || shiftedOut)) {
if(bPtr==1 && planePrefixToUseNext) {
buf[2]=buf[0]; buf[3]=0; bPtr=3;
buf[0]=(planePrefixToUseNext>>8);
buf[1]=(planePrefixToUseNext&0xFF);
}
b|=0x80;
}
}
} // End of if ISO mode
buf[bPtr++]=b;
if(bPtr==DS_BUFLEN)
#ifdef DEBUGGING_TABLES
{
static char tmp[200];
sprintf(tmp,"TEContainer::convertLine: Need to increase DS_BUFLEN (2) - DetectionStruct::addChar b=%d buf=%s",b,buf); // %s OK (debugging)
throw(tmp);
}
#else
throw "TEContainer::convertLine: Need to increase DS_BUFLEN";
#endif
buf[bPtr]=0;
} // End of if(getCharNextTime)
getCharNextTime=1;
ConversionResult c=CR_NO_RESULT_YET;
if(theEncodings[isoToUseNext].isEbcdic()) {
if(!hadEbcdicYet) { shiftedOut=1; // By default
hadEbcdicYet=0; }
if(!shiftedOut && buf[bPtr-1]>=0x20) c=CR_NEED_MORE_BYTES;
} else hadEbcdicYet=0;
if(c==CR_NO_RESULT_YET) c=theEncodings[isoToUseNext].tryConvertAndSetState(buf);
TableEncoding::clearStartOfLineStatus();
int needToReturn=0;
if(c!=CR_NEED_MORE_BYTES) {
int ob=bPtr; bPtr=0;
switch(c) {
case CR_SET_SECOND_PARAMETER_TO_EOL:
if(isoToUseNext!=isoEncodingInUse) throw "Single shifting resulted in SET_SECOND_PARAMETER_TO_EOL";
theEncodings[isoEncodingInUse].setSpecialCaseSecondParameter();
needToClearAtEOL=1;
break;
case CR_SUCCESS_WITH_ONE_LESS_BYTE:
buf[0]=buf[ob-1]; buf[1]=0;
bPtr=1; getCharNextTime=0;
// Don't break;
case CR_SUCCESS: {
unsigned x=TableEncoding::getLastConvertedCharacter();
if(x=='\n') needToReturn=1;
else {
dest.addString(getStaticUnicodeBuf(x));
#ifdef MAKE_FREQUENCY_TABLE
theNewFrequencyTable[x&0xFFFF]++;
#endif
}
break; }
case CR_FAILURE: case CR_UTTER_FAILURE:
dest.addString(getStaticUnicodeBuf(invalidSubChar)); break;
case CR_ISO_SO: shiftedOut=1; break;
case CR_ISO_SI: shiftedOut=0; break;
default: ; 
// Default (ditched character etc)
// is to do nothing
}
}
if(needToReturn) break;
} // End of main loop
if(needToClearAtEOL) theEncodings[isoEncodingInUse].clearSpecialCaseSecondParameter();
return(0);
}
#endif
