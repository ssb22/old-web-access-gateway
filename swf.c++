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

// This is a heavily hacked swfparse.cpp, which extracts
// textual data from swf files.

#include "allinc.h"
#include "access.h"

#include <stdio.h>
#include <string.h>
#include <map>

// Global Types
typedef unsigned long U32, *P_U32, **PP_U32;
typedef signed long S32, *P_S32, **PP_S32;
typedef unsigned short U16, *P_U16, **PP_U16;
typedef signed short S16, *P_S16, **PP_S16;
typedef unsigned char U8, *P_U8, **PP_U8;
typedef signed char S8, *P_S8, **PP_S8;
typedef signed long SFIXED, *P_SFIXED;
typedef signed long SCOORD, *P_SCOORD;
typedef unsigned long BOOL;


typedef struct SPOINT
{
SCOORD x;
SCOORD y;
} SPOINT, *P_SPOINT;

typedef struct SRECT 
{
SCOORD xmin;
SCOORD xmax;
SCOORD ymin;
SCOORD ymax;
} SRECT, *P_SRECT;

// Start Sound Flags
enum {
soundHasInPoint     = 0x01,
soundHasOutPoint    = 0x02,
soundHasLoops       = 0x04,
soundHasEnvelope    = 0x08

// the upper 4 bits are reserved for synchronization flags
};

enum {
fillGradient        =   0x10,
fillLinearGradient  =   0x10,
fillRadialGradient  =   0x12,
fillMaxGradientColors   =   8,
// Texture/bitmap fills
fillBits            =   0x40    // if this bit is set, must be a bitmap pattern
};

// Flags for defining a shape character
enum {
// These flag codes are used for state changes - and as return values from ShapeParser::GetEdge()
eflagsMoveTo       = 0x01,
eflagsFill0        = 0x02,
eflagsFill1        = 0x04,
eflagsLine         = 0x08,
eflagsNewStyles    = 0x10,

eflagsEnd          = 0x80  // a state change with no change marks the end
};

enum FontFlags
{
fontUnicode   = 0x20,
fontShiftJIS  = 0x10,
fontANSI      = 0x08,
fontItalic    = 0x04,
fontBold      = 0x02,
fontWideCodes = 0x01
};

// Edit text field flags
enum
{
sfontFlagsBold          = 0x01,
sfontFlagsItalic        = 0x02,
sfontFlagsWideCodes     = 0x04,
sfontFlagsWideOffsets   = 0x08,
sfontFlagsANSI          = 0x10,
sfontFlagsUnicode       = 0x20,
sfontFlagsShiftJIS      = 0x40,
sfontFlagsHasLayout     = 0x80
};

// Edit Text Flags
enum {
seditTextFlagsHasFont       = 0x0001,
seditTextFlagsHasMaxLength  = 0x0002,
seditTextFlagsHasTextColor  = 0x0004,
seditTextFlagsReadOnly      = 0x0008,
seditTextFlagsPassword      = 0x0010,
seditTextFlagsMultiline     = 0x0020,
seditTextFlagsWordWrap      = 0x0040,
seditTextFlagsHasText       = 0x0080,
seditTextFlagsUseOutlines   = 0x0100,
seditTextFlagsBorder        = 0x0800,
seditTextFlagsNoSelect      = 0x1000,
seditTextFlagsHasLayout     = 0x2000
};


enum TextFlags
{
isTextControl = 0x80,

textHasFont   = 0x08,
textHasColor  = 0x04,
textHasYOffset= 0x02,
textHasXOffset= 0x01
};


typedef struct MATRIX
{
SFIXED a;
SFIXED b;
SFIXED c;
SFIXED d;
SCOORD tx;
SCOORD ty;
} MATRIX, *P_MATRIX;

typedef struct CXFORM
{
/*
S32 flags;
enum
{ 
needA=0x1,  // Set if we need the multiply terms.
needB=0x2   // Set if we need the constant terms.
};
*/

S16 aa, ab;     // a is multiply factor, b is addition factor
S16 ra, rb;
S16 ga, gb;
S16 ba, bb;
}
CXFORM, *P_CXFORM;


#ifndef NULL
#define NULL 0
#endif

// Tag values that represent actions or data in a Flash script.
enum
{ 
stagEnd                 = 0,
stagShowFrame           = 1,
stagDefineShape         = 2,
stagFreeCharacter       = 3,
stagPlaceObject         = 4,
stagRemoveObject        = 5,
stagDefineBits          = 6,
stagDefineButton        = 7,
stagJPEGTables          = 8,
stagSetBackgroundColor  = 9,
stagDefineFont          = 10,
stagDefineText          = 11,
stagDoAction            = 12,
stagDefineFontInfo      = 13,
stagDefineSound         = 14,   // Event sound tags.
stagStartSound          = 15,
stagDefineButtonSound   = 17,
stagSoundStreamHead     = 18,
stagSoundStreamBlock    = 19,
stagDefineBitsLossless  = 20,   // A bitmap using lossless zlib compression.
stagDefineBitsJPEG2     = 21,   // A bitmap using an internal JPEG compression table.
stagDefineShape2        = 22,
stagDefineButtonCxform  = 23,
stagProtect             = 24,   // This file should not be importable for editing.

// These are the new tags for Flash 3.
stagPlaceObject2        = 26,   // The new style place w/ alpha color transform and name.
stagRemoveObject2       = 28,   // A more compact remove object that omits the character tag (just depth).
stagDefineShape3        = 32,   // A shape V3 includes alpha values.
stagDefineText2         = 33,   // A text V2 includes alpha values.
stagDefineButton2       = 34,   // A button V2 includes color transform, alpha and multiple actions
stagDefineBitsJPEG3     = 35,   // A JPEG bitmap with alpha info.
stagDefineBitsLossless2 = 36,   // A lossless bitmap with alpha info.
stagDefineEditText      = 37,   // An editable Text Field
stagDefineSprite        = 39,   // Define a sequence of tags that describe the behavior of a sprite.
stagNameCharacter       = 40,   // Name a character definition, character id and a string, (used for buttons, bitmaps, sprites and sounds).
stagFrameLabel          = 43,   // A string label for the current frame.
stagSoundStreamHead2    = 45,   // For lossless streaming sound, should not have needed this...
stagDefineMorphShape    = 46,   // A morph shape definition
stagDefineFont2         = 48,   // 
};

// PlaceObject2 Flags
enum
{
splaceMove          = 0x01, // this place moves an exisiting object
splaceCharacter     = 0x02, // there is a character tag (if no tag, must be a move)
splaceMatrix        = 0x04, // there is a matrix (matrix)
splaceColorTransform= 0x08, // there is a color transform (cxform with alpha)
splaceRatio         = 0x10, // there is a blend ratio (word)
splaceName          = 0x20, // there is an object name (string)
splaceDefineClip    = 0x40  // this shape should open or close a clipping bracket (character != 0 to open, character == 0 to close)
// one bit left for expansion
};

