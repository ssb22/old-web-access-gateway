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
#include "utf.h"
#include "debug.h"

char* getStaticUtf8Buf(unsigned x) {
checkPoint();
// Might as well write out all the cases - need fast execution
#define BitLess(x,y) (!((x)&(~(y))))
#define BottomSix(x) (0x80|((x)&0x3F))
#define SixBits(x,s) BottomSix((x)>>(s))
#define DoBuf(i,x,s) (buf[i]=SixBits(x,s))
#define FinBuf(i,x) (buf[i]=BottomSix(x), buf[(i)+1]=0)
static char buf[7];
if(BitLess(x,0x7F)) { buf[0]=x; buf[1]=0; }
else if(BitLess(x,0x7FF)) { buf[0]=(0xC0|(x>>6)); FinBuf(1,x); }
else if(BitLess(x,0xFFFF)) { buf[0]=(0xE0|(x>>12)); DoBuf(1,x,6); FinBuf(2,x); }
else if(BitLess(x,0x1FFFFF)) { buf[0]=(0xF0|(x>>18)); DoBuf(1,x,12); DoBuf(2,x,6); FinBuf(3,x); }
else if(BitLess(x,0x3FFFFFF)) { buf[0]=(0xF8|(x>>24)); DoBuf(1,x,18); DoBuf(2,x,12); DoBuf(3,x,6); FinBuf(4,x); }
else if(BitLess(x,0x7FFFFFFF)) { buf[0]=(0xFC|(x>>30)); DoBuf(1,x,24); DoBuf(2,x,18); DoBuf(3,x,12); DoBuf(4,x,6); FinBuf(5,x); }
else throw "getStaticUtf8Buf: Invalid UCS character";
return buf;
}

ConversionResult utf8Convert(unsigned &output,const unsigned char* input) {
checkPoint();
// Again we want fast execution.  This is a bit hacky
// (no pun intended)                        ^^^
if(!(input[0]&0x80)) {
// A single byte
output=input[0]; return CR_SUCCESS;
} else if(!(input[0]&0xC0)) {
// UTF-8 characters never start in the range 0x80-0xBF
return CR_UTTER_FAILURE;
} else
// Now, lots of macros...
// (Might as well use the preprocessor to help unroll
// things)
#define TailRange(x) (((x)&0xC0)==0x80)
#define ChkTail(n) if(!input[n]) return CR_NEED_MORE_BYTES; else if(!TailRange(input[n])) return CR_UTTER_FAILURE
#define ChkTail1 ChkTail(1)
#define ChkTail2 ChkTail1; else ChkTail(2)
#define ChkTail3 ChkTail2; else ChkTail(3)
#define ChkTail4 ChkTail3; else ChkTail(4)
#define ChkTail5 ChkTail4; else ChkTail(5)
#define AddTail(n,s) ((input[n]&0x3F)<<s)
#define EndTail(n) (input[n]&0x3F)
#define AddTail1 EndTail(1)
#define AddTail2 (AddTail(1,6)|EndTail(2))
#define AddTail3 (AddTail(1,12)|AddTail(2,6)|EndTail(3))
#define AddTail4 (AddTail(1,18)|AddTail(2,12)|AddTail(3,6)|EndTail(4))
#define AddTail5 (AddTail(1,24)|AddTail(2,18)|AddTail(3,12)|AddTail(4,6)|EndTail(5))
#define UtfTry(test,shift,check,extra) (!(input[0]&(test))) { check; else { output=(((input[0]&((test)-1))<<(shift))|(extra)); return CR_SUCCESS; } }
// UtfTry should start with 'if', but emacs' editing
// mode didn't like all those unmatched 'else'
// constructs, so keep the 'if' out of the macro
if UtfTry(0x20,6,ChkTail1,AddTail1) else
if UtfTry(0x10,12,ChkTail2,AddTail2) else
if UtfTry(0x08,18,ChkTail3,AddTail3) else
if UtfTry(0x04,24,ChkTail4,AddTail4) else
if UtfTry(0x02,30,ChkTail5,AddTail5) else
return CR_UTTER_FAILURE;
}
#endif
