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
#ifndef TABLE_ENCODING_H
#define TABLE_ENCODING_H
#include "platform.h"
#ifdef ENCODINGS_AVAILABLE
#include <stdio.h>

// See tecont.h

/// An enumeration for the result of an attempt to convert a character.  See source.
enum ConversionResult {
/// Success: In which case sets lastConvertedCharacter (otherwise lastConvertedCharacter undefined)
CR_SUCCESS,
/// Failure: Might just be an invalid character
CR_FAILURE,
/// Need more bytes: Give it more bytes. (NB If TOO MANY bytes, rest would be discarded, so try 1, then 2 etc) (Assumes len > 0)
CR_NEED_MORE_BYTES,
/// Utter failure: Can't possibly be this encoding
CR_UTTER_FAILURE,
// For all ISOs, lastConvertedCharacter is set to the plane prefix or 0
CR_WAS_MY_IMMED_ISO_SEQUENCE, // Immediate (and cancelled by end of line)
CR_WAS_MY_SO_ISO_SEQUENCE,    // Shift out
CR_WAS_MY_SS2_ISO_SEQUENCE,   // Single shift 2
CR_WAS_MY_SS3_ISO_SEQUENCE,   // Single shift 3
CR_POSS_ISO_BUT_NOT_MINE,     // As it says
CR_ISO_SS2, // The actual SS2 sequence (but only returned if this is ISO-compliant)
CR_ISO_SS3, // Ditto for SS3
CR_ISO_SO,  // Ditto for shift out
// NB shift OUT = "out there" = into the CJK or whatever
// (shift in = back to ASCII etc)
CR_ISO_SI,  // And shift in
CR_WAS_ISO_PURE_ASCII, // Until next ESCAPE
CR_ONE_BYTE_SPECIAL_CASE, // And sets lastConvertedCharacter to special case parameter
CR_DITCH_THIS_CHARACTER,
CR_SET_MY_SECOND_PARAMETER,
CR_SET_SECOND_PARAMETER_TO_EOL, // "until" end of line, that is
CR_CLEAR_MY_SECOND_PARAMETER,
CR_SUCCESS_WITH_ONE_LESS_BYTE, // As CR_SUCCESS but last byte was unprocessed
// (and code assumes that this isn't returned if going onto the next line of i/p)
CR_NO_RESULT_YET // Used internally
};

/// An enumeration of tables that have special cases
enum SpecialCases {
SC_NONE=0,
SC_JAPANESE_EUCJIS,
SC_CHINESE_GB
};

/// An enumeration of the actual special cases
enum SpecialCaseParameter {
SCP_NONE,
SCP_HALF_WIDTH_KATAKANA,
SCP_SHIFT_JIS,
SCP_HZ,
SCP_ZW
};