// Action codes
enum
{
sactionNone                     = 0x00,
sactionNextFrame                = 0x04,
sactionPrevFrame                = 0x05,
sactionPlay                     = 0x06,
sactionStop                     = 0x07,
sactionToggleQuality            = 0x08,
sactionStopSounds               = 0x09,
sactionAdd                      = 0x0A,
sactionSubtract                 = 0x0B,
sactionMultiply                 = 0x0C,
sactionDivide                   = 0x0D,
sactionEqual                    = 0x0E,
sactionLessThan                 = 0x0F,
sactionLogicalAnd               = 0x10,
sactionLogicalOr                = 0x11,
sactionLogicalNot               = 0x12,
sactionStringEqual              = 0x13,
sactionStringLength             = 0x14,
sactionSubString                = 0x15,
sactionInt                      = 0x18,
sactionEval                     = 0x1C,
sactionSetVariable              = 0x1D,
sactionSetTargetExpression      = 0x20,
sactionStringConcat             = 0x21,
sactionGetProperty              = 0x22,
sactionSetProperty              = 0x23,
sactionDuplicateClip            = 0x24,
sactionRemoveClip               = 0x25,
sactionTrace                    = 0x26,
sactionStartDragMovie           = 0x27,
sactionStopDragMovie            = 0x28,
sactionStringLessThan           = 0x29,
sactionRandom                   = 0x30,
sactionMBLength                 = 0x31,
sactionOrd                      = 0x32,
sactionChr                      = 0x33,
sactionGetTimer                 = 0x34,
sactionMBSubString              = 0x35,
sactionMBOrd                    = 0x36,
sactionMBChr                    = 0x37,
sactionHasLength                = 0x80,
sactionGotoFrame                = 0x81, // frame num (WORD)
sactionGetURL                   = 0x83, // url (STR), window (STR)
sactionWaitForFrame             = 0x8A, // frame needed (WORD), 
// actions to skip (BYTE)
sactionSetTarget                = 0x8B, // name (STR)
sactionGotoLabel                = 0x8C, // name (STR)
sactionWaitForFrameExpression   = 0x8D, // frame needed on stack,
// actions to skip (BYTE)
sactionPushData                 = 0x96,
sactionBranchAlways             = 0x99,
sactionGetURL2                  = 0x9A,
sactionBranchIfTrue             = 0x9D,
sactionCallFrame                = 0x9E,
sactionGotoExpression           = 0x9F
};

//////////////////////////////////////////////////////////////////////
// Input script object definition.
//////////////////////////////////////////////////////////////////////

// An input script object.  This object represents a script created from 
// an external file that is meant to be inserted into an output script.
struct CInputScript  
{
// Pointer to file contents buffer.
U8 *m_fileBuf;

AccessOptions* ao;
CGIEnvironment* theEnvironment;
InString* htmlLineInput;

// File state information.
U32 m_filePos;
U32 m_fileSize;
U32 m_fileStart;
U16 m_fileVersion;

// Bit Handling
S32 m_bitPos;
U32 m_bitBuf;

// Tag parsing information.
U32 m_tagStart;
U32 m_tagZero;
U32 m_tagEnd;
U32 m_tagLen;

// Parsing information.
S32 m_nFillBits;
S32 m_nLineBits;


// Font glyph counts (gotta save it somewhere!)
int m_iGlyphCounts[256];

// font glyph mappings -- chris
map<int, map<int, char, less<int> >, less<int> > fontGlyphMappings;

U8* m_srcAdpcm;
U32 m_bitBufAdpcm;      // this should always contain at least 24 bits of data
S32 m_bitPosAdpcm;
U32 m_nSamplesAdpcm;    // number of samples decompressed so far

// Streaming sound info from SoundStreamHead tag
int m_iStreamCompression;
int m_iStreamSampleRate;
int m_iStreamSampleSize;
int m_iStreamStereoMono;
int m_nStreamSamples;

// Constructor/destructor.
CInputScript();
~CInputScript();

// Tag scanning methods.
U16 GetTag(void);
void SkipBytes(int n);
U8 GetByte(void);
U16 GetWord(void);
U32 GetDWord(void);
void GetRect(SRECT *r);
void GetMatrix(MATRIX *matrix);
void GetCxform(CXFORM *cxform, BOOL hasAlpha);
char *GetString(void);
U32 GetColor(BOOL fWithAlpha=false);

void PrintMatrix(MATRIX matrix, char* str);
void PrintRect(SRECT rect, char* str);

// Routines for reading arbitrary sized bit fields from the stream.
// Always call start bits before gettings bits and do not intermix 
// these calls with GetByte, etc... 
void InitBits();
S32 GetSBits(S32 n);
U32 GetBits(S32 n);
void where();

// Tag subcomponent parsing methods
// For shapes
void ParseShapeStyle(BOOL fWithAlpha=false);
BOOL ParseShapeRecord(int& xLast, int& yLast, BOOL fWithAlpha=false);
void ParseButtonRecord(U32 byte, BOOL fGetColorMatrix=true);
BOOL ParseTextRecord(int nGlyphBits, int nAdvanceBits);

// Parsing methods.

void ParseTagidOnly();      // Parse anything which is only a tagid.

void ParseShowFrame(U32 frame, U32 offset);  // 01: stagShowFrame
void ParseDefineShape(BOOL fWithAlpha=false);// 02: stagDefineShape
void ParseFreeCharacter();                     // 03: stagFreeCharacter
void ParsePlaceObject();                       // 04: stagPlaceObject
void ParseRemoveObject();                      // 05: stagRemoveObject
void ParseDefineBits();                        // 06: stagDefineBits
void ParseDefineButton();       //x 07: stagDefineButton
void ParseSetBackgroundColor();                // 09: stagSetBackgroundColor
void ParseDefineFont();         //x 10: stagDefineFont
void ParseDefineText();         //x 11: stagDefineText
void ParseDoAction(BOOL fPrintTag=true);                          // 12: stagDoAction    
void ParseDefineFontInfo();     //x 13: stagDefineFontInfo
void ParseDefineSound();                       // 14: stagDefineSound
void ParseStartSound();                        // 15: stagStartSound
void ParseStopSound();                         // 16: stagStopSound
void ParseDefineButtonSound();                 // 17: stagDefineButtonSound
void ParseSoundStreamHead();                   // 18: stagSoundStreamHead
void ParseSoundStreamBlock();                  // 19: stagSoundStreamBlock
void ParseDefineBitsLossless();                // 20: stagDefineBitsLossless
void ParseDefineBitsJPEG2();                   // 21: stagDefineBitsJPEG2
void ParseDefineButtonCxform();                // 23: stagDefineButtonCxform
void ParseProtect();                           // 24: stagProtect
void ParsePlaceObject2();                      // 26: stagPlaceObject2
void ParseRemoveObject2();                     // 28: stagRemoveObject2
void ParseDefineText2();        //x 33: stagDefineText2
void ParseDefineButton2();      //x 34: stagDefineButton2
void ParseDefineBitsJPEG3();                   // 35: stagDefineBitsJPEG3
void ParseDefineBitsLossless2();               // 36: stagDefineBitsLossless2
void ParseDefineEditText();                    // 37: stagDefineEditText
void ParseDefineSprite();       //x 39: stagDefineSprite
void ParseNameCharacter();                     // 40: stagNameCharacter
void ParseFrameLabel();                        // 43: stagFrameLabel
void ParseSoundStreamHead2(BOOL fIsHead2=true);                  // 45: stagSoundStreamHead2
void ParseDefineMorphShape();   //x 46: stagDefineMorphShape
void ParseDefineFont2();        //x 48: stagDefineFont2
void ParseUnknown(U16 code);
void ParseTags(BOOL sprite, U32 tabs);
BOOL ParseFile(FILE* inFile,AccessOptions* ao,CGIEnvironment* theEnvironment,InString&htmlLineInput);
void S_DumpImageGuts();

// ADPCM stuff
void AdpcmFillBuffer();
long AdpcmGetBits(int n);
long AdpcmGetSBits(int n);
void AdpcmDecompress(long n, long stereo, int if16bit, U8 *dst=NULL);

// MP3 stuff
void DecodeMp3Headers(int iSamplesPerFrame);
void DecodeMp3Frame(U8* pbFrame, int iEncodedSize, int iDecodedSize);
};

#define INDENT

//////////////////////////////////////////////////////////////////////
// Inline input script object methods.
//////////////////////////////////////////////////////////////////////

//
// Inlines to parse a Flash file.
//

inline void CInputScript::SkipBytes(int n) {
m_filePos += n;
}

inline U8 CInputScript::GetByte(void) {
InitBits();
return m_fileBuf[m_filePos++];
}

inline U16 CInputScript::GetWord(void) {
U8* s = m_fileBuf + m_filePos;
m_filePos += 2;
InitBits();
return (U16) s[0] | ((U16) s[1] << 8);
}

inline U32 CInputScript::GetDWord(void) {
U8 * s = m_fileBuf + m_filePos;
m_filePos += 4;
InitBits();
return (U32) s[0] | ((U32) s[1] << 8) | ((U32) s[2] << 16) | ((U32) s [3] << 24);
}

//////////////////////////////////////////////////////////////////////
// Input script object methods.
//////////////////////////////////////////////////////////////////////

CInputScript::CInputScript() {
// Initialize the input pointer.
m_fileBuf = NULL;

// Initialize the file information.
m_filePos = 0;
m_fileSize = 0;
m_fileStart = 0;
m_fileVersion = 0;

// Initialize the bit position and buffer.
m_bitPos = 0;
m_bitBuf = 0;
}


CInputScript::~CInputScript(void)
// Class destructor.
{
// Free the buffer if it is there.
if (m_fileBuf)
{
delete m_fileBuf;
m_fileBuf = NULL;
m_fileSize = 0;
}
}


U16 CInputScript::GetTag(void)
{
// Save the start of the tag.
m_tagStart = m_filePos;
m_tagZero  = m_tagStart;

// Get the combined code and length of the tag.
U16 wRawCode = GetWord();
U16 code = wRawCode;

// The length is encoded in the tag.
U32 len = code & 0x3f;

// Remove the length from the code.
code = code >> 6;

// Determine if another long word must be read to get the length.
if (len == 0x3f)
{
len = (U32) GetDWord();
m_tagZero += 4;
}


// Determine the end position of the tag.
m_tagEnd = m_filePos + (U32) len;
m_tagLen = (U32) len;

return code;
}


void CInputScript::GetRect (SRECT * r)
{
InitBits();
int nBits = (int) GetBits(5);
r->xmin = GetSBits(nBits);
r->xmax = GetSBits(nBits);
r->ymin = GetSBits(nBits);
r->ymax = GetSBits(nBits);
}


void CInputScript::GetMatrix(MATRIX* mat)
{
InitBits();

// Scale terms
if (GetBits(1))
{
int nBits = (int) GetBits(5);
mat->a = GetSBits(nBits);
mat->d = GetSBits(nBits);
}
else
{
mat->a = mat->d = 0x00010000L;
}

// Rotate/skew terms
if (GetBits(1))
{
int nBits = (int)GetBits(5);
mat->b = GetSBits(nBits);
mat->c = GetSBits(nBits);
}
else
{
mat->b = mat->c = 0;
}

// Translate terms
int nBits = (int) GetBits(5);
mat->tx = GetSBits(nBits);
mat->ty = GetSBits(nBits);
}


void CInputScript::GetCxform(CXFORM* cx, BOOL hasAlpha)
{
InitBits();

// !!! The spec has these bits reversed !!!
BOOL fNeedAdd = (GetBits(1) != 0);
BOOL fNeedMul = (GetBits(1) != 0);
// !!! The spec has these bits reversed !!!


int nBits = (int) GetBits(4);

cx->aa = 256; cx->ab = 0;
if (fNeedMul)
{
cx->ra = (S16) GetSBits(nBits);
cx->ga = (S16) GetSBits(nBits);
cx->ba = (S16) GetSBits(nBits);
if (hasAlpha) cx->aa = (S16) GetSBits(nBits);
}
else
{
cx->ra = cx->ga = cx->ba = 256;
}

if (fNeedAdd)
{
cx->rb = (S16) GetSBits(nBits);
cx->gb = (S16) GetSBits(nBits);
cx->bb = (S16) GetSBits(nBits);
if (hasAlpha) cx->ab = (S16) GetSBits(nBits);
}
else
{
cx->rb = cx->gb = cx->bb = 0;
}
}

char *CInputScript::GetString(void)
{
// Point to the string.
char *str = (char *) &m_fileBuf[m_filePos];

// Skip over the string.
while (GetByte());

return str;
}

U32 CInputScript::GetColor(BOOL fWithAlpha)
{
U32 r = GetByte();
U32 g = GetByte();
U32 b = GetByte();
U32 a = 0xff;

if (fWithAlpha)
a = GetByte();

return (a << 24) | (r << 16) | (g << 8) | b;
}

void CInputScript::InitBits(void)
{
// Reset the bit position and buffer.
m_bitPos = 0;
m_bitBuf = 0;
}


S32 CInputScript::GetSBits(S32 n)
// Get n bits from the string with sign extension.
{
// Get the number as an unsigned value.
S32 v = (S32) GetBits(n);

// Is the number negative?
if (v & (1L << (n - 1)))
{
// Yes. Extend the sign.
v |= -1L << n;
}

return v;
}


U32 CInputScript::GetBits (S32 n)
// Get n bits from the stream.
{
U32 v = 0;

while (true)
{
//if (m_bitPos == 0)

S32 s = n - m_bitPos;
if (s > 0)
{
// Consume the entire buffer
v |= m_bitBuf << s;
n -= m_bitPos;

// Get the next buffer
m_bitBuf = GetByte();
m_bitPos = 8;
}
else
{
// Consume a portion of the buffer
v |= m_bitBuf >> -s;
m_bitPos -= n;
m_bitBuf &= 0xff >> (8 - m_bitPos); // mask off the consumed bits

return v;
}
}
}


void CInputScript::ParsePlaceObject()
{
/*U32 tagid = (U32)*/ GetWord();
/*U32 depth = (U32)*/ GetWord();
}


void CInputScript::ParsePlaceObject2()
{
U8 flags = GetByte();
/*U32 depth =*/ GetWord();

// Get the tag if specified.
if (flags & splaceCharacter)
GetWord();

// Get the matrix if specified.
if (flags & splaceMatrix) {
MATRIX matrix;
GetMatrix(&matrix);
}

// Get the color transform if specified.
if (flags & splaceColorTransform) {
CXFORM cxform;
GetCxform(&cxform, true);
}        

// Get the ratio if specified.
if (flags & splaceRatio)
GetWord();

// Get the clipdepth if specified.
if (flags & splaceDefineClip) 
GetWord();

// Get the instance name
if (flags & splaceName)
GetString();
}


void CInputScript::ParseRemoveObject() {
GetWord();
GetWord();
}


void CInputScript::ParseSetBackgroundColor() {
GetByte();
GetByte();
GetByte();
}