#define TE_NAMELEN 20
#define TE_MAIN_TABLEN 256
#define TE_HALF_MAIN_TABLEN 128
#define ESCAPE 27
#define ISO_HISTORIC_WATERMARK 'C'
#define ISO_MIN_SO 40
#define ISO_MAX_SO (ISO_MIN_SO+3)
#define invalidSubChar 1 // any Unicode character
/**
TableEncoding: Manages a single encoding table.
WARNING: This class has a few 64K assumptions in
it (unsigned totalValid etc)
*/
class TableEncoding {
#ifdef DEVELOPMENT_COPY
friend void processArguments(int argc,char* argv[]);
#endif
friend void makeJISTable();
public:
TableEncoding() {};
void load(FILE* dataFile);
~TableEncoding();
ConversionResult tryAndConvert(const char* data) const;
/// tryConvertAndSetState handles three of the state changes
ConversionResult tryConvertAndSetState(const char* data);
static unsigned getLastConvertedCharacter() { return(lastConvertedCharacter); }
int getNoOfSpecialCaseInstances() const;
void setSpecialCaseInstanceNumber(int n);
int setSpecialCaseFromCharsetIfAppropriateToLanguage(const char* charset); // Returns the special case that was set, or -1 if failed
const char* getDescription(int specialCaseInstanceNumber) const;
#ifdef MULTIPLE_STYLES_SUPPORTED
const char* getRecommendedStyle() const;
#endif
void setSpecialCaseParameter(SpecialCaseParameter p) { theSpecialCaseParameter=p; }
void setSpecialCaseSecondParameter() { theSecondParameterIsSet=1; }
void clearSpecialCaseSecondParameter() { theSecondParameterIsSet=0; }
int getSpecialCaseSecondParameter() const { return(theSecondParameterIsSet); }
void setSpecialCaseSecondParameterTo(int i) { theSecondParameterIsSet=i; }
void resetSpecialCaseParams();
static void setStartOfLineStatus() { isAtStartOfLine=1; }
static void clearStartOfLineStatus() { isAtStartOfLine=0; }
int isEbcdic() const { return(theMaxBytesPerChar==2 && theMainTable.twoBytes.ebcdic); }
int hasISODesignator() const { return(theISODesignator!=0); }
private:
int checkISODesignator(int c) const;
ConversionResult handleSpecialCaseISOOneByte(char c) const;
ConversionResult handleISOStuff(const char* data) const;
ConversionResult specialCaseConversionHook(const char* data) const;
void setSpecialCaseFromISODesignator();
ConversionResult rawTryAndConvert(const char* data) const;
SpecialCases theSpecialCaseStatus;
SpecialCaseParameter theSpecialCaseParameter;
static unsigned lastConvertedCharacter;
static char isAtStartOfLine;
// Some nasty format-specific stuff here
// - we need fast loading and small memory,
// but not too much compromise on maintainability
char theName[TE_NAMELEN+1];
int theISODesignator; // of first plane
char theSecondParameterIsSet;
int theMaxBytesPerChar;
union {
unsigned short oneByte[TE_MAIN_TABLEN];
// Unicode 0xFFFF is invalid
struct {
/**
validSecondary contains 1, 2, 3...
so can look up by (value-1)<<(mapsAreOnly128Bytes?7:8)
(assumes not ALL valid)
NB if single7BitValidButNoMap, the first
128 bytes of validSecondary are undefined */
unsigned char validSecondary[TE_MAIN_TABLEN];
unsigned short * secondaryMemory;
/// If mapsAreOnly128Bytes, they are from 80-FF ie. the second byte cannot be 7F or below
enum { MAPS256=0,MAPS128=0x80 } mapsAreOnly128Bytes;
/// If single7BitValidButNoMap, first byte
/// cannot be 7F or below; if it is then it
/// goes through unharmed
int single7BitValidButNoMap;
/// If ebcdic, character 14 switches into
/// single-byte mode (default) and character
/// 15 into double-byte mode
/// (Can error-check against getting 14 when in
/// single mode & 15 when in double)
/// (Single byte: Assume MSB 0 ie read 1st table)
/// If it is in ebcdic mode, NB that the presence
/// of a byte 14 or 15 makes it highly likely to be
/// this one (like JIS escape sequence etc)
int ebcdic;
} twoBytes;
struct {
/// extensionByte indicates that there are three more bytes
int extensionByte;
/// firstPlaneSpecifier is also the default plane if only 2 bytes
int firstPlaneSpecifier;
int numSupportedPlanes;
/// NB The code assumes that all this >0x80
//@{
int minFirstByte;
int maxFirstByte;
int minSecondByte;
int maxSecondByte;
//@}
/// totalPlaneSize = (max-min+1)*(max-min+1)
int totalPlaneSize;
/// firstByteSize = max-min+1
int firstByteSize;
/// Use data[plane*totalPlaneSize+b1*firstByteSize+b2]
/// (if all starting from 0, ie. subtract minimum 1st)
unsigned short * data;
} fourBytes;
} theMainTable;
};
#endif
#endif