BOOL CInputScript::ParseShapeRecord(int& xLast, int& yLast, BOOL fWithAlpha)
{
// Determine if this is an edge.
BOOL isEdge = (BOOL) GetBits(1);

if (!isEdge)
{
// Handle a state change
U16 flags = (U16) GetBits(5);

// Are we at the end?
if (flags == 0)
return true;

// Process a move to.
if (flags & eflagsMoveTo)
{
U16 nBits = (U16) GetBits(5);
S32 x = GetSBits(nBits);
S32 y = GetSBits(nBits);
xLast = x;
yLast = y;
}
// Get new fill info.
if (flags & eflagsFill0)
{
/*int i =*/ GetBits(m_nFillBits);
}
if (flags & eflagsFill1)
{
/*int i =*/ GetBits(m_nFillBits);
}
// Get new line info
if (flags & eflagsLine)
{
/*int i =*/ GetBits(m_nLineBits);
}
// Check to get a new set of styles for a new shape layer.
if (flags & eflagsNewStyles)
{
// Parse the style.
ParseShapeStyle(fWithAlpha);

// Reset.
m_nFillBits = (U16) GetBits(4);
m_nLineBits = (U16) GetBits(4);
}

return flags & eflagsEnd ? true : false;
}
else
{
if (GetBits(1))
{
// Handle a line
U16 nBits = (U16) GetBits(4) + 2;   // nBits is biased by 2

// Save the deltas
if (GetBits(1))
{
// Handle a general line.
S32 x = GetSBits(nBits);
S32 y = GetSBits(nBits);
xLast += x;
yLast += y;
}
else
{
// Handle a vert or horiz line.
if (GetBits(1))
{
// Vertical line
S32 y = GetSBits(nBits);
yLast += y;
}
else
{
// Horizontal line
S32 x = GetSBits(nBits);
xLast += x;
}
}
}
else
{
// Handle a curve
U16 nBits = (U16) GetBits(4) + 2;   // nBits is biased by 2

// Get the control
S32 cx = GetSBits(nBits);
S32 cy = GetSBits(nBits);
xLast += cx;
yLast += cy;

// Get the anchor
S32 ax = GetSBits(nBits);
S32 ay = GetSBits(nBits);
xLast += ax;
yLast += ay;
}

return false;
}
}

void CInputScript::ParseShapeStyle(BOOL fWithAlpha)
{
U16 i = 0;

// Get the number of fills.
U16 nFills = GetByte();

// Do we have a larger number?
if (nFills == 255)
{
// Get the larger number.
nFills = GetWord();
}

// Get each of the fill style.
for (i = 1; i <= nFills; i++) {
U16 fillStyle = GetByte();

if (fillStyle & fillGradient) {
// Get the gradient matrix.
MATRIX mat;
GetMatrix(&mat);

// Get the number of colors.
U16 nColors = (U16) GetByte();

// Get each of the colors.
for (U16 j = 0; j < nColors; j++) {
GetByte();
GetColor(fWithAlpha);
}
} else if (fillStyle & fillBits) {
// Get the bitmap matrix.
/*U16 uBitmapID =*/ GetWord();
MATRIX mat;
GetMatrix(&mat);
} else {
// A solid color
GetColor(fWithAlpha);
}
}

// Get the number of lines.
U16 nLines = GetByte();

// Do we have a larger number?
if (nLines == 255) {
// Get the larger number.
nLines = GetWord();
}

// Get each of the line styles.
for (i = 1; i <= nLines; i++) {
GetWord();
GetColor(fWithAlpha);
}
}

void CInputScript::ParseDefineShape(BOOL fWithAlpha)
{
/*U32 tagid = (U32)*/ GetWord();

// Get the bounding rectangle
SRECT rect;
GetRect(&rect);

ParseShapeStyle(fWithAlpha);

InitBits();     // Bug!  this was not in the original swfparse.cpp
// Required to reset bit counters and read byte aligned.

m_nFillBits = (U16) GetBits(4);
m_nLineBits = (U16) GetBits(4);

int xLast = 0;
int yLast = 0;

BOOL atEnd = false;
while (!atEnd)
atEnd = ParseShapeRecord(xLast, yLast, fWithAlpha);
}


void CInputScript::S_DumpImageGuts()
{
U32 lfCount = 0;                
while (m_filePos < m_tagEnd)
{
lfCount += 1;
GetByte();
}
}

void CInputScript::ParseButtonRecord(U32 iByte, BOOL fGetColorMatrix)
{
//U32 iPad = iByte >> 4;
//U32 iButtonStateHitTest = (iByte & 0x8);
//U32 iButtonStateDown = (iByte & 0x4);
//U32 iButtonStateOver = (iByte & 0x2);
//U32 iButtonStateUp = (iByte & 0x1);

/*U32 iButtonCharacter = (U32)*/GetWord();
/*U32 iButtonLayer = (U32)*/GetWord();

MATRIX matrix;
GetMatrix(&matrix);

if (fGetColorMatrix)
{
// nCharactersInButton always seems to be one
int nCharactersInButton = 1;

for (int i=0; i<nCharactersInButton; i++)
{
CXFORM cxform;
GetCxform(&cxform, true);   // ??could be false here??
}
}
}

void CInputScript::ParseDefineButton()
{
/*U32 tagid = (U32)*/ GetWord();

U32 iButtonEnd = (U32)GetByte();
do
ParseButtonRecord(iButtonEnd, false);
while ((iButtonEnd = (U32)GetByte()) != 0);

// parse ACTIONRECORDs until ActionEndFlag
ParseDoAction(false);
}

void CInputScript::ParseDefineButton2()
{
/*U32 tagid = (U32)*/ GetWord();

/*U32 iTrackAsMenu = (U32)*/ GetByte();

// Get offset to first "Button2ActionCondition"
// This offset is not in the spec!
U32 iOffset = (U32) GetWord();
U32 iNextAction = m_filePos + iOffset - 2;

//
// Parse Button Records
//

U32 iButtonEnd = (U32)GetByte();
do
ParseButtonRecord(iButtonEnd, true);
while ((iButtonEnd = (U32)GetByte()) != 0);

//
// Parse Button2ActionConditions
//

m_filePos = iNextAction;

U32 iActionOffset = 0;
while (true)
{
iActionOffset = (U32) GetWord();
iNextAction  = m_filePos + iActionOffset - 2;

/*U32 iCondition = (U32)*/ GetWord();

// parse ACTIONRECORDs until ActionEndFlag
ParseDoAction(false);

// Action Offset of zero means there's no more
if (iActionOffset == 0)
break;

m_filePos = iNextAction;
}
}

void CInputScript::ParseDoAction(BOOL fPrintTag)
{
for (;;) 
{
// Handle the action
int actionCode = GetByte();
if (actionCode == 0)
{
// Action code of zero indicates end of actions
return;
}

int len = 0;
if (actionCode & sactionHasLength) 
{
len = GetWord();
}        

S32 pos = m_filePos + len;

switch ( actionCode ) 
{
case sactionGotoFrame:
GetWord();
break;

case sactionGetURL:
{
char *url = GetString();
char *target = GetString();
// ao->processTag("P",*htmlLineInput); // NEEDATTENTION List?
theEnvironment->h_putchar(' ');
HTMLTag theTag("A");
theTag.addAttribute("HREF",url);
ao->processTag(theTag,*htmlLineInput);
theEnvironment->h_fputs(target);

// NEEDATTENTION Not all characters in "target"
// will be meaningful
int toAdd=strlen(target);
ao->banner->meaningfulCharacterCounter+=toAdd;
ao->banner->hadMeaningfulThisRow+=toAdd;
ao->banner->hadMeaningfulThisCell+=toAdd;

ao->processTag("/A",*htmlLineInput);
//ao->processTag("/P",*htmlLineInput);
theEnvironment->h_putchar(' ');
break;
}

case sactionWaitForFrame:
GetWord();
GetByte();
break;

case sactionWaitForFrameExpression:
GetByte();
break;

case sactionPushData:
{
U8 dataType = GetByte();

// property ids are pushed as floats for some reason
if ( dataType == 1 )
{
union
{
U32 dw;
float f;
} u;

u.dw = GetDWord();
}

break;
}

case sactionBranchAlways:
GetWord();
break;

case sactionGetURL2:
GetByte();

case sactionBranchIfTrue:
GetWord();
break;

case sactionGotoExpression: 
GetByte();

default:
break;
}

m_filePos = pos;
}
}



void CInputScript::ParseDefineFont()
{
U32 iFontID = (U32)GetWord();
int iStart = m_filePos;
int iOffset = GetWord();

int iGlyphCount = iOffset/2;
m_iGlyphCounts[iFontID] = iGlyphCount;

int* piOffsetTable = new int[iGlyphCount];
piOffsetTable[0] = iOffset;

int n;
for(n=1; n<iGlyphCount; n++)
piOffsetTable[n] = GetWord();

for(n=0; n<iGlyphCount; n++) {
m_filePos = piOffsetTable[n] + iStart;

InitBits(); // reset bit counter

m_nFillBits = (U16) GetBits(4);
m_nLineBits = (U16) GetBits(4);


int xLast = 0;
int yLast = 0;

BOOL fAtEnd = false;

while (!fAtEnd)
fAtEnd = ParseShapeRecord(xLast, yLast);
}

delete piOffsetTable;
}


void CInputScript::ParseDefineFontInfo()
{
U32 iFontID = (U32) GetWord();

int iNameLen = GetByte();
char* pszName = new char[iNameLen+1];
int n;
for(n=0; n < iNameLen; n++)
pszName[n] = (char)GetByte();
pszName[n] = '\0';


delete pszName;

U8 flags = (FontFlags)GetByte();

int iGlyphCount = m_iGlyphCounts[iFontID];

int *piCodeTable = new int[iGlyphCount];

for(n=0; n < iGlyphCount; n++)
{
if (flags & fontWideCodes)
piCodeTable[n] = (int)GetWord();
else
piCodeTable[n] = (int)GetByte();

// save font mappings
fontGlyphMappings[iFontID][n] = piCodeTable[n];
}


delete piCodeTable;
}

BOOL CInputScript::ParseTextRecord(int nGlyphBits, int nAdvanceBits)
{
long fontId = 0;
int iFontHeight = 0;
U8 flags = (TextFlags)GetByte();
if (flags == 0) return 0;

if (flags & isTextControl) {
if (flags & textHasFont)
fontId = GetWord();
if (flags & textHasColor) {
GetByte();
GetByte();
GetByte();
}
if (flags & textHasXOffset)
GetWord();
if (flags & textHasYOffset)
GetWord();
if (flags & textHasFont)
iFontHeight = GetWord();
} else {
int iGlyphCount = flags;

InitBits();     // reset bit counter

if (iFontHeight > 500) ao->processTag("H1",*htmlLineInput);
else ao->processTag("P",*htmlLineInput);
int g;
for (g = 0; g < iGlyphCount; g++) {
int iIndex = GetBits(nGlyphBits);
GetBits(nAdvanceBits);
theEnvironment->h_putchar(fontGlyphMappings[fontId][iIndex]);
}
if (iFontHeight > 500) ao->processTag("/H1",*htmlLineInput);
else ao->processTag("/P",*htmlLineInput);
}

return true;
}


void CInputScript::ParseDefineText()
{
/*U32 tagid = (U32)*/ GetWord();

SRECT   rect;
GetRect(&rect);

MATRIX  m;
GetMatrix(&m);

int nGlyphBits = (int)GetByte();
int nAdvanceBits = (int)GetByte();

BOOL fContinue = true;
do
fContinue = ParseTextRecord(nGlyphBits, nAdvanceBits);
while (fContinue);
}

void CInputScript::ParseDefineEditText()
{
/*U32 tagid = (U32)*/ GetWord();

SRECT rBounds;
GetRect(&rBounds);

U16 flags = GetWord();

if (flags & seditTextFlagsHasFont) {
GetWord();
GetWord();
}

if (flags & seditTextFlagsHasTextColor) {
GetColor(true);
}

if (flags & seditTextFlagsHasMaxLength) {
/*int iMaxLength =*/ GetWord();
}

if (flags & seditTextFlagsHasLayout) {
/*int iAlign =*/ GetByte();
/*U16 uLeftMargin =*/ GetWord();
/*U16 uRightMargin =*/ GetWord();
/*U16 uIndent =*/ GetWord();
/*U16 uLeading =*/ GetWord();
}

/*char* pszVariable =*/ GetString();

if (flags & seditTextFlagsHasText )
/*char* pszInitialText =*/ GetString();
}

void CInputScript::ParseDefineFont2()
{
/*U32 tagid = (U32)*/ GetWord();

U16 flags = GetWord();

// Skip the font name
int iNameLen = GetByte();
char szFontName[256];
int i;
for (i=0; i<iNameLen; i++)
szFontName[i] = GetByte();
szFontName[i] = 0;

// Get the number of glyphs.
U16 nGlyphs = GetWord();

int iDataPos = m_filePos;

if (nGlyphs > 0)
{
//
// Get the FontOffsetTable
//

U32* puOffsetTable = new U32[nGlyphs];
int n;
for (n=0; n<nGlyphs; n++)
if (flags & sfontFlagsWideOffsets)
puOffsetTable[n] = GetDWord();
else
puOffsetTable[n] = GetWord();

//
// Get the CodeOffset
//

U32 iCodeOffset = 0;
if (flags & sfontFlagsWideOffsets)
iCodeOffset = GetDWord();
else
iCodeOffset = GetWord();

//
// Get the Glyphs
//

for(n=0; n<nGlyphs; n++)
{
m_filePos = iDataPos + puOffsetTable[n];

InitBits(); // reset bit counter

m_nFillBits = (U16) GetBits(4);
m_nLineBits = (U16) GetBits(4);

int xLast = 0;
int yLast = 0;

BOOL fAtEnd = false;

while (!fAtEnd)
fAtEnd = ParseShapeRecord(xLast, yLast);
}

delete puOffsetTable;


if (m_filePos != iDataPos + iCodeOffset)
return;

//
// Get the CodeTable
//

m_filePos = iDataPos + iCodeOffset;

for (i=0; i<nGlyphs; i++)
{
if (flags & sfontFlagsWideOffsets)
GetWord();
else
GetByte();

}
}

if (flags & sfontFlagsHasLayout)
{
//
// Get "layout" fields
//

/*S16 iAscent =*/ GetWord();
/*S16 iDescent =*/ GetWord();
/*S16 iLeading =*/ GetWord();

// Skip Advance table
SkipBytes(nGlyphs * 2);


// Get BoundsTable
for (i=0; i<nGlyphs; i++) {
SRECT rBounds;
GetRect(&rBounds);
}

//
// Get Kerning Pairs
//

S16 iKerningCount = GetWord();
for (i=0; i<iKerningCount; i++) {
// U16 iCode1, iCode2;
if (flags & sfontFlagsWideOffsets) {
/*iCode1 =*/ GetWord();
/*iCode2 =*/ GetWord();
} else {
/*iCode1 =*/ GetByte();
/*iCode2 =*/ GetByte();
}
/*S16 iAdjust =*/ GetWord();
}
}
}



void CInputScript::ParseDefineText2()
{
/*U32 tagid = (U32)*/ GetWord();
}

void CInputScript::ParseDefineMorphShape()
{
/*U32 tagid = (U32)*/ GetWord();

SRECT r1, r2;
GetRect(&r1);
GetRect(&r2);

// Calculate the position of the end shape edges
U32 iOffset = GetDWord();
U32 iEndShapePos = m_filePos + iOffset;

// Always get RGBA not RGB for DefineMorphShape
BOOL fGetAlpha = true;

// Get the fills
int nFills = GetByte();
if ( nFills >= 255 )
nFills = GetWord();

int i;
for (i = 1; i <= nFills; i++ )
{
int fillStyle = GetByte();
if (fillStyle & fillGradient)
{
// Gradient fill
MATRIX mat1, mat2;
GetMatrix(&mat1);
GetMatrix(&mat2);

// Get the gradient color points
int nColors = GetByte();
int j;
for (j = 0; j < nColors; j++)
{
U8 r1, r2;
U32 c1, c2;

r1 = GetByte();
c1 = GetColor(fGetAlpha);
r2 = GetByte();
c2 = GetColor(fGetAlpha);
}
}
else if (fillStyle & fillBits)
{
// A bitmap fill
/*U16 tag =*/ GetWord();        // the bitmap tag

MATRIX mat1, mat2;
GetMatrix(&mat1);
GetMatrix(&mat2);
}
else
{
// A solid color
/*U32 rgb1 =*/ GetColor(fGetAlpha);
/*U32 rgb2 =*/ GetColor(fGetAlpha);
}
}

// Get the lines
int nLines = GetByte();
if ( nLines >= 255 )
nLines = GetWord();

for (i = 1; i <= nLines; i++ ) {
U16 thick1, thick2;
U32 rgb1, rgb2;

// get the thickness
thick1 = GetWord();
thick2 = GetWord();

// get the color
rgb1 = GetColor(fGetAlpha);
rgb2 = GetColor(fGetAlpha);
}

//
// Get the bits per style index for the start shape
//

InitBits();
m_nFillBits = (U16) GetBits(4);
m_nLineBits = (U16) GetBits(4);

//
// Parse the start shape
//

BOOL atEnd = false;
int  xLast = 0;
int  yLast = 0;
while (!atEnd)
atEnd = ParseShapeRecord(xLast, yLast, true);

if (m_filePos != iEndShapePos)
return;

//
// Get the bits per style index for the end shape
// THIS IS POINTLESS -- THERE ARE NO STYLES ?!
//

InitBits();
m_nFillBits = (U16) GetBits(4);     // not sure if we should save these to n_FillBits & nLineBits
m_nLineBits = (U16) GetBits(4);     // there are no styles so none of this make sense.

//
// Parse the end shape
//

atEnd = false;
xLast = 0;
yLast = 0;
while (!atEnd)
atEnd = ParseShapeRecord(xLast, yLast, true);
}

//
// MP3 tables
//

int vertab[4]={2,3,1,0};
int freqtab[4]={44100,48000,32000};
int ratetab[2][3][16]=
{
{
{  0, 32, 64, 96,128,160,192,224,256,288,320,352,384,416,448,  0},
{  0, 32, 48, 56, 64, 80, 96,112,128,160,192,224,256,320,384,  0},
{  0, 32, 40, 48, 56, 64, 80, 96,112,128,160,192,224,256,320,  0},
},
{
{  0, 32, 48, 56, 64, 80, 96,112,128,144,160,176,192,224,256,  0},
{  0,  8, 16, 24, 32, 40, 48, 56, 64, 80, 96,112,128,144,160,  0},
{  0,  8, 16, 24, 32, 40, 48, 56, 64, 80, 96,112,128,144,160,  0},
},
};

void CInputScript::DecodeMp3Frame(U8* pbFrame, int iEncodedSize, int iDecodedSize)
{
// This is left as an exercise for the reader (he he)
}

void CInputScript::DecodeMp3Headers(int iSamplesPerFrame)
{
int iFrameCount = 0;

if (iSamplesPerFrame > 0)
{
while (true)
{
// Get the MP3 frame header
U8  hdr[4];
for (int i=0; i<4; i++)
hdr[i] = GetByte();

// Decode the MP3 frame header
int ver     = vertab[((hdr[1] >> 3) & 3)];
int layer   = 3 - ((hdr[1] >> 1) & 3);
int pad     = (hdr[2] >>1 ) & 1;
int stereo  = ((hdr[3] >> 6) & 3) != 3;
int freq    = 0;
int rate    = 0;

if (hdr[0] != 0xFF || hdr[1] < 0xE0 || ver==3 || layer != 2)
{
// bad MP3 header
break;
}
else
{
freq = freqtab[(hdr[2] >>2 ) & 3] >> ver;
rate = ratetab[ver ? 1 : 0][layer][(hdr[2] >> 4) & 15] * 1000;

if (!freq || !rate)
{
// bad MP3 header
break;
}
}

// Get the size of a decoded MP3 frame
int iDecodedFrameSize = (576 * (stereo + 1));
if (!ver)
iDecodedFrameSize *= 2;

// Get the size of this encoded MP3 frame
int iEncodedFrameSize = ((ver ? 72 : 144) * rate) / freq + pad - 4;

//char* ppszMpegVer[4] = {"1","2","2.5","3?"};

// Decode the MP3 frame
DecodeMp3Frame(&m_fileBuf[m_filePos], iEncodedFrameSize, iDecodedFrameSize);

// Move to the next frame
iFrameCount++;
if (m_filePos + iEncodedFrameSize >= m_tagEnd)
break;
m_filePos += iEncodedFrameSize;
}
}
}

//
// ADPCM tables
//

static const int indexTable2[2] = {
-1, 2,
};

// Is this ok?
static const int indexTable3[4] = {
-1, -1, 2, 4,
};

static const int indexTable4[8] = {
-1, -1, -1, -1, 2, 4, 6, 8,
};

static const int indexTable5[16] = {
-1, -1, -1, -1, -1, -1, -1, -1, 1, 2, 4, 6, 8, 10, 13, 16,
};

static const int* indexTables[] = {
indexTable2,
indexTable3,
indexTable4,
indexTable5
};

static const int stepsizeTable[89] = {
7, 8, 9, 10, 11, 12, 13, 14, 16, 17,
19, 21, 23, 25, 28, 31, 34, 37, 41, 45,
50, 55, 60, 66, 73, 80, 88, 97, 107, 118,
130, 143, 157, 173, 190, 209, 230, 253, 279, 307,
337, 371, 408, 449, 494, 544, 598, 658, 724, 796,
876, 963, 1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066,
2272, 2499, 2749, 3024, 3327, 3660, 4026, 4428, 4871, 5358,
5894, 6484, 7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899,
15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767
};



void CInputScript::AdpcmFillBuffer()
{
while (m_bitPosAdpcm <= 24)
{
m_bitBufAdpcm = (m_bitBufAdpcm<<8) | *m_srcAdpcm++;
m_bitPosAdpcm += 8;
}
}

long CInputScript::AdpcmGetBits(int n)
{
if (m_bitPosAdpcm < n)
AdpcmFillBuffer();

//assert(bitPos >= n);

long v = ((U32)m_bitBufAdpcm << (32-m_bitPosAdpcm)) >> (32-n);
m_bitPosAdpcm -= n;

return v;
}

long CInputScript::AdpcmGetSBits(int n)
{
if (m_bitPosAdpcm < n)
AdpcmFillBuffer();

//assert(bitPos >= n);

long v = ((S32)m_bitBufAdpcm << (32-m_bitPosAdpcm)) >> (32-n);
m_bitPosAdpcm -= n;

return v;
}


void CInputScript::AdpcmDecompress(long n, long stereo, int f16Bit, U8* data)
{
long    valpred[2]; // Current state
int     index[2];

for (int i=0; i<2; i++)
{
valpred[i] = 0;
index[i] = 0;
}

// int iSampleCount = n;

// Get the compression header
int nBits = (int)GetBits(2)+2;

const int* indexTable = indexTables[nBits-2];
int k0 = 1 << (nBits-2);
int signmask = 1 << (nBits-1);


if (f16Bit)
{
short*  dst = (short*)data;

if (!stereo)
{
// Optimize for mono
long    vp = valpred[0]; // maybe these can get into registers...
int     ind = index[0];
long    ns = m_nSamplesAdpcm;

while ( n-- > 0 )
{
ns++;

if ((ns & 0xFFF) == 1)
{
// Get a new block header
vp = GetSBits(16);
if (dst != NULL)
*dst++ = (short)vp;

ind = (int)GetBits(6); // The first sample in a block does not have a delta

}
else
{
// Process a delta value
int delta = (int)GetBits(nBits);

// Compute difference and new predicted value
// Computes 'vpdiff = (delta+0.5)*step/4'
int step = stepsizeTable[ind];
long vpdiff = 0;
int k = k0;

do
{
if (delta & k)
vpdiff += step;
step >>= 1;
k >>= 1;
}
while (k);

vpdiff += step; // add 0.5

if (delta & signmask) // the sign bit
vp -= vpdiff;
else
vp += vpdiff;

// Find new index value
ind += indexTable[delta & (~signmask)];

if (ind < 0)
ind = 0;
else if (ind > 88)
ind = 88;

// clamp output value
if (vp != (short)vp)
vp = vp < 0 ? -32768 : 32767;

/* Step 7 - Output value */
if (dst != NULL)
*dst++ = (short)vp;

}
}

valpred[0] = vp;
index[0] = ind;
m_nSamplesAdpcm = ns;
}
else
{
int sn = stereo ? 2 : 1;

// Stereo
while ( n-- > 0 ) {

m_nSamplesAdpcm++;

if ((m_nSamplesAdpcm & 0xFFF) == 1 )
{
// Get a new block header
for ( int i = 0; i < sn; i++ )
{
valpred[i] = GetSBits(16);
if (dst != NULL)
*dst++ = (short)valpred[i];

// The first sample in a block does not have a delta
index[i] = (int)GetBits(6);
}
}
else
{
// Process a delta value
for ( int i = 0; i < sn; i++ ) {
int delta = (int)GetBits(nBits);

// Compute difference and new predicted value
// Computes 'vpdiff = (delta+0.5)*step/4'

int step = stepsizeTable[index[i]];
long vpdiff = 0;
int k = k0;

do {
if ( delta & k ) vpdiff += step;
step >>= 1;
k >>= 1;
} while ( k );
vpdiff += step; // add 0.5


if ( delta & signmask ) // the sign bit
valpred[i] -= vpdiff;
else
valpred[i] += vpdiff;

// Find new index value
index[i] += indexTable[delta&(~signmask)];

if ( index[i] < 0 )
index[i] = 0;
else if ( index[i] > 88 )
index[i] = 88;

// clamp output value
if ( valpred[i] != (short)valpred[i] )
valpred[i] = valpred[i] < 0 ? -32768 : 32767;


/* Step 7 - Output value */
if (dst != NULL)
*dst++ = (short)valpred[i];
}
}
}
}
}
else
{
U8* dst = data;
// U8* pbSamples = dst;

if (!stereo)
{
// Optimize for mono
long    vp = valpred[0]; // maybe these can get into registers...
int     ind = index[0];
long    ns = m_nSamplesAdpcm;

while ( n-- > 0 )
{
ns++;

if ((ns & 0xFFF) == 1)
{
// Get a new block header
vp = GetBits(8);
if (dst != NULL)
*dst++ = (U8)vp;

ind = (int)GetBits(6); // The first sample in a block does not have a delta

}
else
{
// Process a delta value
int delta = (int)GetBits(nBits);

// Compute difference and new predicted value
// Computes 'vpdiff = (delta+0.5)*step/4'
int step = stepsizeTable[ind];
long vpdiff = 0;
int k = k0;

do
{
if (delta & k)
vpdiff += step;
step >>= 1;
k >>= 1;
}
while (k);

vpdiff += step; // add 0.5

if (delta & signmask) // the sign bit
vp -= vpdiff;
else
vp += vpdiff;

// Find new index value
ind += indexTable[delta & (~signmask)];

if (ind < 0)
ind = 0;
else if (ind > 88)
ind = 88;

// clamp output value
if (vp < 0)
vp = 0;
else if (vp > 255)
vp = 255;

/* Step 7 - Output value */
if (dst != NULL)
*dst++ = (U8)vp;
}
}

valpred[0] = vp;
index[0] = ind;
m_nSamplesAdpcm = ns;

}
else
{
int sn = stereo ? 2 : 1;

// Stereo
while ( n-- > 0 ) {

m_nSamplesAdpcm++;

if ((m_nSamplesAdpcm & 0xFFF) == 1 )
{
// Get a new block header
for ( int i = 0; i < sn; i++ )
{
valpred[i] = GetBits(8);
if (dst != NULL)
*dst++ = (U8)valpred[i];

// The first sample in a block does not have a delta
index[i] = (int)GetBits(6);
}
}
else
{
// Process a delta value
for ( int i = 0; i < sn; i++ )
{
int delta = (int)GetBits(nBits);

// Compute difference and new predicted value
// Computes 'vpdiff = (delta+0.5)*step/4'

int step = stepsizeTable[index[i]];
long vpdiff = 0;
int k = k0;

do {
if ( delta & k ) vpdiff += step;
step >>= 1;
k >>= 1;
} while ( k );
vpdiff += step; // add 0.5


if ( delta & signmask ) // the sign bit
valpred[i] -= vpdiff;
else
valpred[i] += vpdiff;

// Find new index value
index[i] += indexTable[delta&(~signmask)];

if ( index[i] < 0 )
index[i] = 0;
else if ( index[i] > 88 )
index[i] = 88;

// clamp output value
if ( valpred[i] != (short)valpred[i] )
valpred[i] = valpred[i] < 0 ? -32768 : 32767;


/* Step 7 - Output value */
if (dst != NULL)
*dst++ = (U8)valpred[i];
}
}
}
}
}
}



void CInputScript::ParseDefineSound()
{
/*U32 tagid = (U32)*/ GetWord();

/*int iCompression =*/ GetBits(4);      // uncompressed, ADPCM or MP3
/*int iSampleRate  =*/ GetBits(2);
/*int iSampleSize  =*/ GetBits(1);
/*int iStereoMono  =*/ GetBits(1);
/*int iSampleCount =*/ GetDWord();


// char* ppszCompression[3] = {"uncompressed", "ADPCM", "MP3"};
// char* ppszSampleRate[4]  = {"5.5", "11", "22", "44"};
// const char* pszSampleSize      = (iSampleSize == 0 ? "8" : "16");
// const char* pszStereoMono      = (iStereoMono == 0 ? "mono" : "stereo");
}


void CInputScript::ParseSoundStreamHead()
{
ParseSoundStreamHead2(false);
}

void CInputScript::ParseSoundStreamHead2(BOOL fIsHead2)
{
/*int iMixFormat =*/ GetByte();

// The stream settings
m_iStreamCompression = GetBits(4);
m_iStreamSampleRate  = GetBits(2);
m_iStreamSampleSize  = GetBits(1);
m_iStreamStereoMono  = GetBits(1);
m_nStreamSamples     = GetWord();

//char* ppszCompression[3] = {"uncompressed", "ADPCM", "MP3"};
//char* ppszSampleRate[4]  = {"5.5", "11", "22", "44"};
//const char* pszSampleSize      = (m_iStreamSampleSize == 0 ? "8" : "16");
//const char* pszStereoMono      = (m_iStreamStereoMono == 0 ? "mono" : "stereo");
}

void CInputScript::ParseSoundStreamBlock()
{
switch (m_iStreamCompression)
{
case 0:
{
break;
}
case 1:
{
m_nSamplesAdpcm = 0;
m_srcAdpcm = &m_fileBuf[m_filePos];
AdpcmDecompress(m_nStreamSamples, m_iStreamStereoMono, m_iStreamSampleSize);
break;
}
case 2:
{
U16 iSamplesPerFrame  = GetWord();
/*U16 iDelay  =*/ GetWord();
DecodeMp3Headers(iSamplesPerFrame);
}
}
}


void CInputScript::ParseTagidOnly() {
GetWord();
}

void CInputScript::ParseNameCharacter() {
/*U32 tagid = (U32)*/ GetWord();
/*char *label =*/ GetString();
}

void CInputScript::ParseFrameLabel() {
/*char *label =*/ GetString();
}


void CInputScript::ParseTags(BOOL sprite, U32 tabs) {
//char str[33] = {0};   // indent level

if (sprite) {
/*U32 tagid = (U32)*/ GetWord();
/*U32 frameCount = (U32)*/ GetWord();
}
else
// Set the position to the start position.
m_filePos = m_fileStart;

// Initialize the end of frame flag.
BOOL atEnd = false;

// Reset the frame position.
U32 frame = 0;

// Loop through each tag.
while (!atEnd)
{
// Get the current tag.
U16 code = GetTag();

// Get the tag ending position.
U32 tagEnd = m_tagEnd;
switch (code)
{
case stagEnd:
// We reached the end of the file.
atEnd = true;
break;

case stagShowFrame:
// Increment to the next frame.
++frame;
break;

case stagFreeCharacter:
ParseTagidOnly();
break;

case stagPlaceObject:
ParsePlaceObject();
break;

case stagPlaceObject2:
ParsePlaceObject2();
break;

case stagRemoveObject:
ParseRemoveObject();
break;

case stagRemoveObject2:
ParseTagidOnly();
break;

case stagSetBackgroundColor:
ParseSetBackgroundColor();
break;

case stagDoAction:
ParseDoAction();
break;

case stagStartSound:
ParseTagidOnly();
break;

case stagDefineShape:
case stagDefineShape2:
ParseDefineShape();
break;

case stagDefineShape3:
ParseDefineShape(true);
break;

case stagDefineBits:
case stagDefineBitsJPEG2:
case stagDefineBitsJPEG3:
case stagDefineBitsLossless2:
case stagDefineBitsLossless:
ParseTagidOnly();
break;

case stagDefineButton:
ParseDefineButton();
break;

case stagDefineButton2:
ParseDefineButton2();
break;

case stagDefineFont:
ParseDefineFont();
break;

case stagDefineMorphShape:
ParseDefineMorphShape();
break;

case stagDefineFontInfo:
ParseDefineFontInfo();
break;

case stagDefineText:
ParseDefineText();
break;

case stagDefineText2:
ParseDefineText2();
break;

case stagDefineSound:
ParseDefineSound();
break;

case stagDefineEditText:
ParseDefineEditText();
break;

case stagDefineButtonSound:
ParseTagidOnly();
break;

case stagSoundStreamHead:
ParseSoundStreamHead();
break;

case stagSoundStreamHead2:
ParseSoundStreamHead2();
break;

case stagSoundStreamBlock:
ParseSoundStreamBlock();
break;

case stagDefineButtonCxform:
ParseTagidOnly();
break;

case stagDefineSprite:
ParseTags(true, tabs + 1);
break;

case stagNameCharacter:
ParseNameCharacter();
break;

case stagFrameLabel:
ParseFrameLabel();
break;

case stagDefineFont2:
ParseDefineFont2();
break;

default:
break;
}

// Increment the past the tag.
m_filePos = tagEnd;
}
}


BOOL CInputScript::ParseFile(FILE* inputFile,AccessOptions* ao,CGIEnvironment* theEnvironment,InString&htmlLineInput) {
this->ao = ao;
this->theEnvironment = theEnvironment;
this->htmlLineInput = &htmlLineInput;

U8 fileHdr[8];
BOOL sts = true;

// Free the buffer if it is there.
if (m_fileBuf != NULL) {
delete m_fileBuf;
m_fileBuf = NULL;
m_fileSize = 0;
}

// Read the file header.
if (fread(fileHdr, 1, 8, inputFile) != 8) sts = false;

// Are we OK?
if (sts) {
// Verify the header and get the file size.
if (fileHdr[0] != 'F' || fileHdr[1] != 'W' || fileHdr[2] != 'S' )
// Bad header.
sts = false;
else
// Get the file version.
m_fileVersion = (U16) fileHdr[3];
}

// Are we OK?
if (sts) {
// Get the file size.
m_fileSize = (U32) fileHdr[4] | ((U32) fileHdr[5] << 8) | ((U32) fileHdr[6] << 16) | ((U32) fileHdr[7] << 24);

// Verify the minimum length of a Flash file.
if (m_fileSize < 21) {
sts = false;
}
}

// Are we OK?
if (sts) {
// Allocate the buffer.
m_fileBuf = new U8[m_fileSize];

// Is there data in the file?
if (m_fileBuf == NULL)
sts = false;
}

// Are we OK?
if (sts) {
// Copy the data already read from the file.
memcpy(m_fileBuf, fileHdr, 8);

// Read the file into the buffer.
if (fread(&m_fileBuf[8], 1, m_fileSize - 8, inputFile) != (m_fileSize - 8))
sts = false;
}

// Are we OK?
if (sts) {
SRECT rect;

// Set the file position past the header and size information.
m_filePos = 8;

// Get the frame information.
GetRect(&rect);
/*U32 frameRate =*/ GetWord() /*>> 8*/;

/*U32 frameCount =*/ GetWord();

// Set the start position.
m_fileStart = m_filePos;    

fflush(stdout);

// Parse the tags within the file.
ParseTags(false, 0);
}

// Free the buffer if it is there.
if (m_fileBuf != NULL) {
delete m_fileBuf;
m_fileBuf = NULL;
}

// Reset the file information.
m_filePos = 0;
m_fileSize = 0;
m_fileStart = 0;
m_fileVersion = 0;

// Reset the bit position and buffer.
m_bitPos = 0;
m_bitBuf = 0;

return sts;
}

void swf2html(FILE* inFile,AccessOptions* ao,CGIEnvironment* theEnvironment,InString&htmlLineInput) {
CInputScript* pInputScript=new CInputScript();
pInputScript->ParseFile(inFile,ao,theEnvironment,htmlLineInput);
delete pInputScript;
}
